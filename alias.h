#include "mylimits.h"

//Sentinel and list object 

typedef struct dlist_obj{
    void *payload;
    struct dlist_obj *prev;
    struct dlist_obj *next;
} dlist_obj_t;

typedef struct {
    char alias_src[MAXLINE];
    char alias_dest[MAXLINE];
} alias_t;

typedef struct {
    int size;
    dlist_obj_t *head;
    dlist_obj_t *tail;
} dlist_head_t;

extern dlist_head_t alias_list;
extern char aliasesfile[];

void set_alias(char **argv);
void resolve_alias(char *cmd);
void print_aliases(char **argv);
void load_aliases(const char *filename);
void save_aliases(const char *filename);


void dl_init(dlist_head_t *q);
int dl_push(dlist_head_t *q, void* dataptr);
void* dl_pop(dlist_head_t *q);
void* dl_peek(dlist_head_t *q);
void* dl_remove(dlist_head_t *q, dlist_obj_t *ob);
alias_t* dl_find_alias(dlist_head_t *q, const char *tgt);
int dl_find_remove_alias(dlist_head_t *q, const char *tgt);
