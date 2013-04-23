
/* forktest.c - vanlig användning av systemanropet fork() */

/* Include-rader ska vara allra först i ett C-program */
#include <sys/types.h> /* definierar bland annat typen pid_t */
#include <errno.h> /* definierar felkontrollvariabeln errno */
#include <stdio.h> /* definierar stderr, dit felmeddelanden skrivs */
#include <stdlib.h> /* definierar bland annat exit() */
#include <unistd.h> /* definierar bland annat fork() */

pid_t childpid; /* processid som returneras från fork */

int main()
{
  /* Följande kod ska finnas inne i main() eller annan procedur! */

  childpid = fork();
  if( 0 == childpid )
  {
    /* denna kod körs endast i child-processen */
  }
  else
  {
    /* denna kod körs endast i parent-processen */
    
    if( -1 == childpid ) /* fork() misslyckades */
      {
        /* i C får man alltid deklarera variabler efter vänster krullparentes */
        char * errormessage = "UNKNOWN"; /* felmeddelandetext */

        if( EAGAIN == errno ) errormessage = "cannot allocate page table";
        if( ENOMEM == errno ) errormessage = "cannot allocate kernel data";
        fprintf( stderr, "fork() failed because: %s\n", errormessage );
        exit( 1 ); /* programmet avbryts om detta fel uppstår */
      }

    /*
      Kommer vi hit i koden så är vi i parent-processen
      och fork() har fungerat bra - i så fall innehåller
      variabeln childpid child-processens process-ID
    */

  }

  exit( 0 ); /* programmet avslutas normalt */
}
