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


void instr_stack_push(enum e_stack_instr_type type, Environment *env, Object *obj, Object *parameters)
{
    if (instr_stack_pointer > STACK_SIZE)
        fatal_error("Instruction stack overflow\n");

    instr_stack[instr_stack_pointer].type = type;
    instr_stack[instr_stack_pointer].env = env;
    instr_stack[instr_stack_pointer].obj = obj;
    instr_stack[instr_stack_pointer].parameters = parameters;
    instr_stack_pointer++;
}

StackInstr instr_stack_pop()
{
    if (instr_stack_pointer <= 0)
        fatal_error("Instruction stack underflow\n");
    return instr_stack[--instr_stack_pointer];
}


void ret_stack_push(Object *obj)
{
    if (ret_stack_pointer > STACK_SIZE)
        fatal_error("Return stack overflow\n");
    ret_stack[ret_stack_pointer++] = obj;
}

Object *ret_stack_pop()
{
    if (ret_stack_pointer <= 0)
        fatal_error("Return stack underflow\n");
    return ret_stack[--ret_stack_pointer];
}


void st_apply(Object *function, Object *parameters, Environment *env)
{

}


void st_eval(Object *to_eval, Environment *env)
{
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

        if (to_eval->pair.car->type == OTYPE_SYM) {
            char *sym = to_eval->pair.car->str.value;

            if (!strcmp(sym, "quote")) {
                ret_stack_push(to_eval->pair.cdr->pair.car);
            }
            else if (!strcmp(sym, "begin")) {
                Object *ret = nil;
                Object *iter = to_eval->pair.cdr;

                while (iter != nil) {
                    st_eval(iter->pair.car, env);
                    iter = iter->pair.cdr;
                    ret = ret_stack_pop();
                }

                ret_stack_push(ret);
            }
            else if (!strcmp(sym, "define")) {
                Object *identifier_obj = to_eval->pair.cdr->pair.car;

                st_eval(to_eval->pair.cdr->pair.cdr->pair.car, env);
                define_object(identifier_obj->str.value, ret_stack_pop(), env);

                ret_stack_push(nil);
            }
            else if (!strcmp(sym, "set!")) {
                Object *identifier_obj = to_eval->pair.cdr->pair.car;

                st_eval(to_eval->pair.cdr->pair.cdr->pair.car, env);
                set_object(identifier_obj->str.value, ret_stack_pop(), env);

                ret_stack_push(nil);
            }
            else if (!strcmp(sym, "if")) {
                Object *cond;

                st_eval(to_eval->pair.cdr->pair.car, env);
                cond = ret_stack_pop();

                if (cond->type == OTYPE_BOOL && !cond->boolean.value) {
                    if (to_eval->pair.cdr->pair.cdr->pair.cdr != nil)
                        instr_stack_push(INSTR_EVAL, env, to_eval->pair.cdr->pair.cdr->pair.cdr->pair.car, NULL);
                    else
                        ret_stack_push(nil);
                }
                else
                    instr_stack_push(INSTR_EVAL, env, to_eval->pair.cdr->pair.cdr->pair.car, NULL);
            }
            else if (!strcmp(sym, "lambda")) {
                Object *lambda = malloc(sizeof(Object));
                Object *body = malloc(sizeof(Object));

                body->type = OTYPE_PAIR;
                body->pair.car = make_symbol("begin");
                body->pair.cdr = to_eval->pair.cdr->pair.cdr;

                lambda->type = OTYPE_PROC;
                lambda->proc.arguments = to_eval->pair.cdr->pair.car;
                lambda->proc.body = body;
                lambda->proc.env = env;
            }
            else
                fatal_error("Not implemented\n");
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