#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
int Built_In_Command();
void Child_Process();
/* 输入的命令行 */
char cmd[256];
/* 命令行拆解成的各部分，以空指针结尾 */
char *args[128];
char *div_args[20][128];
int built_in_return=0;
int cmd_num=0;
int main()
{
    while (1)
 {
        /* 提示符 */
        printf("# ");
        fflush(stdin);//清空输入缓冲区
        fgets(cmd, 256, stdin);
        /* 清理结尾的换行符 */
        int i,j,k;
        for (i = 0; cmd[i] != '\n'; i++)   //以换行符为基准
            ;
        cmd[i] = '\0';
        /* 拆解命令行 */
        args[0] = cmd;
        for (i = 0; *args[i]; i++)           //出现空格后i+1
            for (args[i+1] = args[i] + 1; *args[i+1]; args[i+1]++)
                if (*args[i+1] == ' ')       //按照空格划分命令
                {
                    *args[i+1] = '\0';
                    args[i+1]++;
                    break;
                }
        args[i] = NULL;
        for(i=0,j=0,k=0;args[i];i++)
        {
     //       printf("args[%d]=%s\n",i,args[i]);
            if(*args[i]=='|')
            {
                div_args[j][k] = NULL;
                j++;
                k=0;
            }
            else
            {
                div_args[j][k] = args[i];
       //         printf("args[%d][%d]=%s\n",j,k,div_args[j][k]);
                k++;
            }
        }
        div_args[j][k]=NULL;
        cmd_num = j+1;          //当只有一条命令或者没有命令时cmd_num均为1
        /* 没有输入命令 */
        if (!args[0])
            continue;
        //内建命令
       // printf("cmd_num=%d\n",cmd_num);
        built_in_return = Built_In_Command();
       // printf("built_in=%d\n",built_in_return);
    if(built_in_return == 0)
      return 0;
    else if(built_in_return == 1)
      continue;
            Child_Process();
    }
}
int Built_In_Command()    //内建命令
{
 int i;
    if (strcmp(args[0], "cd") == 0)
    {
        if (args[1])
            chdir(args[1]);
        return 1;
    }
    if (strcmp(args[0], "pwd") == 0)
    {
        char wd[4096];
        puts(getcwd(wd, 4096));
        return 1;
    }
    if (strcmp(args[0], "exit") == 0)
        return 0;
 if (strcmp(args[0],"export") == 0)
 {
  if (args[1])
  {
      for(i=0;args[1][i]!='='&&i<127;i++);
      args[1][i] = '\0';
      args[2] = & args[1][i+1];
      if(i==127)
      printf("export failed\n");
      else if(setenv(args[1],args[2],1) == 0)
      printf("export succeed\n");
      else printf("export failed\n");
  }
  return 1;
 }
 return 2;
}
void Child_Process()
{
 int i;
 int fd[128][2];
 for(i=0;i<=cmd_num-1;i++)
 {
//  printf("i=%d\n",i);
    if(i<cmd_num-1)
     pipe(fd[i]);
     pid_t pid = fork();
    // printf("%d\n",pid);
        if(pid<0)/*如果(进程标记<0)*/
        {
            printf("fork error");
        }
        else if(pid==0)/*否则如果(进程标记==0)*/
        {

      //      printf("pid[%d]\n",i);
            if(i != cmd_num-1)
            {
                 dup2(fd[i][1],STDOUT_FILENO);
                 close(fd[i][0]);
                 close(fd[i][1]);
  //               printf("pid1[%d]\n",i);
            }
            if(i!= 0)
            {
                 dup2(fd[i-1][0],STDIN_FILENO);
                 close(fd[i-1][0]);
                 close(fd[i-1][1]);
    //             printf("pid2[%d]\n",i);
            }
        //    printf("div_args=%s\n",div_args[i][0]);
            execvp(div_args[i][0],div_args[i]);
        }
       // dup2(fd[0],STDIN_FILENO);
        if(i != 0)
            close(fd[i-1][0]);
        if(i != cmd_num-1)
            close(fd[i][1]);
        wait(NULL);
 }
}

