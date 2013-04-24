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

int pipe_printenv[2];
int pipe_grep[2];
int pipe_sort[2];

int main(int argc, char **argv, char **envp) {

	int return_value;
	
	pipe(pipe_printenv); 

	/*
		PRINTENV
	*/
	child_pid = fork(); 
	if(0 == child_pid) {

		/*
			Skicka till nästa
		*/
		close(pipe_printenv[PIPE_READ]);
		dup2(pipe_printenv[PIPE_WRITE], 1);
		close(pipe_printenv[PIPE_WRITE]);

		execlp("printenv","printenv", NULL);
	} 

	close(pipe_printenv[PIPE_WRITE]);
	dup2(pipe_printenv[PIPE_READ], 0);
 	close(pipe_printenv[PIPE_READ]);

	pipe(pipe_grep); 


	/*
		GREP
	*/
	child_pid = fork(); 
	if(0 == child_pid) {
			close(pipe_grep[PIPE_READ]);
			dup2(pipe_grep[PIPE_WRITE], 1);
			close(pipe_grep[PIPE_WRITE]);

		if(argc > 1){
			execvp("grep", argv);
		}
	} 

	close(pipe_grep[PIPE_WRITE]);
	dup2(pipe_grep[PIPE_READ], 0);
	close(pipe_grep[PIPE_READ]);

	pipe(pipe_sort); 


	/*
		GREP
	*/
	child_pid = fork(); 
	if(0 == child_pid) {
			close(pipe_sort[PIPE_READ]);
			dup2(pipe_sort[PIPE_WRITE], 1);
			close(pipe_sort[PIPE_WRITE]);

			execlp("sort", "sort", NULL);
	} 


	/*
		PAGER
	*/
	close(pipe_sort[PIPE_WRITE]);
	dup2(pipe_sort[PIPE_READ], 0);
	close(pipe_sort[PIPE_READ]);


	char* pager = getenv("PAGER");
	if(pager == NULL){
		pager = "less";
	}
	execlp(pager, pager,NULL);

}