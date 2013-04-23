/* TODO:
- Kontrollera ruturvärden från systemanrop!
- Längre beskrivande kommentar, om flera c filer i samtliga
- Samma för metoderna
- Kommentar för alla variabler
- Kommentarer vid icke trivial kod
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* definierar bland annat pipe() och STDIN_FILENO */
#include <sys/types.h> /* definierar typen pid_t */
/*
#define PIPE_READ 	(0)
#define PIPE_WRITE 	(1)
*/

int PIPE_READ = 0;
int PIPE_WRITE = 1;

pid_t child_pid;

int main(int argc, char **argv, char **envp) {
	int pipe; 		/* för returvärden från systemanrop */
	int pipe_filedesc[2]; 	/* för fildeskriptorer från pipe(2) */
	
	// Debug info:
	fprintf( stderr, "Parent (producer, pid %ld) started\n", (long int) getpid() );
	
	// Skapa en pipe
	pipe = pipe( pipe_filedesc ); 
	
	if(pipe == -1) { 
		/* avsluta programmet om pipe() misslyckas */
		perror( "Cannot create pipe" ); 
		exit( 1 ); 
	}

	child_pid = fork(); /* skapa en child-process */

	if(child_pid = 0) {
		// Detta är första childen, dvs grep ska köras här om parametrar finns.
		if(argc > 0) {
			// Argument finns! Kalla på grep.
		} else {
			// Inga argument fortsätt med sort.
		}
	} else {
		// Detta är parent, kalla på printenv eller kör nedanstående for loop och skicka via pipe till child.
	}

	// Hitta pager här eller strunta i det och alltid köra less?
	/* OLD SHIT
	int i;
	for(i=0; envp[i]!= NULL; i++) 
		printf("%2d:%s\n",i, envp[i]);

	if (fork()) { 
		// printenv - parent

	} else if (argc >0 && fork()) {
		// grep 	- Child 
		// Bara om parametrar finns
	} else if (fork()) {
		// sort 	- Grand-child 
	} else if (fork()) {
		// less 	- grand-grand-child 
	}
	*/
}