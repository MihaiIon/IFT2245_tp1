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
#include <fcntl.h> 

void parseargs(char *input, char *args[]);
int processinput(char *args[], int size);
int execute(char *args[], int size, int isbackground, int hasredirect, char *fname);
int processcommand(char *args[], int size, size_t *i, int isbackground);
int processif(char *args[], int size, size_t *i, int isbackground);
int spacecount(char *text);

int main (void)
{
  fprintf (stdout, "%% ");
  /* ¡REMPLIR-ICI! : Lire les commandes de l'utilisateur et les exécuter. */
  int bytes_read;
  size_t size;
  char *input = NULL;
  
  while(bytes_read = getline(&input, &size, stdin)){
    int words = spacecount(input);
    char *args[words];

    parseargs(input, args); //split at spaces

    processinput(args, words);
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

int execute(char *args[], int size, int isbackground, int hasredirect, char *fname){
    pid_t pid;
    pid_t pid_return_val;
    int status;
    pid = fork();
    if(pid < 0){
      return 1;
    }
    else if(pid == 0) {
      //handling child process
      if(isbackground){
        setpgid(pid, 0);
        exit(processinput(args, size));
      }
      if(hasredirect){
        int fd = open(fname, O_WRONLY | O_CREAT, 0600);
        if(fd < 0) {
          //error handling
          printf("file error\n");
          exit(1);
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
      }
      execvp(args[0], args);
      //if execvp did not work
      printf("%s: command not found\n", args[0]);
      exit(1);
    }
    else{
      //parent process
      if(isbackground){
        setpgid(pid, 0);
        pid_return_val = waitpid(0, &status, WNOHANG);
      } else{
        pid_return_val = waitpid(pid, &status, WUNTRACED); 
      }
    }

    return status; //return the child process status
}
int processinput(char *args[], int size){
    
    int isbackground = 0;
    for(size_t k = 0; k < size; k++)
    {
      if(args[k] == NULL){
        break;
      }
      if(strcmp(args[k], "&") == 0){
        isbackground = 1;
        break;
      }
    }
    size_t i = 0; 
    return processcommand(args, size, &i, isbackground);
}

int processcommand(char *args[], int size, size_t *i, int isbackground){
  int status = 0;
  size_t j = 0;
  char *tempcommand[size];

  while (*i < size){
    if (args[*i] == NULL){

      tempcommand[j] = args[*i];      
      status = execute(tempcommand, size, isbackground, 0, NULL);
      return status;

    } else if (strcmp(args[*i], "if") == 0 && !isbackground){
      (*i)++;
      return processif(args, size, i, isbackground);

    } else if (strcmp(args[*i], "&&" ) == 0 && !isbackground){
      
      tempcommand[j] = NULL; // set as null to process the command
      (*i)++;
      status = execute(tempcommand, size, isbackground, 0, NULL);
      if(status == 0){
        //AND case, continue on success
        return processcommand(args, size, i, isbackground);
      }
    } else if (strcmp(args[*i], "||") == 0 && !isbackground){
      tempcommand[j] = NULL; // set as null to process the command
      (*i)++;
      status = execute(tempcommand, size, isbackground, 0, NULL);
      if(status != 0){
        //OR case, continue on fail
        return processcommand(args, size, i, isbackground);
      }
      break;
    } else if (strcmp(args[*i], "&") == 0){ 
      tempcommand[j] = NULL; // set as null to process the command
      (*i)++;
      status = execute(tempcommand, size, isbackground, 0, NULL);
      break;
    } else if (strcmp(args[*i], ">") == 0 && !isbackground){ 
      tempcommand[j] = NULL;
      (*i)++;
      //next string is the filename
      status = execute(tempcommand, size, isbackground, 1, args[(*i)++]);
      return processcommand(args, size, i, isbackground);
    } else {
      tempcommand[j] = args[*i];
      (*i)++;
      j++;
    }
  }
  return status;
}

int processif(char *args[], int size, size_t *i, int isbackground){
  int status = 1;
  int condstatement = 0;
  size_t j = 0;
  char *condcommand[size];
  char *docommand[size];

  while (*i < size){
    if (strcmp(args[*i], "if") == 0){
      (*i)++;
      condstatement = processif(args, size, i, isbackground); //conditionnal block is a if
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
      condstatement = processcommand(condcommand, size, &tempindex, isbackground);
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
    while (*i < size){
      if (strcmp(args[*i], "if") == 0){
        (*i)++;
        return processif(args, size, i, isbackground);
      } else if (strcmp(args[*i], ";") == 0) {
        (*i)++;
        docommand[j] = NULL;
        size_t tempindex = 0;
        status = processcommand(docommand, size, &tempindex, isbackground);
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

int spacecount(char *text){
  int count = 2;
  while(*text){
    if(*text == ' '){
      count++;
    }
    text++;
  }
  return count;
}

