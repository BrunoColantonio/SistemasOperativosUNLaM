/*
-------------------------- ENCABEZADO --------------------------
Nombre del programa: cliente4.c
Número de APL: 3
Número de ejercicio: 4
Número de entrega: Entrega
---------------- INTEGRANTES DEL GRUPO ----------------
Apellido, Nombre          | DNI
Agostino, Matías          | 43861796
Colantonio, Bruno Ignacio | 43863195
Fernández, Rocío Belén    | 43875244
Galo, Santiago Ezequiel   | 43473506
Panigazzi, Agustín Fabián | 43744593
-------------------- FIN DE ENCABEZADO --------------------
*/



#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h> 
#include <string.h> 
#include <strings.h>
#include <ctype.h>
#include <sys/shm.h> 	
#include <fcntl.h>	
#include <semaphore.h> 
#include <signal.h>
#define SEGMENTO_ID		504
#define SEGMENTO_ID2	786 
#define REGISTROS 	1

typedef struct 
{
    char nombre[15];
    char raza[15];
    char sexo;
    char castracion[3];
}Gato;

typedef struct{
	int decision;
	Gato gato;
}Decision;

void mostrarAyuda();

int main(int argc, char* argv[]){	

	signal(SIGINT, SIG_IGN);
	Decision des;
	sem_t *leer		 =	sem_open("/leer",		O_CREAT);
	sem_t *escribir =	sem_open("/escribir",O_CREAT);
	
	int shmid = shmget(SEGMENTO_ID, sizeof(Decision), IPC_CREAT | 0666);
	int shmid2 = shmget(	SEGMENTO_ID2, sizeof(int), IPC_CREAT | 0666);	
	
	Decision *area_compartida = (Decision*)shmat( shmid, NULL, 0);
	int *area_compartidaRes = (int*)shmat(shmid2,NULL,0);

	int opcion;
		if(strcasecmp(argv[1],"ALTA")==0 && argc==6)
			opcion = 1;
		else if(strcasecmp(argv[1],"BAJA")==0 && argc == 3)
			opcion = 2;
		else if(strcasecmp(argv[1],"CONSULTA")==0 && argc == 3)
			opcion = 3;
		else if (strcasecmp(argv[1],"CONSULTA")==0 && argc == 2)
			opcion = 4;
		else if(strcasecmp("-h", argv[1]) == 0 || strcasecmp("--help", argv[1]) == 0)
		{
			mostrarAyuda();
			return 0;
		}
		else
		{
			puts("----------Error de parametros----------");
			puts("\nPara mostrar la ayuda, ejecute:");
			puts("- ./clienteComp -h");
			puts("- ./clienteComp --help");
			return 0;
		}
			
	

		switch (opcion)
		{
		case 1:
			des.decision = 1;

			strcpy(des.gato.nombre,argv[2]);
			strcpy(des.gato.raza,argv[3]);
			des.gato.sexo = *argv[4];
			strcpy(des.gato.castracion,argv[5]);
			des.gato.sexo = toupper(des.gato.sexo);
			des.gato.castracion[0] = toupper(des.gato.castracion[0]);
			des.gato.castracion[1] = toupper(des.gato.castracion[1]);

			if((des.gato.sexo=='M' || des.gato.sexo=='H') && 
				(strcasecmp(des.gato.castracion,"SC")==0 || strcasecmp(des.gato.castracion,"CA")==0))
			{
				sem_wait(escribir); // P(escribir)
				*area_compartida = des;
				sem_post(leer); // V(leer)

				sleep(0.1);
				
				sem_wait(leer);

				if(*area_compartidaRes == 1)
					printf("%s ha sido ingresado correctamente\n",des.gato.nombre);
				else if(*area_compartidaRes == 2)
					printf("El gato: %s, ya se encuentra cargado en el sistema\n", des.gato.nombre);

				sem_post(escribir);
			}
			else
				puts("ERROR EN LOS PARAMETROS");


			break;
		case 2:
			des.decision = 2;
			strcpy(des.gato.nombre,argv[2]);

			sem_wait(escribir); // P(escribir)
			*area_compartida = des;
			sem_post(leer); // V(leer)

			sleep(0.1);
			
			sem_wait(leer);

			if(*area_compartidaRes == 0)
				printf("%s no existe en el sistema\n",des.gato.nombre);
			else if(*area_compartidaRes == 1)
				printf("Se ha registrado la adopcion de %s correctamente\n", des.gato.nombre);

			sem_post(escribir);
			

			break;
		case 3:
			des.decision = 3;
			strcpy(des.gato.nombre,argv[2]);
			
			sem_wait(escribir); // P(escribir)
			*area_compartida = des;
			sem_post(leer); // V(leer)

			sleep(0.1);
			
			sem_wait(leer);
			des = *area_compartida;

			if(*area_compartidaRes == 0)
				printf("%s no existe en el sistema\n",des.gato.nombre);
			else if(*area_compartidaRes == 1)
				printf("Nombre: %s\nRaza: %s\nSexo: %c\nEstado de castracion: %s\n", 
				des.gato.nombre, des.gato.raza, des.gato.sexo, des.gato.castracion);

			sem_post(escribir);

			break;
		case 4:
			des.decision = 4;

			sem_wait(escribir); // P(escribir)
			*area_compartida = des;
			sem_post(leer); // V(leer)

			sleep(0.1);
			
			int resultado = 1;
			printf("%-15s|%-15s|%-5s|%-21s\n","Nombre","Raza","Sexo","Estado de castracion");
			printf("---------------|---------------|-----|--------------------\n");

			while(resultado != 0)
			{
				sem_wait(leer);
				resultado = *area_compartidaRes;
				if(resultado!=0)
				{
					des = *area_compartida;

					printf("%-15s|%-15s|%-5c|%-21s\n", 
					des.gato.nombre, des.gato.raza, des.gato.sexo, des.gato.castracion);
				}
				else
					puts("No hay mas gatos en el sistema");
				

				sem_post(escribir);

				sleep(0.5);
			}


			break;
		}

	
	shmdt( &area_compartida );	
	shmdt( &area_compartidaRes);



	sem_close( leer );
	sem_close( escribir ); 

	 	
	return 0;
}



void mostrarAyuda()
{
	puts("-------------- AYUDA --------------");
	puts("\n  Este programa desea llevar un registro minimo de los rescates y adopciones de gatos del refugio.\n" 
		"Para ello se crea un servidor y un cliente que se comunican a traves de memoria compartida.");
	puts("Comandos para el Cliente:");
	puts("- 1. ./clientesComp \"ALTA\" \"Nombre\" \"Raza\" \"Sexo(H-M)\" \"Estado Castracion(CA-SC)\" ");
	puts("- 2. ./clientesComp \"BAJA\" \"Nombre\" ");
	puts("- 3. ./clientesComp \"CONSULTA\" \"Nombre\" ");
	puts("- 4. ./clientesComp \"CONSULTA\" (Para hacer una consulta general)");
}