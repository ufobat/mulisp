//
// Created by Matthieu Felix on 01/10/2016.
//

#include "mulisp.h"

#define MAX_MACRO_DEPTH 100

// NOT A REAL ENVIRONMENT. It's just a glorified hash table.
Environment *syntaxes;

/*
 * Initializes the syntax hash table.
 */
void init_syntax()
{
    syntaxes = new_env(NULL);
}

/*
 * Defines a new set of syntax-rules.
 */
void define_syntax(char *syntax_name, Object *syntax_rules)
{
    // TODO: check the validity of the syntax_rules.
    define_object(syntax_name, syntax_rules, syntaxes);
}

/*
 * Checks if a symbol is the name of a syntax.
 */
int is_syntax(char *symbol_name)
{
    return get_entry(symbol_name, syntaxes) != NULL;
}


/*
 * Given an expression and a list of syntax rules, finds the appropriate
 * syntax rule and expands the expression into it
 */
Object *expand_syntax_rule(Object *expr)
{

}


/*
 * Expands all the macros in the expression. Will stop if depth reaches
 * MAX_MACRO_DEPTH.
 */
Object *expand_syntax_max(Object *expr, int depth)
{
    if (depth > MAX_MACRO_DEPTH)
        fatal_error("Max macro depth reached");

    if (is_atom(expr))
        return expr;

    if (expr->pair.car->type == OTYPE_SYM &&
            is_syntax(expr->pair.car->str.value)) {
        Object *iter = expand_syntax_rule(expr);
        return expand_syntax_max(expr, depth+1);
    }
    else {
        return expr;
    }
}

/*
 * Returns the expression, expanded
 */
Object *expand_syntax(Object *expr)
{
    return expand_syntax_max(expr, 0);
}