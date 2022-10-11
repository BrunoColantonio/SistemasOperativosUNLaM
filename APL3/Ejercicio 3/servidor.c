#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define TAM_COMANDO 100  
#define TAM_ARCHIVO 50
#define SIN_STOCK 1
#define LIST 2
#define QUIT 3
#define STOCK 4
#define REPO 5
#define INVALIDO 77

typedef struct {

	char comando[TAM_COMANDO];
	int parteNumerica;
}t_comando;

int main(int argc, char *argv[]){
	
	char archivo[TAM_ARCHIVO];
	int fifo1, fifo2;
	
	fifo1 = open("./fifo1", O_RDONLY);
	fifo2 = open("./fifo2", O_WRONLY);
	
	printf("PASO LA PRIMERA");
	
	if(fifo1 == -1 || fifo2 == -1) {
		printf("Error al abrir las fifos.\n");
		exit(1);
	}
	//Se revisa si se recibió un parametro de ayuda     
	/*if(argc == 1){         
		if(strcmp(argv[1],"-help") == 0 || strcmp(argv[1],"-h") == 0){             					ayuda();             
		} else{
			strcpy(archivo, argv[1]);
		}
	} else {         
		printf("Error de parametros.\n\n");         
		ayuda();     
	} 
	
	fifo1 = open("./fifo1", O_WRONLY);
	fifo2 = open("./fifo2", O_RDONLY);
	
	if(fifo1 == -1 || fifo2 == -1) {
		printf("Error al abrir las fifos.\n");
		exit(1);
	}*/
	
}
