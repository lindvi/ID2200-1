/* TODO:
- Kontrollera ruturvärden från systemanrop!
- Längre beskrivande kommentar, om flera c filer i samtliga
- Samma för metoderna
- Kommentar för alla variabler
- Kommentarer vid icke trivial kod

	digenv.c
================
	Programmet utför följande kommando: 
	"printenv | grep [parameter] | sort | less"	
	alternativt: 
	"printenv | sort | less" om ingen parameter angivits.

	Syftet med denna kod är att få förståelse för hur pipes fungerar i linux
	och hur man använder dom i kod.

	CHANGELOG:
	- Behöver bara en fdarray för pipes.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* definierar bland annat pipe() och STDIN_FILENO */
#include <sys/types.h> /* definierar typen pid_t */

#define PIPE_READ 	(0) // Identifikation för kanaler i pipes (human readable)
#define PIPE_WRITE 	(1) // 

#define DBG			(1) // För utskrivning av debuggmeddelanden


pid_t child_pid;		// Struct för PID info
int pipefd[2];			// Array för att lagra file descriptor till pipes

int createPipe() {
	if(pipe(pipefd) < 0) {
		perror("createPipe: Error occured while creating pipe...");
		return -1;
	}
	return 0;
}

/*	pipeRead()
	Gör följande: 
	1. Stänger pipen för skrivning
	2. Styr om stdout till stdin
	3. Stänger pipen för läsning för att på så sätt signalera till den andra änden att den är klar.
*/
int pipeRead() {
	if(close(pipefd[PIPE_WRITE]) == -1) {
		perror("pipeRead: Cannot close WRITE");	// Fel har uppstått! Skriv meddelande i terminalen
		return -1;								// Returnera -1
	}
	
	dup2(pipefd[PIPE_READ], 0);					// Styr om stdin till pipen
		
	//if(close(pipefd[PIPE_READ]) == -1) {
	//	perror("pipeRead: Cannot close READ");	// Fel har uppstått! Skriv meddelande i terminalen
	//	return -1;								// Returnera -1
	//}
	return 0;									// Allt har gått bra! Returnera 0
}

/*	pipeWrite()
	Gör följande: 
	1. Stänger pipen för läsning
	2. Styr om stdin till stdout
	3. Stänger pipen för skrivning för att på så sätt signalera till den andra änden att den är klar.
*/
int pipeWrite() {
	if(close(pipefd[PIPE_READ]) == -1) {
		perror("pipeWrite: Cannot close READ");	// Fel har uppstått! Skriv meddelande i terminalen
		return -1;								// Returnera -1
	}
	
	dup2(pipefd[PIPE_WRITE], 1);				// Styr om stdout till pipen
	
	//if(close(pipefd[PIPE_WRITE]) == -1) {
	//	perror("pipeWrite: Cannot close WRITE");// Fel har uppstått! Skriv meddelande i terminalen
	//	return -1;								// Returnera -1
	//}	
	return 0;
}

int main(int argc, char **argv, char **envp) {
	if(createPipe() < 0) exit(1);	// Skapar en pipe, avslutar vid fel

	/*  PRINTENV  */
	child_pid = fork(); 
	if(0 == child_pid) {
		if(DBG)printf("[INFO] Printenv Child\n");
		pipeWrite();
		execlp("printenv","printenv", NULL);
	}
	if(DBG)printf("[INFO] Printenv Parent\n");
	pipeRead();


	/*  GREP  */
	if(argc > 1) {
		if(createPipe() < 0) exit(1);	// Skapar en pipe, avslutar vid fel
		child_pid = fork(); 
		if(0 == child_pid) {
			if(DBG)printf("[INFO] Grep Child\n");
			pipeWrite();	// GREP
			execvp("grep", argv);
		} 
		if(DBG)printf("[INFO] Grep Parent\n");
		pipeRead();		// GREP
	}

	/*  SORT  */
	if(createPipe() < 0) exit(1);	// Skapar en pipe, avslutar vid fel
	child_pid = fork(); 
	if(0 == child_pid) {
			if(DBG)printf("[INFO] Sort Child\n");
			pipeWrite();	// SORT
			execlp("sort", "sort", NULL);
	} 
	if(DBG)printf("[INFO] Sort Parent\n");
	pipeRead();		// SORT

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