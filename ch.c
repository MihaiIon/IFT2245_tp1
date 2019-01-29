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

    parseargs(input, args); //split at spaces
    for(size_t i = 0; i < sizeof(args); i++)
    {
      if(args[i] == NULL){
        break;
      }
      printf("%dl Printing the input: %s\n", i, args[i]);
    }
    
    // if(args[0] != NULL){
    //   printf("%s\n",args[0]);
    // }

    //master loop for one input
    size_t i = 0;
    size_t maxargs = sizeof(args);
    int status;


    while(i < maxargs){
      printf("loop start %d\n", i);
      char *tempcommand[64];
      int andflag = 0;
      int orflag = 0;
      size_t j = 0;
      for(;;j++)
      {
        if(args[i] == NULL){
          printf("null\n");
          tempcommand[j] = args[i];
          //NULL signifies the end of the input
          i = maxargs;
          break;
        }
        if(strcmp(args[i], "&&") == 0){
          //command is over
          printf("AND\n");
          andflag = 1;
          tempcommand[j] = NULL; // set as null to process the command
          i++;
          break;
        }
        if(strcmp(args[i], "||") == 0){
          //command is over
          printf("OR\n");
          orflag = 1;
          tempcommand[j] = NULL; // set as null to process the command
          i++;
          break;
        }
        tempcommand[i] = args[i];
        printf("copy %s\n", args[i]);
        i++;
      }
      printf("after for\n");

      for(size_t k = 0; k < sizeof(tempcommand); k++)
      {        
        if(tempcommand[k] == NULL){
          break;
        }
        printf("%d Printing tempcommand : %s\n", k,tempcommand[k]);

      }
      
      status = execute(tempcommand);
      if(andflag && status != 0){
        //l'operation n'a pas fonctionné
        break;
      }
      else if(orflag && status == 0){
        //on est dans un OR on continue jusqua un succes
        break;
      }
      printf("Exit status : %d\n", status);
    }
    fprintf (stdout, "%% ");
    
    // command = tempcommand[0]; //tempcommand[0] contains the command and filename

    // pid = fork();
    // if(pid < 0){
    //   return 1;
    // }
    // else if(pid == 0) {
    //   //handling child process
    //   execvp(command, tempcommand);
    //   //if execvp did not work
    //   printf("%s: command not found\n", command);
    //   exit(0);
    // }
    // else{
    //   //parent process
    //   pid_t pid_return_val;
    //   int status;
    //   pid_return_val = wait(&status); //do something with return value
    //   fprintf (stdout, "%% ");
    // }
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
      pid_return_val = wait(&status); //do something with return value
    }
    return status;//return the child process status
}
