#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
    int pipe1[2], pipe2[2], pipe3[2];

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1 || pipe(pipe3) == -1) {
        perror("pipe failed");
        exit(1);
    }

    pid_t pid1 = fork();
    if (pid1 < 0) {
        perror("fork error");
        exit(1);
    }
    
    if (pid1 == 0){
        close(pipe1[1]);
        close(pipe3[1]);
        close(pipe2[0]);
        close(pipe3[0]);

        int num;
        while (read(pipe1[0], &num, sizeof(num)) > 0) {
            num = num * num; 
            write(pipe2[1], &num, sizeof(num));
        }
        close(pipe1[0]);
        close(pipe2[1]);
        exit(0);
    }

    pid_t pid2 = fork();
    if (pid2 < 0) {
        perror("fork error");
        exit(1);
    }

    if (pid2 == 0) { 
        close(pipe1[0]); 
        close(pipe1[1]); 
        close(pipe2[1]); 
        close(pipe3[0]); 

        int num;
        while (read(pipe2[0], &num, sizeof(num)) > 0) {
            num += 1;
            write(pipe3[1], &num, sizeof(num));
        }
        close(pipe2[0]);
        close(pipe3[1]);
        exit(0);
    }

    close(pipe1[0]); 
    close(pipe2[0]); 
    close(pipe2[1]); 
    close(pipe3[1]); 

    int num;
    printf("enter an integer: ");
    while (scanf("%d", &num) != EOF) {
        write(pipe1[1], &num, sizeof(num));
        read(pipe3[0], &num, sizeof(num));  
        printf("result: %d\n", num);
        printf("enter an integer: ");
    }

    close(pipe1[1]);
    close(pipe3[0]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;

}
