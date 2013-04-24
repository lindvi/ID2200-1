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

#define PRINTENV 	(0)	// 
#define GREP		(1)	// Identifikation för pipes (human readable)
#define SORT		(2)	//


pid_t child_pid;		// Struct för PID info
int pipes[3][2];		// Lagrar alla pipes (Printenv, grep och sort)


int pipeRead( int pipe ) {
	// Stäng read-änden av pipen för att signalera till den mottagande att vi är klara med att lyssna
	if(close(pipes[pipe][PIPE_WRITE]) == -1) {
		perror("pipeRead: Cannot close WRITE");
		return -1;
	}
	dup2(pipes[pipe][PIPE_READ], 0);
	
	if(close(pipes[pipe][PIPE_READ]) == -1) {
		perror("pipeRead: Cannot close READ");
		return -1;
	}
	return 0;
}

int pipeWrite( int pipe ) {
	if(close(pipes[pipe][PIPE_READ]) == -1)	
		perror("pipeWrite: Cannot close READ");
	
	dup2(pipes[pipe][PIPE_WRITE], 1);
	
	if(close(pipes[pipe][PIPE_WRITE]) == -1)
		perror("pipeWrite: Cannot close WRITE");
	
	return 0;
}

int main(int argc, char **argv, char **envp) {
	int DBG = 1;
	pipe(pipes[PRINTENV]); 

	/*
		PRINTENV
	*/
	child_pid = fork(); 
	if(0 == child_pid) {
		if(DBG)printf("[INFO] Printenv Child\n");
		/*
			Skicka till nästa
		*/
		pipeWrite(PRINTENV);
		execlp("printenv","printenv", NULL);
	}
	if(DBG)printf("[INFO] Printenv Parent\n");
	pipeRead(PRINTENV);


	/*
		GREP
	*/
	if(argc > 1) {
		pipe(pipes[GREP]); 
		child_pid = fork(); 
		if(0 == child_pid) {
			if(DBG)printf("[INFO] Grep Child\n");
			execvp("grep", argv);
		} 
		if(DBG)printf("[INFO] Grep Parent\n");
		pipeRead(GREP);
	}

	/*
		SORT
	*/
	pipe(pipes[SORT]); 
	child_pid = fork(); 
	if(0 == child_pid) {
			if(DBG)printf("[INFO] Sort Child\n");
			pipeWrite(SORT);
			execlp("sort", "sort", NULL);
	} 
	if(DBG)printf("[INFO] Sort Parent\n");
	pipeRead(SORT);

	/*
		PAGER
	*/
	


	char* pager = getenv("PAGER");
	if(pager == NULL){
		pager = "less";
	}
	execlp(pager, pager,NULL);
	return 0;
}