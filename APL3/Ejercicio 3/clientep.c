#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define TAM_COMANDO 100  
#define SIN_STOCK 1
#define LIST 2
#define QUIT 3
#define STOCK 4
#define REPO 5
#define INVALIDO 77
 
void mostrarOpciones(); 
void resolver(); 
void ayuda();
int validarComando(char *comando, int *parteEntera);
void removerSaltoDeLinea(char *string);
int crearFifos();
void borrarFifos();

typedef struct {

	char comando[TAM_COMANDO];
	int parteNumerica;
}t_comando;

int main(int argc, char *argv[]) {  
	
	if(crearFifos() == -1){
		printf("ERROR AL CREAR LAS FIFOS\n\n");
		exit(1);
	}
	
	resolver();
	
	borrarFifos();
	
	     
	//Se revisa si se recibió un parametro de ayuda     
	/*if(argc == 1){         
		if(strcmp(argv[1],"-help") == 0 || strcmp(argv[1],"-h") == 0){             					ayuda();             
		}   
	} else if(argc >= 1){         
		printf("Este script NO recibe parametros.\n");         
		ayuda();     
	} else {         
		resolver();     
	}      */
	return 0; 
}

void mostrarOpciones(){    
	printf("\n---------------LISTADO DE OPCIONES---------------\n\n");     
	printf("STOCK ID_Producto|Muestra descripcion y stock para el producto dado\n\n");         
	printf("SIN_STOCK|Muestra ID, descripcion y costo de los productos con STOCK cero\n\n");     
	printf("REPO Cantidad|Muestra el costo total de reponer una cantidad dada para cada producto sin stock\n\n");         
	printf("LIST|Muestra ID, descripcion y precio de todos los productos existentes\n\n");     
	printf("QUIT|Finaliza la ejecucion\n\n"); 
}   

void resolver(){       
	char comando[TAM_COMANDO]; 
	char fin[TAM_COMANDO] = "QUIT";
	int tipoComando;
	int parteEntera;
	t_comando comandoEntero;
	int fifo1, fifo2;
	
	printf("LLEGA HASTA ACA\n");
	
	fifo1 = open("./fifo1", O_WRONLY);
	fifo2 = open("./fifo2", O_RDONLY);
	
	printf("PASO LA PRIMERA");
	
	if(fifo1 == -1 || fifo2 == -1) {
		printf("Error al abrir las fifos.\n");
		exit(1);
	}
	  
	do{         
		mostrarOpciones();         
		printf("Ingrese la acción que desea:");      
		fgets(comando, TAM_COMANDO, stdin);
		removerSaltoDeLinea(comando);
		tipoComando = validarComando(comando, &parteEntera);  
		
		system("clear");
		
		if(tipoComando == INVALIDO){
			printf("-------------Comando invalido-------------\n\n");
		} else {
			//ENVIAR COMANDO
			strcpy(comandoEntero.comando, comando);
			comandoEntero.parteNumerica = parteEntera;
			write(fifo1, &comandoEntero, sizeof(t_comando));
			
			//RECIBIR RESPUESTA
		}
		
	}while(strcmp(comando,"QUIT") != 0);     
}   

void removerSaltoDeLinea(char *string){
	int largo = strlen(string);
	
	if(largo > 1 && string[largo-1] == '\n')
		string[largo-1] = '\0';
}

int validarComando(char *comando, int *parteEntera){
	
	char comandoString[TAM_COMANDO];
	int comandoEntero;
	
	if(!strcmp(comando, "SIN_STOCK"))
		return SIN_STOCK;
	else if(!strcmp(comando, "LIST"))
		return LIST;
	else if(!strcmp(comando, "QUIT"))
		return QUIT;
	else {
		sscanf(comando, "%s %d", comandoString, &comandoEntero);
		
		//printf("%s   a\n", comandoString);
		if(!strcmp(comandoString, "STOCK")){
			*parteEntera = comandoEntero;
			return STOCK;
		} else if(!strcmp(comandoString, "REPO")){
			*parteEntera = comandoEntero;
			return REPO;
		} else 
			return INVALIDO;
	}	
} 

int crearFifos(){

	if(mkfifo("./fifo1", 0666) == -1 || mkfifo("./fifo2", 0666) == -1)
		return -1;
	else 
		return 1;
}

void borrarFifos(){
	unlink("./fifo1");
	unlink("./fifo2");
}

void ayuda(){     
	printf("ayuda\n"); 
} 
