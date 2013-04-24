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

int pipes[3][2];

int pipeRead( int pipe ) {
	if(close(pipes[pipe][PIPE_WRITE]) == -1)perror("pipeRead: Cannot close WRITE");
	dup2(pipes[pipe][PIPE_READ], 0);
	if(close(pipes[pipe][PIPE_READ]) == -1)	perror("pipeRead: Cannot close READ");

	return 0;
}

int pipeWrite( int pipe ) {
	if(close(pipes[pipe][PIPE_READ]) == -1)	perror("pipeWrite: Cannot close READ");
	dup2(pipes[pipe][PIPE_WRITE], 1);
	if(close(pipes[pipe][PIPE_WRITE]) == -1)perror("pipeWrite: Cannot close WRITE");
	
	return 0;
}

int main(int argc, char **argv, char **envp) {

	pipe(pipes[PRINTENV]); 

	/*
		PRINTENV
	*/
	child_pid = fork(); 
	if(0 == child_pid) {

		/*
			Skicka till nästa
		*/
		pipeWrite(PRINTENV);
		execlp("printenv","printenv", NULL);
	} 
	pipeRead(PRINTENV);
	pipe(pipes[GREP]); 


	/*
		GREP
	*/
	child_pid = fork(); 
	if(0 == child_pid) {
		pipeWrite(GREP);
		if(argc > 1){
			execvp("grep", argv);
		}
	} 
	pipeRead(GREP);
	pipe(pipes[SORT]); 


	/*
		GREP
	*/
	child_pid = fork(); 
	if(0 == child_pid) {
			pipeWrite(SORT);
			execlp("sort", "sort", NULL);
	} 


	/*
		PAGER
	*/
	pipeRead(SORT);


	char* pager = getenv("PAGER");
	if(pager == NULL){
		pager = "less";
	}
	execlp(pager, pager,NULL);
	return 0;
}