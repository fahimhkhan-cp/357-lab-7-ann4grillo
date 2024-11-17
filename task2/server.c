#define _GNU_SOURCE
#include "net.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define PORT 1030

void sigchild_handler(int signo){

   int status;
   pid_t pid;

   while((pid = waitpid(-1, &status, WNOHANG)) > 0){
      printf("child process %d terminated\n", pid);
   }
}

void handle_request(int nfd)
{
   FILE *network = fdopen(nfd, "r");
   char *line = NULL;
   size_t size;
   ssize_t num;

   if (network == NULL)
   {
      perror("fdopen");
      close(nfd);
      return;
   }

   while ((num = getline(&line, &size, network)) >= 0)
   {
      write(nfd, line, num);
      printf("writing back to cilent\n");
   }

   free(line);
   fclose(network);
}

void run_service(int fd)
{
   while (1)
   {
      int nfd = accept_connection(fd);
      if (nfd != -1)
      {
         printf("Connection established\n");

         pid_t pid = fork();
         if (pid ==0){
            close(fd);
            handle_request(nfd);
            exit(0);

         }else if (pid > 0){

            close(nfd);  
         }else{
            perror("fork");
            close(nfd);
         }
         
         printf("Connection closed\n");
      }
   }
}

int main(void)
{
   signal(SIGCHLD, sigchild_handler);

   int fd = create_service(PORT);

   if (fd == -1)
   {
      perror(0);
      exit(1);
   }

   printf("listening on port: %d\n", PORT);
   run_service(fd);
   close(fd);

   return 0;
}
