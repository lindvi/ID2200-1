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

#define PIPE_READ 	(0)
#define PIPE_WRITE 	(1)

#define PRINTENV 	(0)
#define GREP		(1)
#define SORT		(2)


pid_t child_pid;

int pipe_filedesc[2];

int main(int argc, char **argv, char **envp) {

	int return_value;
	
	return_value = pipe(pipe_filedesc); 
	if(-1 == return_value) { 
		perror("Cannot create pipe"); 
		exit(1); 
	}

	/*
		PRINTENV
	*/
	child_pid = fork(); 
	if(0 == child_pid) {

		/*
			Skicka till nästa
		*/
		return_value = close(pipe_filedesc[PIPE_READ]);
	    if( -1 == return_value )
	    { 
	    	perror( "Cannot close pipe" ); 
			exit( 1 ); 
		}
		dup2(pipe_filedesc[PIPE_WRITE], 1);
		return_value = close(pipe_filedesc[PIPE_WRITE]);
		if( -1 == return_value )
	    { 
	    	perror( "Cannot close pipe" ); 
			exit( 1 ); 
		}
		execlp("printenv","printenv", NULL);
	} 


	return_value = close(pipe_filedesc[PIPE_WRITE]);
	if( -1 == return_value )
	    { 
	    	perror( "Cannot close pipe" ); 
			exit( 1 ); 
		}
	dup2(pipe_filedesc[PIPE_READ], 0);
	return_value = close(pipe_filedesc[PIPE_READ]);
	if( -1 == return_value )
	    { 
	    	perror( "Cannot close pipe" ); 
			exit( 1 ); 
		}

	execlp("less","less",NULL);

}