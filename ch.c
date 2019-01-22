/* ch.c.
auteur:
date:
problèmes connus:

  */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

int main (void)
{
  fprintf (stdout, "%% ");

  /* ¡REMPLIR-ICI! : Lire les commandes de l'utilisateur et les exécuter. */
  int bytes_read;
  size_t size;
  char *input = NULL;
  while (bytes_read = getline(&input, &size, stdin)) {
	  fprintf(stdout, "%% ");
	  //fork() et exec() avec le bon programme
  }
  free(input);
  fprintf (stdout, "Bye!\n");
  exit (0);
}
