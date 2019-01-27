/* ch.c.
auteur:
date:
problèmes connus:

  */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h> 
#include <sys/wait.h> 
#include <string.h> 

void parseargs(char *input, char *args[]);

int main (void)
{
  fprintf (stdout, "%% ");
  /* ¡REMPLIR-ICI! : Lire les commandes de l'utilisateur et les exécuter. */
  int bytes_read;
  size_t size;
  char *input = NULL;
  pid_t pid;
  
  char *command = NULL;

  //setvbuf(stdout, NULL, _IONBF, 0);
  while(bytes_read = getline(&input, &size, stdin)){
    //parse the string
    char *args[64];

    parseargs(input, args);
    command = args[0]; //args[0] contains the command and filename
    pid = fork();
    if(pid < 0){
      return 1;
    }
    else if(pid == 0) {
      //handling child process
      execvp(command, args);
      exit(0);
    }
    else{
      //parent process
      pid_t pid_return_val;
      int status;
      pid_return_val = wait(&status); //do something with return value

      fprintf (stdout, "%% ");

    }
  }
  free(input);
  fprintf (stdout, "Bye!\n");
  exit (0);
}

void parseargs(char *input, char *args[]){
  char *token;
  char delim[] = " \n";
  int i = 0;

  token = strtok(input, delim);

  while(token != NULL){
    args[i] = token;
    token = strtok(NULL, delim);
    i++;
  }
  args[i] = NULL; //set the last args as NULL for execvp

  return;
}
