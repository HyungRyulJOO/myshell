#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <pwd.h>

#define LEN_HOSTNAME	30
#define MAX_LEN_LINE    10

int main(void)      //gcc -o 파일이름 파일이름.c 형식으로 컴파일 ./파일 이름 실행

{
    char command[MAX_LEN_LINE];    
 
    char *array[10] = {command,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
    int ret, status;
    pid_t pid, cpid;     //프로세스 번호를 저장하는 타입

    char hostname[LEN_HOSTNAME + 1];

    while (true) {
        char *s, *sss;
        int len;

        memset(hostname, 0x00, sizeof(hostname));
        gethostname(hostname, LEN_HOSTNAME); //
        printf("%s@%s (toy_shell) $ ", getpwuid(getuid())->pw_name, hostname);  //username과 hostname 출력

        s = fgets(command, MAX_LEN_LINE, stdin);  //stream에서 문자열을 받는다 
        
        if ( strcmp(s, "exit\n") == 0 ) //Enter 값도 입력으로 받는 것으로 문자열 비교
        {
            printf("%s", s);
            exit(1);                    //스트림에 exit입력시 종료
        }
        
        else {
            
            if (s == NULL) {     //아무 것도 없으면
                fprintf(stderr, "fgets failed\n");   // 출력
                exit(1);       //종료
            }
            
            len = strlen(command);     //문자열의 길이 구하기
            printf("%d\n", len);      //len출력
            if (command[len - 1] == '\n') {
                command[len - 1] = '\0';
            }

            sss = strtok(s, ";"); //스트림으로 받은 문자열 s를 ;를 기준으로 자름
            int count = 0;
            int n =0;
            while(sss != NULL)    //자른 sss를 array에 보관
            {
                array[count] = sss;
                count ++;
                n++;
                sss = strtok(NULL, ";");
            }

            for (int countN = 0 ; countN < n ; countN++ )   //array에 담긴 갯수만큼 fork반복
            {
                pid = fork();     //복사본의 프로세스 생성
        
                if (pid < 0) {
                    fprintf(stderr, "fork failed\n");
                    exit(1);
                }

                if (pid != 0) {  /* parent */
                    cpid = waitpid(pid, &status, 0);  //자식 프로세스의 종료를 기다린다
                    
                    if (cpid != pid) {
                        fprintf(stderr, "waitpid failed\n");
                    }

                    printf("Child process terminated\n"); ///cpid == pid 이면

                    if (WIFEXITED(status)) {   //자식 프로세스의 종료 상태를 확인 
                            printf("Exit status is %d\n\n", WEXITSTATUS(status));
                    }         ///성공 0 실패 1
            }

                else {  /* child pid == 0 */
                    ret = execve(array[countN], &array[countN], NULL);  //여러개의 문자열이 담긴 array에 해당하는 주소의 파일 실행

                    if (ret < 0) {
                        fprintf(stderr, "execve failed\n");
                        return 1;   ////실행 실패시 1을 반환
                    }
                } 
            }           
        }
    }   
    return 0;
}       
