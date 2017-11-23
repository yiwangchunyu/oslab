#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<pwd.h>
#include<string.h>
#include<ctype.h>
#include<fcntl.h>
void error();
void print_prompt()
{
    char buf[10]="mysh> ";
    if(write(1,buf,10)<0) error();
}
void myexit()
{
    exit(0);
}
void mypwd()
{
    char pwd[100]= {0};
    getcwd(pwd,sizeof(pwd)-1); //pwd保存的是绝对路径
    strcat(pwd,"\n");
    if(write(1,pwd,100)<0) error();
}
void mycd(char *path)
{
    if(path==NULL)
    {
        path=getenv("HOME");
    }
    if(chdir(path)<0)
        error();
}
void mywait()
{
    wait(NULL);
}
void error()
{
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}
void parse_command(char *line,char **myargv, int *bg_tag, char *rd_tag, char *rd_file)
{
    char *start = line;
    while(isspace(*start)) start++;
    myargv[0] = start;
    int i=1;
    while(*start)
    {
        if(*start=='&')  //implement backgroung jobs
        {
            *bg_tag=1;
            *start='\0';
            if(*(--start)!='\0') i++;
            break;
        }
        if((*start=='>')||(*start=='<'))  //implement redirection
        {
            *rd_tag=*start;
            char *name_start;
            char *rd = start;
            *start='\0';
            rd++;
            while(isspace(*rd))
                rd++;
            name_start=rd;
            while(!isspace(*rd))
                rd++;
            *rd = '\0';
            strcpy(rd_file,name_start);
            if(*(--start)!='\0') i++;
            break;
        }
        if(isspace(*start))
        {
            while(isspace(*start))
            {
                *start='\0';
                start++;
            }
            myargv[i++]=start;
        }
        else start++;
    }
    myargv[i-1]=NULL;
}
void command_exec(char **myargv, int bg_tag, char *rd_tag, char* rd_file)
{
    int fd,fd0,fd1;
    if(*rd_tag)
    {
        fd = open(rd_file,O_RDWR|O_CREAT|O_TRUNC,0777);
        if(fd<0)
            error();
        if(*rd_tag=='>')
        {
            fd1=dup(1);
            dup2(fd,1);
        }
        else
        {
            fd0=dup(0);
            dup2(fd,0);
        }
    }
    if(strcmp(myargv[0],"exit")==0) myexit();
    else if(strcmp(myargv[0],"cd")==0) mycd(myargv[1]);
    else if(strcmp(myargv[0],"pwd")==0) mypwd();
    else if(strcmp(myargv[0],"wait")==0) mywait();
    else
    {
        int id = fork();
        if(id<0)
            error();
        if(id!=0)
		if(!bg_tag) wait(NULL);
		else;
          else if(execvp(myargv[0], myargv)<0)
                error();
           
    }
    if(*rd_tag)
    {
        dup2(fd1,1);
        dup2(fd0,0);
        close(fd);
    }
}
int main(int argc, char * argv[])
{
    char line[512];
    char *myargv[10];
    int *bg_tag=(int *)malloc(sizeof(int));
    int *pipe_tag=(int *)malloc(sizeof(int));
    char *rd_tag=(char *)malloc(sizeof(char));

    *bg_tag=0;
    *rd_tag=0;
    *pipe_tag=0;
    char rd_file[20];
    if(argc==2)
    {
        FILE *fp = fopen(argv[1],"r");
        if(fp==NULL)
            error();
        else
        {
            while(fgets(line, 512, fp))
            {
                *bg_tag=0;
                *rd_tag=0;
                *pipe_tag=0;
                int len = strlen(line);
                if(write(1,line,len)<0) error();
                parse_command(line,myargv,bg_tag, rd_tag,rd_file);
                if(myargv[0]==NULL) continue;
                command_exec(myargv,*bg_tag,rd_tag, rd_file);
                memset(line,0,512);
                memset(myargv,0,10);
            }
        }
    }
    else if(argc==1)
    {
        while(1)
        {
            *bg_tag=0;
            *rd_tag=0;
            *pipe_tag=0;
            print_prompt();
            memset(line,0,512);
            memset(myargv,0,10);
            ssize_t s = read(0, line, 1024);
            if(s<0) error();
            parse_command(line,myargv,bg_tag, rd_tag, rd_file);
            if(myargv[0]==NULL) continue;
            command_exec(myargv,*bg_tag,rd_tag, rd_file);
        }
    }
    else error();
    return 0;
}


