#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LINE 80 // 명령어 최대 길이

int main() {
    char input[MAX_LINE]; // 사용자 입력을 저장할 버퍼
    char *args[MAX_LINE / 2 + 1]; // 명령어와 인자를 저장할 배열
    int should_run = 1; // 프로그램 실행 여부

    while (should_run){
        printf("minishell> ");
        fflush(stdout); // 출력 버퍼를 비워서 프롬프트가 바로 보이도록 함

        // 사용자 입력 읽기
        if (fgets(input, sizeof(input), stdin)==NULL) continue; // 입력이 없으면 다시 프롬프트로

        input[strcspn(input, "\n")] = 0; // 입력에서 개행 문자 제거

        if (strcmp(input, "exit") == 0) { // "exit" 명령어 입력 시 종료
            should_run = 0;
            continue;
        }

        int i = 0;
        char *token = strtok(input, " "); // 입력을 공백 기준으로 분리
        while (token != NULL){
            args[i++] = token; // 명령어와 인자를 배열에 저장
            token = strtok(NULL, " ");
        }
        args[i] = NULL; // 명령어와 인자 배열의 끝을 나타냄

        if(args[0]==NULL) continue; // 명령어가 없으면 다시 프롬프트로

        pid_t pid = fork(); // 자식 프로세스 생성 => 현재 실행 중인 프로그램이 똑같은 복사본(자식프로세스)를 하나 더 만듦

        if (pid < 0){ //복사본 만들기에 실패했다면
            fprintf(stderr, "Fork Failed\n"); //시스템 호출 실패 시 에러 메시지 출력
            return 1; //문제가 발생했음을 나타내는 값 반환
        }
        else if (pid==0){ //pid가 0이면 자식 프로세스를 의미
            if (execvp(args[0], args)<0){/*execvp 함수는 명령어를 실행하는 시스템 호출로, 첫 번째 인자는 명령어, 두 번째 인자는 명령어와 인자를 포함하는 배열.
                execvp 함수가 성공적으로 실행되면 현재 프로세스의 메모리 공간이 새로운 프로그램으로 대체되고, execvp 함수는 반환하지 않는다.
                그러나 execvp 함수가 실패하면 -1을 반환하고, errno 변수를 설정하여 오류의 원인을 나타낸다.*/
                printf("명령어를 찾을 수 없거나 실행에 실패했습니다.\n");
                exit(1); // 자식 프로세스 종료, 좀비프로세스 방지 위해 exit(1)로 종료
            }
        }
        else{
            wait(NULL); // 부모 프로세스는 자식 프로세스가 끝날 때까지 기다림
            /*wait 함수가 자식 프로세스가 전하는 종료 신호를 받아주지 않는다면, 자식 프로세스는 지워지지 못하고
            메모리를 계속차지하는 좀비 프로세스가 됨*/
        }
    }
    return 0; // 프로그램 종료
}