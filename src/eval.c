#include <string.h>
#include "mulisp.h"

#define STACK_SIZE 1000

enum e_stack_instr_type
{
    INSTR_EVAL, INSTR_APPLY
};

typedef struct s_stack_instr
{
    enum e_stack_instr_type type;
    Environment *env;
    Object *obj; // Either to function to apply or the object to evaluate
    Object *parameters;
} StackInstr;

StackInstr instr_stack[STACK_SIZE];
int instr_stack_pointer = 0;

Object *ret_stack[STACK_SIZE];
int ret_stack_pointer = 0;


void print_ret_stack()
{
    int i;

    for (i = 0; i < ret_stack_pointer; i++) {
        write_item(ret_stack[i]);
        if (i != ret_stack_pointer - 1)
            printf(" | ");
    }
    printf("\n");
}


void instr_stack_push(enum e_stack_instr_type type, Environment *env, Object *obj, Object *parameters)
{
    if (instr_stack_pointer > STACK_SIZE)
        fatal_error("Instruction stack overflow.\n");

    instr_stack[instr_stack_pointer].type = type;
    instr_stack[instr_stack_pointer].env = env;
    instr_stack[instr_stack_pointer].obj = obj;
    instr_stack[instr_stack_pointer].parameters = parameters;
    instr_stack_pointer++;
}

StackInstr instr_stack_pop()
{
    if (instr_stack_pointer <= 0)
        fatal_error("Instruction stack underflow.\n");
    return instr_stack[--instr_stack_pointer];
}


void ret_stack_push(Object *obj)
{
    if (ret_stack_pointer > STACK_SIZE)
        fatal_error("Return stack overflow.\n");
    ret_stack[ret_stack_pointer++] = obj;

    //print_ret_stack();
}

Object *ret_stack_pop()
{
    if (ret_stack_pointer <= 0)
        fatal_error("Return stack underflow.\n");

    //printf("Popped\n");
    return ret_stack[--ret_stack_pointer];
}



/* Binds arguments to argument lists
 */
void map_args(Object *formals, Object *parameters, Environment *env)
{
    if (formals->type == OTYPE_SYM) {
        define_object(formals->str.value, parameters, env);
    }
    else if (formals->type == OTYPE_PAIR) {
        if (parameters->type != OTYPE_PAIR)
            fatal_error("Parameter destructuring error\n");
        map_args(formals->pair.car, parameters->pair.car, env);
        map_args(formals->pair.cdr, parameters->pair.cdr, env);
    }
    else if (formals->type == OTYPE_NIL) {
        if (parameters->type != OTYPE_NIL)
            fatal_error("Parameter destructuring error\n");
    }
    else
        fatal_error("Argument lists should only contain symbols and pairs\n");
}

void st_eval(Object *to_eval, Environment *env);


/* Evaluates all items in a list, returns a new list with these items */
Object *map_eval(Object *list, Environment *env)
{
    Object *ret = nil;

    if(list->type == OTYPE_NIL)
        return nil;
    else if(list->type == OTYPE_PAIR) {
        ret = malloc(sizeof(Object));
        ret->type = OTYPE_PAIR;
        st_eval(list->pair.car, env);
        ret->pair.car = ret_stack_pop();
        ret->pair.cdr = map_eval(list->pair.cdr, env);
    }
    else
        fatal_error("Malformed function call\n");

    return ret;
}


void st_apply(Object *function, Object *parameters, Environment *env)
{
    Environment *running_env;

    if(function->type == OTYPE_PRIM) {
        ret_stack_push((function->prim.f)(parameters, env));
    }
    else if(function->type == OTYPE_PROC) {
        running_env = new_env(function->proc.env);

        map_args(function->proc.arguments, parameters, running_env);
        instr_stack_push(INSTR_EVAL, running_env, function->proc.body, NULL);
        // TODO: push an instruction to delete running_env
    }
    else
        fatal_error("Tried to apply a non-function\n");
}


void st_eval(Object *to_eval, Environment *env)
{
    int is_sym;
    char *sym;

    switch (to_eval->type) {
    case OTYPE_STR:
    case OTYPE_NIL:
    case OTYPE_BOOL:
    case OTYPE_FLT:
    case OTYPE_INT:
    case OTYPE_FRAC:
    case OTYPE_CHR:
    case OTYPE_PRIM:
    case OTYPE_PROC:
        ret_stack_push(to_eval);
        break;

    case OTYPE_SYM:
        ret_stack_push(get_object(to_eval->str.value, env));
        break;

    case OTYPE_PAIR:
        /* This is the end
         * Hold your breath and count to ten */

        /* TODO: check all instruction structures are correct */
        is_sym = (to_eval->pair.car->type == OTYPE_SYM);
        sym = to_eval->pair.car->str.value;

        if (is_sym && !strcmp(sym, "quote")) {
            ret_stack_push(to_eval->pair.cdr->pair.car);
        }
        else if (is_sym && !strcmp(sym, "begin")) {
            /* TODO: Not properly tail recursive. This is a fairly major problem. */
            Object *ret = nil;
            Object *iter = to_eval->pair.cdr;

            if(iter == nil) {
                ret_stack_push(ret);
                return;
            }

            while (iter->pair.cdr != nil) {
                st_eval(iter->pair.car, env);
                iter = iter->pair.cdr;
                ret_stack_pop();
            }

            instr_stack_push(INSTR_EVAL, env, iter->pair.car, NULL);
        }
        else if (is_sym && !strcmp(sym, "define")) {
            Object *identifier_obj = to_eval->pair.cdr->pair.car;
            Object *new_value = eval(to_eval->pair.cdr->pair.cdr->pair.car,
                                     env);

            define_object(identifier_obj->str.value, new_value, env);

            ret_stack_push(nil);
        }
        else if (is_sym && !strcmp(sym, "set!")) {
            Object *identifier_obj = to_eval->pair.cdr->pair.car;
            Object *new_value = eval(to_eval->pair.cdr->pair.cdr->pair.car,
                                     env);

            set_object(identifier_obj->str.value, new_value, env);

            ret_stack_push(nil);
        }
        else if (is_sym && !strcmp(sym, "if")) {
            Object *cond;

            cond = eval(to_eval->pair.cdr->pair.car, env);

            if (cond->type == OTYPE_BOOL && !cond->boolean.value) {
                if (to_eval->pair.cdr->pair.cdr->pair.cdr != nil)
                    instr_stack_push(INSTR_EVAL, env, to_eval->pair.cdr->pair.cdr->pair.cdr->pair.car, NULL);
                else
                    ret_stack_push(nil);
            }
            else
                instr_stack_push(INSTR_EVAL, env, to_eval->pair.cdr->pair.cdr->pair.car, NULL);
        }
        else if (is_sym && !strcmp(sym, "lambda")) {
            Object *lambda = malloc(sizeof(Object));
            Object *body = malloc(sizeof(Object));
            Environment *lambda_env = new_env(env);

            body->type = OTYPE_PAIR;
            body->pair.car = make_symbol("begin");
            body->pair.cdr = to_eval->pair.cdr->pair.cdr;

            lambda->type = OTYPE_PROC;
            lambda->proc.arguments = to_eval->pair.cdr->pair.car;
            lambda->proc.body = body;
            lambda->proc.env = lambda_env;

            ret_stack_push(lambda);
        }
        else {
            Object *function;
            Object *parameters;

            st_eval(to_eval->pair.car, env);
            function = ret_stack_pop();
            parameters = map_eval(to_eval->pair.cdr, env);
            instr_stack_push(INSTR_APPLY, env, function, parameters);
        }


        break;


    default:
        fatal_error("Unknown type");
        ret_stack_push(nil);
    }
}

Object *dispatch()
{
    StackInstr to_exec;

    while (instr_stack_pointer != 0) {
        to_exec = instr_stack_pop();
        if (to_exec.type == INSTR_EVAL)
            st_eval(to_exec.obj, to_exec.env);
        else
            st_apply(to_exec.obj, to_exec.parameters, to_exec.env);
    }

    return ret_stack_pop();
}

Object *eval(Object *to_eval, Environment *env)
{
    instr_stack_push(INSTR_EVAL, env, to_eval, NULL);

    return dispatch();
}
