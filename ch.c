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
int execute(char *args[]);
int processcommand(char *args[], size_t *i);

int main (void)
{
  fprintf (stdout, "%% ");
  /* ¡REMPLIR-ICI! : Lire les commandes de l'utilisateur et les exécuter. */
  int bytes_read;
  size_t size;
  char *input = NULL;
  
  //setvbuf(stdout, NULL, _IONBF, 0);
  while(bytes_read = getline(&input, &size, stdin)){
    char *args[64];

    parseargs(input, args); //split at spaces

    for(size_t i = 0; i < sizeof(args); i++)
    {
      if(args[i] == NULL){
        break;
      }
      printf("%ld Printing the input: %s\n", i, args[i]);
    }

    size_t i = 0;
    processcommand(args,&i);

    fprintf (stdout, "%% ");
  }
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

int execute(char *args[]){
    pid_t pid;
    pid_t pid_return_val;
    int status;

    pid = fork();
    if(pid < 0){
      return 1;
    }
    else if(pid == 0) {
      //handling child process
      execvp(args[0], args);
      //if execvp did not work
      printf("%s: command not found\n", args[0]);
      exit(0);
    }
    else{
      //parent process
      pid_return_val = wait(&status);
    }

    return status;//return the child process status
}

int processcommand(char *args[], size_t *i){
  int status = 0;
  size_t j = 0;
  char *tempcommand[64];

  while (*i < sizeof(args)){

    if (args[*i] == NULL){
      tempcommand[j] = args[*i];      
      status = execute(tempcommand);
      printf("Exit status : %d\n", status);
      return status;
    } else if (strcmp(args[*i], "&&") == 0){
      tempcommand[j] = NULL; // set as null to process the command
      (*i)++;
      status = execute(tempcommand);
      if(status == 0){
        //l'operation en cours a fonctionné donc on continue
        return processcommand(args, i);
      }
    } else if (strcmp(args[*i], "||") == 0){
      tempcommand[j] = NULL; // set as null to process the command
      (*i)++;
      status = execute(tempcommand);
      if(status != 0){
        //l'operation n'a pas fonctionné donc on continue
        return processcommand(args, i);
      }
    } else {
      tempcommand[j] = args[*i];
      (*i)++;
      j++;
    }
  }
  
  return status;

}