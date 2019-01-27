/* ch.c.
auteur:
date:
problèmes connus:

  */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h> 
#include<sys/wait.h> 
#include<string.h> 


int main (void)
{
  fprintf (stdout, "%% ");
  /* ¡REMPLIR-ICI! : Lire les commandes de l'utilisateur et les exécuter. */
  int bytes_read;
  size_t size;
  char *input = NULL;
  pid_t pid;

  //setvbuf(stdout, NULL, _IONBF, 0);
  while(bytes_read = getline(&input, &size, stdin)){
    fprintf (stdout, "%% ");
    //parse the string

    pid = fork();
    if(pid < 0){
      return 1;
    }
    else if(pid == 0) {
      //handling child process
      execlp("ls","ls", NULL); 
      exit(0);
    }
    else{
      //parent process
      pid_t pid_return_val;
      int status;
      pid_return_val = wait(&status); //do something with return val
    }
  }
  free(input);
  fprintf (stdout, "Bye!\n");
  exit (0);
}
