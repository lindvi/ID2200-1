#include <stdio.h>
#include <stdlib.h>

/* TODO:
- Kontrollera ruturvärden från systemanrop!
- Längre beskrivande kommentar, om flera c filer i samtliga
- Samma för metoderna
- Kommentar för alla variabler
- Kommentarer vid icke trivial kod

 */

int main(int argc, char **argv, char **envp) {
	int i;
	for(i=0; envp[i]!= NULL; i++) 
		printf("%2d:%s\n",i, envp[i]);
}