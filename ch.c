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
int processif(char *args[], size_t *i);

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

    for(size_t k = 0; k < 64; k++)
    {
      if(args[k] == NULL){
        break;
      }
      printf("%ld Printing the input: %s\n", k, args[k]);
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
  size_t i = 0;

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
      exit(1);
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
  // for(size_t k = 0; k < 64; k++)
  // {
  //   if(args[k] == NULL){
  //     break;
  //   }
  //   printf("%ld Processing this command: %s\n", k, args[k]);
  // }

  while (*i < 64){
    if (args[*i] == NULL){
      tempcommand[j] = args[*i];      
      status = execute(tempcommand);
      printf("Exit status : %d\n", status);
      return status;
    } else if (strcmp(args[*i], "if") == 0){
      (*i)++;
      return processif(args,i);
    } else if (strcmp(args[*i], "&&") == 0){
      
      tempcommand[j] = NULL; // set as null to process the command
      (*i)++;
      status = execute(tempcommand);
      if(status == 0){
        //AND case, continue on success
        return processcommand(args, i);
      }
    } else if (strcmp(args[*i], "||") == 0){
      tempcommand[j] = NULL; // set as null to process the command
      (*i)++;
      status = execute(tempcommand);
      if(status != 0){
        //OR case, continue on fail
        return processcommand(args, i);
      }
      break;
    } else {
      tempcommand[j] = args[*i];
      (*i)++;
      j++;
    }
  }
  return status;
}

int processif(char *args[], size_t *i){
  int status = 1;
  int condstatement = 0;
  size_t j = 0;
  char *condcommand[64];
  char *docommand[64];

  // size_t k = *i;
  // for(;k < 64; k++)
  // {
  //   if(args[k] == NULL){
  //     break;
  //   }
  //   printf("%ld Printing args: %s\n", k, args[k]);
  // }

  while (*i < 64){
    if (strcmp(args[*i], "if") == 0){
      (*i)++;
      condstatement = processif(args, i); //conditionnal block is a if
      break;
    } else if (strcmp(args[*i], ";") == 0) {
      (*i)++;
      condcommand[j] = NULL;
      size_t tempindex = 0;
      condstatement = processcommand(condcommand, &tempindex);
      break;
    } else {
      condcommand[j] = args[*i];
      (*i)++;
      j++;
    }
  }
  j = 0;

  //skip ahead to "do" in case some instructions were skipped
  while(strcmp(args[*i], "do") != 0){
    (*i)++;
  }

  //"do" statement
  if(condstatement == 0 && strcmp(args[*i], "do") == 0){
    (*i)++;
    while (*i < 64){
      if (strcmp(args[*i], "if") == 0){
        (*i)++;
        return processif(args,i);
      } else if (strcmp(args[*i], ";") == 0) {
        (*i)++;
        docommand[j] = NULL;
        size_t tempindex = 0;
        status = processcommand(docommand, &tempindex);
        break;
      } else {
        docommand[j] = args[*i];
        (*i)++;
        j++;
      }
    }
    if (strcmp(args[*i], "done") != 0){
      //syntax error
      (*i)++;
      return 1;
    }
    (*i)++;
    return status;
  }
}

