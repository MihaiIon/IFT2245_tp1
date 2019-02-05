/* ch.c.
auteurs: Andre Langlais & Mihai Ionescu
date: 8 fevrier 2019
problèmes connus:

  */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h> 
#include <sys/wait.h> 
#include <string.h>
#include <fcntl.h> 
/* isbg = boolean value for background processing*/
/* isrd = boolean value for redirecting the output of an instruction
*         to a specified file*/
void parse_args(char *input, char *args[]);
int process_input(char *args[], int size);
int execute(char *args[], int size, int isbg, int hasrd, char *fname);
int process_command(char *args[], int size, size_t *i, int isbg);
int process_if(char *args[], int size, size_t *i, int isbg);
int space_count(char *text);

int main (void)
{
  fprintf (stdout, "%% ");
  /* ¡REMPLIR-ICI! : Lire les commandes de l'utilisateur et les exécuter. */
  size_t size;
  char *input = NULL;
  int bytes_read;
  
  setvbuf(stdout, NULL, _IONBF, 0);
  while(bytes_read = getline(&input, &size, stdin)){
    if(bytes_read == -1){
      break;
    }
    int words = space_count(input);
    char *args[words];

    parse_args(input, args); /*split in an array of strings*/
    process_input(args, words); /*process the command*/

    fprintf (stdout, "%% ");
  }
  fprintf (stdout, "Bye!\n");
  exit (0);
}
/*
* Function: parse_args
* ----------------------
* parses the input into an array of string
* the input is splitted at spaces
*/
void parse_args(char *input, char *args[]){
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
/*
* Function: execute
* ----------------------
* executes an instruction using exevp
* returns: the status as an int
*/
int execute(char *args[], int size, int isbg, int hasrd, char *fname){
    pid_t pid;
    pid_t pid_return_val;
    int status;
    pid = fork();
    if(pid < 0){
      return 1;
    }
    else if(pid == 0) {
      /*child process*/
      if(isbg){
        setpgid(pid, 0);
        exit(process_input(args, size));
      }
      if(hasrd){
        int fd = open(fname, O_WRONLY | O_CREAT, 0600);
        if(fd < 0) {
          /*error handling*/
          printf("file error\n");
          exit(1);
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
      }
      execvp(args[0], args);
      /*if execvp did not work*/
      printf("%s: command not found\n", args[0]);
      exit(1);
    }
    else{
      /*parent process*/
      if(isbg){
        setpgid(pid, 0);
        pid_return_val = waitpid(0, &status, WNOHANG);
      } else{
        pid_return_val = waitpid(pid, &status, WUNTRACED); 
      }
    }
    return status;
}
/*
* Function: process_input
* ----------------------
* entry points of the processing engine
* returns: the status of an instruction
*/
int process_input(char *args[], int size){
    
    int isbg = 0; /*indicate if an instruction 
                   will be processed in background*/
    for(size_t k = 0; k < size; k++)
    {
      if(args[k] == NULL){
        break;
      }
      if(strcmp(args[k], "&") == 0){
        isbg = 1;
        break;
      }
    }
    size_t i = 0; 
    return process_command(args, size, &i, isbg);
}
/*
* Function: process_command
* ----------------------
* recursively parses the instruction word by word applying 
* a different treatement on key elements
* returns: return status of an instruction
*/
int process_command(char *args[], int size, size_t *i, int isbg){
  int status = 0;
  size_t j = 0;
  char *tempcommand[size];

  while (*i < size){
    if (args[*i] == NULL){
      /*End of the user input*/
      tempcommand[j] = args[*i];      
      status = execute(tempcommand, size, isbg, 0, NULL);
      return status;

    } else if (strcmp(args[*i], "if") == 0 && !isbg){
      (*i)++;
      return process_if(args, size, i, isbg);

    } else if (strcmp(args[*i], "&&" ) == 0 && !isbg){
      
      tempcommand[j] = NULL; /*set as NULL for execvp*/
      (*i)++;
      status = execute(tempcommand, size, isbg, 0, NULL);
      if(status == 0){
        /*AND case, continue on success*/
        return process_command(args, size, i, isbg);
      }
    } else if (strcmp(args[*i], "||") == 0 && !isbg){
      tempcommand[j] = NULL; /*set as NULL for execvp*/
      (*i)++;
      status = execute(tempcommand, size, isbg, 0, NULL);
      if(status != 0){
        /*OR case, continue on fail*/
        return process_command(args, size, i, isbg);
      }
      break;
    } else if (strcmp(args[*i], "&") == 0){ 
      tempcommand[j] = NULL; /*set as NULL for execvp*/
      (*i)++;
      status = execute(tempcommand, size, isbg, 0, NULL);
      break;
    } else if (strcmp(args[*i], ">") == 0 && !isbg){ 
      tempcommand[j] = NULL;
      (*i)++;
      /*next string must be the file name*/
      status = execute(tempcommand, size, isbg, 1, args[(*i)++]);
      return process_command(args, size, i, isbg);
    } else {
      /*standard case, copy the string*/
      tempcommand[j] = args[*i];
      (*i)++;
      j++;
    }
  }
  return status;
}
/*
* Function: process_if
* ----------------------
* parses an if-statement, 
* calls itself if it find another if-statement
* returns: return status of the the instruction in the body
*/
int process_if(char *args[], int size, size_t *i, int isbg){
  int status = 1;
  int condstatement = 0; /*conditionnal statement*/
  size_t j = 0;
  char *condcommand[size]; /*command in the condtionnal statement*/
  char *docommand[size]; /*command in the body*/

  while (*i < size){
    if (strcmp(args[*i], "if") == 0){
      (*i)++;
      condstatement = process_if(args, size, i, isbg);
      break;
    } else if (strcmp(args[*i], "true") == 0) {
      condstatement = 0;
      (*i)++;
      break;
    } else if (strcmp(args[*i], "false") == 0) {
      condstatement = 1;
      (*i)++;
      break;
    } else if (strcmp(args[*i], ";") == 0) {
      (*i)++;
      condcommand[j] = NULL;
      size_t tempindex = 0;
      condstatement = process_command(condcommand, size, &tempindex, isbg);
      break;
    } else {
      condcommand[j] = args[*i];
      (*i)++;
      j++;
    }
  }
  j = 0;

  /*skip ahead to "do" in case some instructions were skipped*/
  /*is needed when using "&&" or "||"*/
  while(strcmp(args[*i], "do") != 0){
    (*i)++;
  }

  /*body statement*/
  if(condstatement == 0 && strcmp(args[*i], "do") == 0){
    (*i)++;
    while (*i < size){
      if (strcmp(args[*i], "if") == 0){
        (*i)++;
        return process_if(args, size, i, isbg);
      } else if (strcmp(args[*i], ";") == 0) {
        (*i)++;
        docommand[j] = NULL;
        size_t tempindex = 0;
        status = process_command(docommand, size, &tempindex, isbg);
        break;
      } else {
        docommand[j] = args[*i];
        (*i)++;
        j++;
      }
    }
    if (strcmp(args[*i], "done") != 0){
      /*the word done must be included at the end of the instruction*/
      (*i)++;
      return 1;
    }
    (*i)++;
    return status;
  }
}
/*
* Function: space_count
* ----------------------
* count the number of spaces, which should be at least 
* as big as the number of words
*/
int space_count(char *text){
  int count = 2;
  while(*text){
    if(*text == ' '){
      count++;
    }
    text++;
  }
  return count;
}

