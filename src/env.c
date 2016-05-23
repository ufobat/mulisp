#include <string.h>
#include "mulisp.h"


unsigned get_hash(char *str)
{
    unsigned valhash = 0;
    int i;

    for (i = 0; i < strlen(str); i++)
        valhash = str[i] + 31 * valhash;

    return valhash % HASHTBL_SIZE;
}


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


void set_object(char *identifier, Object *obj, Environment *env)
{
    struct s_hashtable_entry *entry = get_entry(identifier, env);

    if (entry == NULL)
        fatal_error("Trying to set undefined identifier %s\n", identifier);

    entry->refer = obj;
}


Environment *new_env(Environment *parent)
{
    Environment *new_env = malloc(sizeof(Environment));
    int i;

    for (i = 0; i < HASHTBL_SIZE; i++)
        new_env->bindings[i] = NULL;

    new_env->parent = parent;

    return new_env;
}


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