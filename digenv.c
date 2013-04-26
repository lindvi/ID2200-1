/*  digenv.c
================
	Programmet utför följande kommando: 
	"printenv | grep [parameter] | sort | less"	
	alternativt: 
	"printenv | sort | less" om ingen parameter angivits.

	Syftet med denna kod är att få förståelse för hur pipes, fork och exec fungerar
	i linux	och hur man använder dom i kod.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 	// definierar bland annat pipe() och STDIN_FILENO
#include <sys/types.h>	// definierar typen pid_t

#define PIPE_READ 	(0) // Identifikation för kanaler i pipes (human readable)
#define PIPE_WRITE 	(1) // Identifikation för kanaler i pipes (human readable)
#define DBG			(1) // För utskrivning av debuggmeddelanden

pid_t child_pid;		// Struct för PID info
int pipefd[2];			// Array för att lagra file descriptor till pipes

/* 	createPipe() - Skapar en ny pipe och lagrar det i pipefd[]
	Uppstår det problem vid skapandet returnerar funktionen -1, annars 0. */
int createPipe() {
	if(pipe(pipefd) < 0) return -1;
	return 0;
}
/*	pipeRead()
	Kopplar ihop pipen(read) med stdin och stänger pipen.
*/
int pipeRead() {
	dup2(pipefd[PIPE_READ], 0);						// Styr om stdin till pipen
	if(close(pipefd[PIPE_WRITE]) == -1)	return -1;	// Problem med att stänga fd? Returnera -1
	if(close(pipefd[PIPE_READ]) == -1)	return -1;	// Problem med att stänga fd? Returnera -1
	return 0;										// Allt har gått bra! Returnera 0
}
/*	pipeWrite()
	Kopplar ihop stdout till pipen(write) och stänger pipen.
*/
int pipeWrite() {
	dup2(pipefd[PIPE_WRITE], 1);					// Styr om stdout till pipen
	if(close(pipefd[PIPE_READ]) == -1) 	return -1;	// Problem med att stänga fd? Returnera -1
	if(close(pipefd[PIPE_WRITE]) == -1) return -1;	// Problem med att stänga fd? Returnera -1
	return 0;
}


int main(int argc, char **argv, char **envp) {
	if(createPipe() < 0) exit(1);	// Skapar en pipe, avslutar vid fel

	/*  PRINTENV  */
	child_pid = fork(); 		// Forka, dvs skapa en ny process med i princip samma innehåll.
	if(0 == child_pid) {		// Kolla om vi är "barnet"
		if(pipeWrite()<0) exit(1);			// Fixa till pipen så stdout hamnar i pipen istället.
		execlp("printenv","printenv", NULL);	// Byt ut koden i barnet till printenv istället. Printenv avslutar sig själv.
	}
	if(pipeRead()<0) exit(1);					// Föräldraprocessen - läser av pipen till stdin

	/*  GREP  */
	if(argc > 1) {						// Kontrollera om parameter angivits, annars ignorera grepp
		if(createPipe() < 0) exit(1);	// Skapar en pipe, avslutar vid fel
		child_pid = fork(); 			// forka
		if(0 == child_pid) {			// Är vi barnet?
			if(pipeWrite()<0) exit(1);	// Fixa till pipen så stdout hamnar i pipen istället.
			if(execvp("grep", argv)<0) exit(1); // Starta grepp i barnet, kontrollera exit code (funkar sisådär)
		} 
		pipeRead();					// Föräldraprocessen - läser av pipen till stdin
	}

	/*  SORT  */
	if(createPipe() < 0) exit(1);	// Skapar en pipe, avslutar vid fel
	child_pid = fork(); 			// Forkar
	if(0 == child_pid) {			// Är vi barnet?
		if(pipeWrite()<0) exit(1);	// Fixa till pipen så stdout hamnar i pipen istället.
		execlp("sort", "sort", NULL);	// Starta sort i barnet.
	} 
	pipeRead();						// Föräldraprocessen - läser av pipen till stdin

	/*  PAGER  */
	char* pager = getenv("PAGER");	// Kolla om det finns någon pager angivet, annars less
	if(pager == NULL){
		pager = "less";
	}
	execlp(pager, pager,NULL);		// Starta less
	return 0;						// Måste vara med, ifall exec skulle misslyckas. Får varningar av kompilatorn annars.
}








/*
TODO:
- Kontrollera ruturvärden från systemanrop!
- Längre beskrivande kommentar, om flera c filer i samtliga
- Samma för metoderna
- Kommentar för alla variabler
- Kommentarer vid icke trivial kod
*/