/**
 * \file env.c
 * Functions to deal with environments.
 * Environments are nested, with bindings for the current environment stored
 * in a hash table. Bindings of the parent environments are accessible through
 * the descendant environment.
 */

#include <string.h>
#include "mulisp.h"


/**
 * get_hash returns the hash key for a string.
 * @param str The string for which to compute the hash
 * @return The hash of the string
 */
unsigned get_hash(char *str)
{
    unsigned valhash = 0;
    int i;

    for (i = 0; i < strlen(str); i++)
        valhash = str[i] + 31 * valhash;

    return valhash % HASHTBL_SIZE;
}


/**
 * env_entry_lookup performs the lookup of an identifier in an environment. It
 * will not perform lookups in parent environments.
 * @param identifier The identifier to look up
 * @param env The environment in which to perform the lookup
 * @return The hashtable entry associated with the identifier, or `NULL` if it
 * was not found.
 */
struct s_hashtable_entry *env_entry_lookup(char *identifier, Environment *env)
{
    unsigned hash = get_hash(identifier);
    struct s_hashtable_entry *entry = env->bindings[hash];

    while (entry != NULL) {
        if (!strcmp(entry->key, identifier))
            return entry;
        entry = entry->next;
    }

    return NULL;
}


/**
 * get_entry performs an identifier lookup in an environment and all its
 * parents.
 * @param identifier The identifier to look up
 * @param env The environment in which to perform the lookup
 * @return The hashtable entry associated with the identifier, or `NULL` if it
 * is not found.
 */
struct s_hashtable_entry *get_entry(char *identifier, Environment *env)
{
    struct s_hashtable_entry *ret = NULL;

    while (env != NULL) {
        ret = env_entry_lookup(identifier, env);
        if (ret != NULL)
            return ret;
        env = env->parent;
    }

    return NULL;
}


/**
 * get_object returns the object associated with an identifier in an
 * environment. The lookup is also performed in the parents of the environment
 * as well.
 * @param identifier The identifier to look up
 * @param env The environment in which to perform the lookup
 * @return The object associated with the identifier, or `NULL` if it is not
 * found
 */
Object *get_object(char *identifier, Environment *env)
{
    struct s_hashtable_entry *entry = get_entry(identifier, env);
    if(entry == NULL)
        fatal_error("Undefined identifier\n");

    Object *ret = entry->refer;
    if(ret == NULL)
        fatal_error("Accessing unspecific binding %s\n", identifier);

    return ret;
}


/**
 * define_object associates an object with an identifier in an environment.
 * @param identifier The identifier with which to identify the object.
 * @param obj The object to associate with the identifier
 * @param env The environment in which to perform the binding
 */
void define_object(char *identifier, Object *obj, Environment *env)
{
    unsigned hash = get_hash(identifier);
    struct s_hashtable_entry *entry = env->bindings[hash];
    struct s_hashtable_entry *new_entry;

    new_entry = malloc(sizeof(struct s_hashtable_entry));
    new_entry->key = malloc(sizeof(char) * (strlen(identifier) + 1));
    strncpy(new_entry->key, identifier, strlen(identifier) + 1);
    new_entry->refer = obj;
    new_entry->next = NULL;

    if (entry == NULL) {
        env->bindings[hash] = new_entry;
        return;
    }

    while (entry->next != NULL)
        entry = entry->next;

    entry->next = new_entry;
}


/**
 * set_object changes the binding of an identifier in an environment. If the
 * binding is not found in the lowest level of the environment, the operation
 * will be applied recursively to parent environments. It is an error to call
 * set_object on an identifier that does not exist in the environment (or its
 * parents).
 * @param identifier The identifier whose binding to change
 * @param obj The new object to bind to the identifier
 * @param env The environment in which to perform the binding change.
 */
void set_object(char *identifier, Object *obj, Environment *env)
{
    struct s_hashtable_entry *entry = get_entry(identifier, env);

    if (entry == NULL)
        fatal_error("Trying to set undefined identifier %s\n", identifier);

    entry->refer = obj;
}


/**
 * new_env creates a new environment.
 * @param parent The parent to associate with the environment. Can be set to
 * `NULL` if the environment is the root environment.
 * @return The newly-created environment
 */
Environment *new_env(Environment *parent)
{
    Environment *new_env = malloc(sizeof(Environment));
    int i;

    for (i = 0; i < HASHTBL_SIZE; i++)
        new_env->bindings[i] = NULL;

    new_env->parent = parent;

    return new_env;
}


/**
 * delete_env frees an environment and all its entry structures and key strings.
 * @param env The environment to free.
 */
void delete_env(Environment *env)
{
    int i;
    struct s_hashtable_entry *entry, *prev_entry;

    for(i = 0; i < HASHTBL_SIZE; i++) {
        entry = env->bindings[i];
        while(entry != NULL) {
            prev_entry = entry;
            free(entry->key);
            entry = entry->next;
            free(prev_entry);
        }
    }

    free(env);
}