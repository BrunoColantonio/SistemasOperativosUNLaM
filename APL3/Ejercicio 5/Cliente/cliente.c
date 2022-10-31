/*
-------------------------- ENCABEZADO --------------------------
Nombre del programa: cliente.c / servidor.c
Número de APL: 3
Número de ejercicio: 5
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
//#include <fcntl.h>	
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>	

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



int main(int argc, char *argv[])
{	
    //SOCKETS 
	int puerto;
	int devolucion;

	struct sockaddr_in sa; //Server adress configs
	
 	if(argc<3 && (strcasecmp("-h", argv[1]) == 0 || strcasecmp("--help", argv[1]) == 0))
    {
        mostrarAyuda();
        return 1;
    }
    else if(argc<3)
    {
        puts("----------Error de parametros----------");
        puts("\nPara mostrar la ayuda, ejecute:");
        puts("- ./clienteComp -h");
        puts("- ./clienteComp --help");
        return 1;
    }

	//Crear el socket del cliente
	if((puerto=socket(AF_INET,SOCK_STREAM,0))==-1){	
		printf("No se pudo crear el socket !!!\n");	
		return 1;
	}

	//argv[0]->Nombre del ejecutable
	//argv[1]->IP
	//argv[2]->Puerto	
	bzero((char *) &sa, sizeof(struct sockaddr_in));
 	sa.sin_family		=	AF_INET;
 	sa.sin_port			=	htons(atoi(argv[2]));
	sa.sin_addr.s_addr=	inet_addr(argv[1]);

	//Solicitud de conexión
	if(connect(puerto,(struct sockaddr *) &sa,sizeof(sa))==-1){


		printf("Solicitud rechazada !!!\n");	
		return 1;
	}

	bzero(&devolucion,sizeof(int));
	recv(puerto,&devolucion,sizeof(int),0);
	if(devolucion==0)
	{
		puts("Capacidad del servidor completa. Vuelva a intentarlo mas tarde");
		close(puerto);
		
		return 1;
	}

    // REFUGIO
    Decision des;

    int opcion;

	char buffer[46];
	char nombre1[15], raza1[15], castracion1[3], accion[10], sexo1;
	int resultado;

	do
	{
		puts("\n\n-------------------MENU-----------------------");
		do{
			printf("1.ALTA(Nombre,Raza,Sexo(M-H),Castracion(CA-SC)\n");
			printf("2.BAJA(Nombre) \n");
			printf("3.CONSULTA(Nombre) \n");
			printf("4.CONSULTA() \n");
			printf("5.SALIR\n\n");
			printf("ingrese opcion: ");
			scanf("%d",&opcion);
		}while(opcion>5 || opcion<1);

		switch (opcion)
		{
			case 1:
				des.decision = 1;
				puts("\n--------------------------ALTA--------------------------");
				puts("ingrese comando (Nombre Raza Sexo(M-H) Castracion(CA-SC)");
				scanf(" %[^\n]s",buffer);
				puts("\n");
				sscanf(buffer,"%s %s %c %s", des.gato.nombre, des.gato.raza, &des.gato.sexo, des.gato.castracion);

				des.gato.sexo = toupper(des.gato.sexo);
				des.gato.castracion[0] = toupper(des.gato.castracion[0]);
				des.gato.castracion[1] = toupper(des.gato.castracion[1]);

				if((des.gato.sexo=='M' || des.gato.sexo=='H') && 
					(strcasecmp(des.gato.castracion,"SC")==0 || strcasecmp(des.gato.castracion,"CA")==0))
				{
					
					send(puerto,&des,sizeof(Decision),0);
					sleep(0.1);
					
					bzero(&des,sizeof(Decision));

					recv(puerto,&des,sizeof(Decision),0);

					if(des.decision == 1)
						printf("\n%s ha sido ingresado correctamente\n",des.gato.nombre);
					else if(des.decision == 2)
						printf("\nEl gato: %s, ya se encuentra cargado en el sistema\n", des.gato.nombre);
				}
				else
					puts("\nERROR EN LOS PARAMETROS");


				break;
			case 2:
				des.decision = 2;
				puts("\n----------BAJA----------");
				puts("ingrese comando (Nombre)");
				scanf(" %[^\n]s",buffer);
				sscanf(buffer,"%s", des.gato.nombre);

				send(puerto,&des,sizeof(Decision),0);
				sleep(0.1);
				bzero(&des,sizeof(Decision));
				recv(puerto,&des,sizeof(Decision),0);

				if(des.decision == 0)
					printf("\n%s no existe en el sistema\n",des.gato.nombre);
				else if(des.decision == 1)
					printf("\nSe ha registrado la adopcion de %s correctamente\n", des.gato.nombre);
				

				break;
			case 3:
				des.decision = 3;
				puts("\n--------CONSULTA--------");
				puts("ingrese comando (Nombre)");
				scanf(" %[^\n]s",buffer);
				sscanf(buffer,"%s", des.gato.nombre);
				
				send(puerto,&des,sizeof(Decision),0);
				sleep(0.1);
				bzero(&des,sizeof(Decision));
				recv(puerto,&des,sizeof(Decision),0);

				if(des.decision == 0)
					printf("\n%s no existe en el sistema\n",des.gato.nombre);
				else if(des.decision == 1)
					printf("\nNombre: %s\nRaza: %s\nSexo: %c\nEstado de castracion: %s\n", 
					des.gato.nombre, des.gato.raza, des.gato.sexo, des.gato.castracion);

				break;
			case 4:
				des.decision = 4;

				send(puerto,&des,sizeof(Decision),0);
				sleep(0.1);
				
				resultado = 1;
				printf("\n%-15s|%-15s|%-5s|%-21s\n","Nombre","Raza","Sexo","Estado de castracion");
				printf("---------------|---------------|-----|--------------------\n");

				while(resultado != 0)
				{
					bzero(&des,sizeof(Decision));
					recv(puerto,&des,sizeof(Decision),0);
					resultado = des.decision;
					if(resultado!=0)
					{
						printf("%-15s|%-15s|%-5c|%-21s\n", 
						des.gato.nombre, des.gato.raza, des.gato.sexo, des.gato.castracion);
					}
					else
						puts("No hay mas gatos en el sistema");

					sleep(0.5);
				}


				break;
			case 5:
				des.decision = 5;
				send(puerto,&des,sizeof(Decision),0);
				break;
		}

	} while(opcion>0 && opcion<5);
	
 	close(puerto);
 	return 0;
}

void mostrarAyuda()
{
	puts("-------------- AYUDA --------------");
	puts("\n  Este programa desea llevar un registro minimo de los rescates y adopciones de gatos del refugio.\n" 
		"Para ello se crea un servidor y clientes que se comunican a traves de sockets.");
	puts("Comandos para el Cliente:");
	puts("- 1. ./clientesComp \"IP\" \"Puerto\"");
	puts("luego debera elegir una opcion dependiendo de lo que se desee hacer");
}
