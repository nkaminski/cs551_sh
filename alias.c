#include "alias.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void dl_init(dlist_head_t *q){
    //sanity check
    assert(q != NULL);
    //initialize queue
    q->size = 0;
    q->head=NULL;
    q->tail=NULL;
}


int dl_push(dlist_head_t *q, void* dataptr){
    dlist_obj_t *nu_obj;
    //push an object
    nu_obj = (dlist_obj_t *)malloc(sizeof(dlist_obj_t));
    if(!nu_obj)
        return -1;
    //Assign data pointer
    nu_obj->payload = dataptr;
    //New object always at head
    nu_obj->prev = NULL;
    //New next object is the current head
    nu_obj->next = q->head;
    //if > 1 item, next block's prev points to new obj
    if(nu_obj->next)
        (nu_obj->next)->prev = nu_obj;
    //move head to current new object
    q->head = nu_obj;
    //if first item, tail points to nu_obj as well
    if(!q->tail)
        q->tail = nu_obj;
    (q->size)++;
    return q->size;
}

void* dl_pop(dlist_head_t *q){
    dlist_obj_t *dq_obj;
    void *retval;
    if(!q->tail)
        return NULL;
    //Dequeue the last object
    dq_obj = q->tail;
    //Move tail pointer back one
    q->tail = dq_obj->prev;
    //Set new tail's next to NULL if it exists
    if(q->tail)
        (q->tail)->next = NULL;
    else
        q->head = NULL;
    //save payload pointer and free the q node
    retval = dq_obj->payload;
    free(dq_obj);
    (q->size)--;
    return retval;
}

void* dl_peek(dlist_head_t *q){
    dlist_obj_t *dq_obj;
    void *retval;
    if(!q->tail)
        return NULL;
    //access the last object
    dq_obj = q->tail;
    retval = dq_obj->payload;
    return retval;
}

void* dl_remove(dlist_head_t *q, dlist_obj_t *ob){
    void *data;
    if(ob == NULL){
        return NULL;
    }
    if(ob->prev){
        /* Has a previous element */
        ob->prev->next = ob->next;
    } else {
        /* was first element */
        q->head = ob->next;
    }
    /* repeated logic as before */
    if(ob->next){
        ob->next->prev = ob->prev;
    } else {
        q->tail = ob->prev;
    }
    data = ob->payload;
    free(ob);
    (q->size)--;
    return data;
}

alias_t* dl_find_alias(dlist_head_t *q, const char *tgt){
    dlist_obj_t *curobj = q->head;
    alias_t * al;
    while(curobj != NULL){
        assert(curobj->payload != NULL);
        al = (alias_t *)curobj->payload;
        if(strcmp(al->alias_src, tgt) == 0){
            return al;
        }
        curobj=curobj->next;
    }
    return NULL;
}
int dl_find_remove_alias(dlist_head_t *q, const char *tgt){
    dlist_obj_t *curobj = q->head;
    alias_t * al;
    while(curobj != NULL){
        assert(curobj->payload != NULL);
        al = (alias_t *)curobj->payload;
        if(strcmp(al->alias_src, tgt) == 0){
            free(curobj->payload);
            dl_remove(q, curobj);
            return 1;
        }
        curobj=curobj->next;
    }
    return 0;
}

void dl_empty(dlist_head_t *q){
    //remove everything
    while(dl_pop(q));
}

/* resolve_alias - tests if the command matches a defined alias and if so,
 * replaces the command with the contents of the alias.
 */
void resolve_alias(char *cmd){
    alias_t *al;
    if(cmd[strlen(cmd)-1] == '\n')
	    cmd[strlen(cmd)-1] = '\0';  /* replace trailing '\n' with null */
    al = dl_find_alias(&alias_list, cmd);
    if(al != NULL){
        strncpy(cmd,al->alias_dest, MAXLINE-1);
    }
}

/*
 * set_alias - Set an alias
 */
void set_alias(char **argv)
{
    alias_t *nualias;
    if(argv[1]==NULL || argv[2] == NULL){
		printf("%s command requires an argument in the form alias aliasname \'command\'\n",argv[0]);
		return;
	}
    //Remove the alias if it already exists
	dl_find_remove_alias(&alias_list, argv[1]);
    // ! removes the alias
    if(argv[2][0] == '!')
        return;
    if(strcmp(argv[1],"alias") == 0){
	printf("\nThe name \'alias\' is reserved and cannot be used as an alias name\n");
	return;
 }
    //Allocate and populate
    nualias = malloc(sizeof(alias_t));
    if(nualias == NULL){
        printf("Unable to add alias \n");
        return;
    }
    strncpy(nualias->alias_src, argv[1], MAXLINE-1);
    strncpy(nualias->alias_dest, argv[2], MAXLINE-1);
    dl_push(&alias_list, nualias);
    printf("\nAlias #%d %s -> ""%s""\n",alias_list.size, nualias->alias_src, nualias->alias_dest);
    return;
}

void print_aliases(char **argv){
    dlist_obj_t *curobj = alias_list.head;
    alias_t * al;
    while(curobj != NULL){
        assert(curobj->payload != NULL);
        al = (alias_t *)curobj->payload;
        printf("alias %s -> \'%s\'\n",al->alias_src, al->alias_dest);
        curobj=curobj->next;
    }
}

void save_aliases(const char *filename){
    dlist_obj_t *curobj = alias_list.head;
    alias_t * al;
    printf("\nSaving aliases to %s\n",filename);
	FILE *fp = fopen(filename,"w");
    if(fp == NULL){
        perror("unable to save aliases file");
        return;
    }
    while(curobj != NULL){
        assert(curobj->payload != NULL);
        al = (alias_t *)curobj->payload;
        fprintf(fp,"%s\x01%s\n",al->alias_src, al->alias_dest);
        curobj=curobj->next;
    }
    fclose(fp);
}

void load_aliases(const char *filename){
    alias_t * al;
    char alias_str[MAXLINE];
    char *alias_strp = alias_str;
    char *asrc;
	FILE *fp = fopen(filename,"r");
    if(fp == NULL){
        return;
    }
    while(fgets(alias_str, MAXLINE, fp) != NULL){
        alias_strp = alias_str;
        asrc = strsep(&(alias_strp), "\x01");
        al = malloc(sizeof(alias_t));
        if(al == NULL || alias_strp == NULL){
            printf("Error adding aliases from aliases file\n");
            return;
        }
        if(alias_strp[strlen(alias_strp)-1] == '\n')
	        alias_strp[strlen(alias_strp)-1] = '\0';  /* replace trailing '\n' with null */

        strncpy(al->alias_src, asrc, MAXLINE-1);
        strncpy(al->alias_dest, alias_strp, MAXLINE-1);
        if(dl_push(&alias_list, al) < 0){
            printf("Failed to add aliases to alias list\n");
            return;
        }
    }   
    fclose(fp);
}

