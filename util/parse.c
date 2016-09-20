#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*
int strcut(char *blob, char *substr){
    assert(substr >= blob);
    assert(substr <= blob+strlen(blob));
    //Cuts a substring out from within another string
    int sublen = strlen(substr);
    memmove(substr,substr+sublen,sublen);
    return strlen(blob);
}
*/

char* parens(const char* instr, size_t *len){
    char *lparen, *rparen;
    lparen = strchr(instr,'(');
    rparen = strrchr(instr,')');
    if(lparen == NULL && rparen == NULL){
        //Base case
        printf("%s\n",instr);
        return instr;
    } else if (lparen && rparen){
        rparen[0] = '\0';
        return parens(lparen+1, len);
    } else {
        return NULL;
    }
}

char* parse2(char* instr){
    char *saveptr_s, *saveptr_p;
    char *tok_s, *tok_p;
    tok_s = strtok_r(instr, ";", &saveptr_s);
    while(tok_s != NULL){
        tok_p = strtok_r(tok_s, "&", &saveptr_p);
        while(tok_p != NULL){
            printf("%s ", tok_p);
            tok_p = strtok_r(NULL,";",&saveptr_p);
            if (tok_p)
                printf("in paralell with ");
        }
    tok_s = strtok_r(NULL,";",&saveptr_s);
    if(tok_s)
        printf("then ");
    }
    printf("\n");
}
/*
int parse(char* init){
    char *cmd = malloc(strlen(init)+1);
    char *pret;
    assert(cmd != NULL);
    strcpy(cmd, init);
    pret = parens(cmd);
    printf("cmd %s\n", cmd);
    printf("pret %s\n", pret);
    //strcut(cmd, pret);
    printf("cmd after cut%s\n", cmd);

   // while((pret = parens(cmd)) != cmd);
  */  




int main(int argc, char **argv){
    parse2(argv[1]);
    return 0;
}
