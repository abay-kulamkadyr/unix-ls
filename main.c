#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <grp.h>
#include <pwd.h>
#include <time.h> 
#include <errno.h>
static char* currentDirectory=".";
static char permissions[10];
static char timeOfModif[20];
static char bufForSymbolicLink[PATH_MAX];
//static char timeOfModif[18];
struct options
{
    bool option_i;
    bool option_l;
    bool option_R;
};
static void initOptions(struct options* getOption )
{
    getOption->option_i=false;
    getOption->option_l=false;
    getOption->option_R=false;
}
struct options parseOptions (int argc, char* argv[])
{
    //creating a struct of options to infer which option has been requested
    struct options passedOptions;
    //initiallizing all options to false initially 
    initOptions(&passedOptions);
    int options;
    //using getopt function to parse all possible permutations of options
    while ((options= getopt(argc,argv, "ilR"))!=-1)
    {
        switch (options)
        {
            case 'i':
            passedOptions.option_i=true;
                break;
            case 'l':
            passedOptions.option_l=true;
            break;
            case 'R':
            passedOptions.option_R=true; 
            break;
            case '?': 
                printf("Sorry the specified optins are not currently supported\nTerminating the program...\n");
                exit(1);
        }
        
    }
    return passedOptions;
}
void permission_bits(bool isDir,bool isLink, mode_t mode)
{
    if (isDir){permissions[0]='d';}       else if(isLink){permissions[0]='l';}
                                          else{permissions[0]='-';}
    if(mode&S_IRUSR){permissions[1]='r';} else{permissions[1]='-';}
    if(mode&S_IWUSR){permissions[2]='w';} else{permissions[2]='-';}
    if(mode&S_IXUSR){permissions[3]='x';} else{permissions[3]='-';}
    if(mode&S_IRGRP){permissions[4]='r';} else{permissions[4]='-';}
    if(mode&S_IWGRP){permissions[5]='w';} else{permissions[5]='-';}
    if(mode&S_IXGRP){permissions[6]='x';} else{permissions[6]='-';}
    if(mode&S_IROTH){permissions[7]='r';} else{permissions[7]='-';}
    if(mode&S_IWOTH){permissions[8]='w';} else{permissions[8]='-';}
    if(mode&S_IXOTH){permissions[9]='x';} else{permissions[9]='-';}
}
void getAndPrintGroup(gid_t grpNum) {
  struct group *grp;

  grp = getgrgid(grpNum); 
  
  if (grp) {
    printf("%s ", grp->gr_name);
  } else {
    printf("No group name for %u found\n", grpNum);
  }
}
void getAndPrintUserName(uid_t uid) {

  struct passwd *pw = NULL;
  pw = getpwuid(uid);

  if (pw) {
    printf("%s ",pw->pw_name);
  } else {
    perror("Hmm not found???");
    printf("No name found for %u\n", uid);
  }
}
void modificationDate(time_t time_mod)
{   
    struct tm* timer;
    timer= localtime(&time_mod);
    const char* format ="%b %d %Y %H:%M";
    strftime(timeOfModif,sizeof(timeOfModif), format, timer);
    printf("%s ",timeOfModif);
}

bool isDirectory(struct stat* path_st)
{
    return S_ISDIR(path_st->st_mode); 
}
bool isLink(struct stat* path_st)
{
    return S_ISLNK(path_st->st_mode);
}
struct stat get_info(char* dir,char *filename)
{
    struct stat ret;
    char fullpath[PATH_MAX];
    sprintf(fullpath, "%s/%s", dir, filename);
    
    if(lstat(fullpath,&ret)==-1)
        exit(1);
    if(isLink(&ret))
    {
        memset(bufForSymbolicLink,0, sizeof(bufForSymbolicLink));
        readlink(fullpath, bufForSymbolicLink,sizeof(bufForSymbolicLink));
    }
    return ret;
}
void option_l(struct stat* statBuf)
{
    permission_bits(isDirectory(statBuf),isLink(statBuf),statBuf->st_mode);
    printf("%s ", permissions);
    printf("%ld ", statBuf->st_nlink);
    getAndPrintUserName(statBuf->st_uid);
    getAndPrintGroup(statBuf->st_gid);
    printf("%ld ",statBuf->st_size);
    modificationDate(statBuf->st_mtime);
}

void processDir(char * filename,struct options *option)
{
        DIR* dirfd = opendir(filename);
        if (dirfd==NULL)
        {
            if(errno==ENOENT)
            {
                char* error = malloc(BUFSIZ * sizeof(char));
                snprintf(error, BUFSIZ, "ls: cannot access %s: No such file or directory\n", filename);
                perror(error);
            }
            else
            {
                printf("%s\n",filename);
            }
            return;
        }
        struct dirent* entity= readdir(dirfd);
        struct stat statBuf;
        while(entity!=NULL)
        {
            if (entity->d_name[0]=='.')
            {
                entity=readdir(dirfd);
                continue;
            }
            //struct stat statbuf=get_info(entity->d_name);

            if((option->option_i))
            {
                printf("%ld ", entity->d_ino);
            }
            if(option->option_l){
                statBuf= get_info(filename, entity->d_name);
                option_l(&statBuf);
                printf("\t%-20s ",entity->d_name);
                if(isLink(&statBuf))
                {
                    printf("->%s", bufForSymbolicLink);
                }
                printf("\n");
            }
            else
            {
                printf("%s\t",entity->d_name);
            }
            entity=readdir(dirfd);    
        }
        printf("\n");
        closedir(dirfd);
}
void option_R (char * dir, struct options* option)
{
    DIR* dirfd= opendir(dir);
    if(dirfd==NULL)
    {
        if(errno==ENOENT)
            {
                perror("Directory does not exists");
            }
            else
            {
                perror("Unable to read directory");
            }
            exit(EXIT_FAILURE);
    }
    processDir(dir, option); 
    struct dirent* entity= readdir(dirfd);
    struct stat statBuf;
    
    
    while(entity!=NULL)
    {
        if (entity->d_name[0]=='.')
        {
            entity=readdir(dirfd);
            continue;
        }
        statBuf= get_info(dir, entity->d_name);
        if(isDirectory(&statBuf))
        {
             printf("\n%s/%s:\n",dir,entity->d_name);

        }
        if(isDirectory(&statBuf))
        {
            char buf[PATH_MAX];
            sprintf(buf,"%s/%s",dir,entity->d_name);
            option_R(buf,option);
        }
        entity= readdir(dirfd);
    }
    closedir(dirfd);
}

void processArguments(int argc, char* argv[], struct options* options)
{
    
    // optind is an index to the argv that it has been processing, 
    // therefore if argc==optind it means that no directory has been provided
    bool directoryProvided= argc-optind;
    if(!options->option_R){
        if (!directoryProvided)
        {
            processDir(currentDirectory, options);
        }
        else{

            // checking if provided multiple directories
            bool isMultipleDirectories= argc-optind>=2;
            // passing each given path to be processed
            for (int i=optind;i<argc;i++){
                if (isMultipleDirectories)
                {
                    printf("%s:\n",argv[i]);
                }
                processDir(argv[i], options);
            }
        }
        
    }
    else
    {
        if(!directoryProvided)
        {
            option_R(currentDirectory, options);
        }
        else
        {
            for(int i=optind;i<argc;i++)
            {
                printf("%s:\n",argv[i]);
                option_R(argv[i], options);
            }
            
        }
    }

}
int main(int argc, char* argv[])
{
    struct options opt=parseOptions(argc, argv);
    processArguments(argc, argv, &opt);
    return 0;
}