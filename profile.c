#include "profile.h"
#include "mylimits.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
/* Initialize Directory
 * Sets the current working directory to the directory contained in
 * "./.profile" if it exists.
 * No error message if "./.profile" doesnt exist
 * Path starts with "/"  
 * No spaces allowed within directory path
 * Maximum path in file is MAXLINE
 */
void init_dir()
{
	FILE *fp = fopen("./.profile","r");
	if(fp != NULL)
	{
		char *buf = malloc(MAXLINE*sizeof(char));
		fgets(buf,MAXLINE,fp);
		fclose(fp);
		while(*buf != '/' /*&& *buf != '~'*/ && *buf != EOF)
		{
			buf++;
		}
		char *pstart = buf;
		size_t size = 0;
		while(*buf != ' ' && *buf != '\n' && *buf != EOF)	
		{
			buf++;
			size++;
		}
		size++;
		char *path = malloc(size*sizeof(char));
		path[size] = '\0';
		size--;
		strncpy(path,pstart,size);

		if(chdir(path) == -1)
		{
			perror("Error while interpreting .profile");
		}
	}
	/* DEBUG
	char cwd[1024];
   	if (getcwd(cwd, sizeof(cwd)) != NULL)
   	    fprintf(stdout, "Current working dir: %s\n", cwd);
   	else
   	    perror("getcwd() error");
	*/
}

/* resolve_path - Searches directories on the PATH for a given command
 * if the command does not include a '/'. The area pointed to by
 * command must be at least MAXLINE long.
 */
int resolve_path(char *cmd){
    int i;
    DIR *curdir;
    char fpath[PATH_MAX];
    const char slashstr[] = "/";
    struct dirent *dfp;
    struct stat fst;
    /* slash in path */
    if(strchr(cmd,'/') != NULL)
        return 0;
    for(i=0;i<n_path_dirs;i++){
        if((curdir = opendir(path_dirs[i])) == NULL){
            perror("Failed to open directory on PATH");
            return -2;
        }
        while((dfp = readdir(curdir)) != NULL){
           /* see if dfp points to a regular file*/
            strcpy(fpath, path_dirs[i]);
            strcat(fpath,slashstr);
            strncat(fpath, dfp->d_name, PATH_MAX-strlen(path_dirs[i])-2);
            if(lstat(fpath, &fst) == -1){
                perror("lstat failed during path walk");
            }
            if((fst.st_mode & S_IFMT) == S_IFREG){
                /* this is a reg file, test name */
                if(strcmp(cmd, dfp->d_name)==0){
                    strcpy(cmd, fpath);
                    return i;
                }
            }
        }
    }
    return -1;
}



