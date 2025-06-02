/**
 * \file eval.c
 * Expression evaluation.
 * The evaluator is stack-based to enable tail-call optimization.
 */

#include <string.h>
#include "mulisp.h"

/**
 * \def STACK_SIZE
 * STACK_SIZE is the maximum size of the stack, i.e. the maximum depth of
 * recursive calls assuming no TCO can be performed. There are two stacks,
 * the instruction stack, on which tail calls are pushed, and the return stack,
 * on which returned values are pushed.
 */
#define STACK_SIZE 100

/**
 * e_stack_instr_type represents a type of instruction, either apply a function
 * (INSTR_APPLY) or evaluate an object (INSTR_EVAL).
 */
enum e_stack_instr_type
{
    INSTR_EVAL, INSTR_APPLY
};

/**
 * s_stack_instr represents an intruction on the stack, including the type of
 * instruction, the environment in which to perform the evaluation, and
 * the object to evaluate/function to apply and its parameter list.
 * If the type is `INSTR_EVAL`, the parameters should be `NULL`.
 */
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


/**
 * print_ret_stack prints the return stack.
 */
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


/**
 * instr_stack_push pushes an instruction on the instruction stack.
 * @param type The type of instruction to be pushed
 * @param env The environment in which the instruction should be executed
 * @param obj The object to evaluate/function to call
 * @param parameters The parameters of the function. This argument can be NULL.
 */
void instr_stack_push(enum e_stack_instr_type type, Environment *env,
                      Object *obj, Object *parameters)
{
    if (instr_stack_pointer > STACK_SIZE)
        fatal_error("Instruction stack overflow.\n");

    instr_stack[instr_stack_pointer].type = type;
    instr_stack[instr_stack_pointer].env = env;
    instr_stack[instr_stack_pointer].obj = obj;
    instr_stack[instr_stack_pointer].parameters = parameters;
    instr_stack_pointer++;

    //printf("Instr stack at %d\n", instr_stack_pointer);
}


/**
 * instr_stack_pop returns the top element on the instruction stack and
 * decreases the stack pointer.
 * @return The top instruction on the instruction stack.
 */
StackInstr instr_stack_pop()
{
    if (instr_stack_pointer <= 0)
        fatal_error("Instruction stack underflow.\n");
    StackInstr x = instr_stack[--instr_stack_pointer];
    return x;
}


/**
 * ret_stack_push pushes an object on top of the return stack, and increments
 * the return stack pointer.
 * @param obj The object to push on the return stack.
 */
void ret_stack_push(Object *obj)
{
    if (ret_stack_pointer > STACK_SIZE)
        fatal_error("Return stack overflow.\n");
    ret_stack[ret_stack_pointer++] = obj;

    //print_ret_stack();
}


/**
 * ret_stack_pop gets the object on top of the instruction stack, and decrements
 * the return stack pointer.
 * @return The object that was on top of the instruction stack.
 */
Object *ret_stack_pop()
{
    if (ret_stack_pointer <= 0)
        fatal_error("Return stack underflow.\n");

    //printf("Popped\n");
    return ret_stack[--ret_stack_pointer];
}



/**
 * map_args binds function arguments.
 * map_args will walk through formals and parameters assuming that both
 * have the same list/sub-list structure. When a symbol is found in `formals`,
 * that symbol is bound to the corresponding element of `parameters` in the
 * given environment.
 *
 * A few examples of `formals`, `parameters`, and the resulting bindings:
 *
 * | Formals     | Parameters    | Bindings               |
 * |-------------|---------------|------------------------|
 * | `a`         | `1`           | a: `1`                 |
 * | `a`         | `(1 2 3)`     | a: `(1 2 3)`           |
 * | `(a b)`     | `(1 2)`       | a: `1`; b: `2`         |
 * | `(a b)`     | `(1 2 3)`     | Error                  |
 * | `(a . b)`   | `(1 2 3)`     | a: `1`; b: `(2 3)`     |
 * | `(a . b)`   | `(1)`         | a: `1`; b: `()`        |
 * | `((a b) c)` | `((1 2) 3)`   | a: `1`; b: `2`; c: `3` |
 * | `((a b) c)` | `((1 2) 3 4)` | Error                  |
 *
 * @param formals A formal argument list
 * @param parameters A list of arguments
 * @param env The environment in which to create the bindings
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


/**
 * map_eval evaluates all items in a list and returns a new list with these
 * items.
 * @param list The list to whose elements to evaluate
 * @param env The environment in which to perform the evaluations
 * @return A new list with evaluated members
 */
Object *map_eval(Object *list, Environment *env)
{
    Object *ret = nil;

    if(list->type == OTYPE_NIL)
        return nil;
    else if(list->type == OTYPE_PAIR) {
        ret = malloc(sizeof(Object));
        ret->type = OTYPE_PAIR;
        ret->pair.car = eval(list->pair.car, env);
        ret->pair.cdr = map_eval(list->pair.cdr, env);
    }
    else
        fatal_error("Malformed function call\n");

    return ret;
}


/**
 * st_apply applies a function by evaluating all its parameters, mapping them
 * to the formal arguments, and pushing a new eval instruction for the body
 * of the function. If the passed function is a primitive procedure, it is
 * evaluated immediately with no prior argument evaluation.
 *
 * @param function The function to evaluate
 * @param parameters The parameters passed to the function
 * @param env The environment in which the function was called
 */
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
            Object *ret = nil;
            Object *iter = to_eval->pair.cdr;

            if(iter == nil) {
                ret_stack_push(ret);
                return;
            }

            while (iter->pair.cdr != nil) {
                eval(iter->pair.car, env);
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

            function = eval(to_eval->pair.car, env);
            parameters = map_eval(to_eval->pair.cdr, env);
            instr_stack_push(INSTR_APPLY, env, function, parameters);
        }


        break;


    default:
        fatal_error("Unknown type");
        ret_stack_push(nil);
    }
}


/**
 * eval pushes a new evaluation instruction on the stack, then repeatedly
 * pops the instruction stacks and performs the appropriate action (apply
 * or eval) until the stack is empty, then returns the result on the return
 * stack.
 * @param to_eval The object to evaluate
 * @param env The environment in which to perform the evaluation
 * @return The result of the evaluation
 */
Object *eval(Object *to_eval, Environment *env)
{
    instr_stack_push(INSTR_EVAL, env, to_eval, NULL);

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
