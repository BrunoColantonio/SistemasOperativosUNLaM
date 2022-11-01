/*
-------------------------- ENCABEZADO --------------------------
Nombre del programa: servidor.c
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
#include <unistd.h>
#include <memory.h>
#include <string.h>
#include <sys/ipc.h> 	//Biblioteca para los flags IPC_ 
#include <sys/shm.h> 	//Para memoria compartida SYSTEM-V	
#include <semaphore.h>	//Para semáforos POSIX
#include <fcntl.h>		//Para utilizar los flags O_
#include <signal.h>

#define SIN_MEM             0
#define TODO_OK             1
#define DUPLICADO           2
#define SEGMENTO_ID	    504 //La clave también puede ser creada con ftok()
#define SEGMENTO_ID2	786
#define REGISTROS 	1

#define min(a,b) ((a)<(b)? (a) : (b))

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


typedef struct s_nodo 
{
	void *dato;
	unsigned TamElem;
	struct s_nodo *sig;
}Nodo;



//lista
typedef Nodo* Lista;


typedef int (*Cmp)(const void* e1, const void* e2);
int cmp_nombre(const void* v1, const void* v2);

void crearLista(Lista* pl);
int insertarEnListaOrd(Lista *pl, const void* dato, size_t tamElem, Cmp cmp);
int buscarEnListaDesord(const Lista* pl, void* dato, size_t tamElem, Cmp cmp);
int eliminarDeListaOrdPorValor(Lista* pl, void* dato, size_t tamElem, Cmp cmp);
int eliminarDeListaUltimo(Lista* pl, void* dato, size_t tamElem);
int listaVacia(const Lista* pl);
int recorrerLista(const Lista* pl, void* dato, size_t tamElem, int i);
void vaciarLista(Lista* pl);

//archivo
void cargarListaArch(Lista* pl, char* path);
void cargarArchivo(Lista* pl, char* path);

// demonio

bool fin = false;
void manejador( int );

// Ayuda
void mostrarAyuda();




int main(int argc, char* argv[]){	

    if(argc>1 && (strcmp("-h", argv[1]) == 0 || strcmp("--help", argv[1]) == 0))
    {
			mostrarAyuda();
            return 0;
    }
    else if(argc>1)
    {
        puts("----------Error de parametros----------");
        puts("\nPara mostrar la ayuda, ejecute:");
	    puts("- ./servidorComp -h");
	    puts("- ./servidorComp --help");
        return 0;
    }

    char archivoServidor[100] = "servidor.txt";

    //SI YA HAY UNA INSTANCIA DE SERVIDOR, TERMINA EL PROGRAMA
    if(access(archivoServidor, F_OK) == 0)
    {
        puts("YA HAY UNA INSTANCIA DEL PROCESO SERVIDOR!");
        return 10;
    }

    FILE *pf = fopen(archivoServidor, "wt");

    // DEMONIO
        signal(SIGUSR1, manejador); //Se asocia la señal SIGUSR1 con el manejador propio
       pid_t pid = fork();

    //Si hay error de creación del hijo, finalizar
      if (pid < 0)
          return 1;

    //Si es el proceso padre, finalizar, así dejamos huérfano al hijo
    if (pid > 0){
        printf("PID del demonio: %d\n", pid);
           return 0;
    }

    //En este punto solo está el huérfano, que será el demonio

       // Creamos una nueva sesión para el demonio, básicamente con esto no quedará tomando la terminal /
     pid_t sid = setsid();
      if (sid < 0) {
       printf("Error al ejecutar setsid\n");
        return 1;
      }



    //
	//Creación de los semáforos
	sem_t *leer			=	sem_open( "/leer",		O_CREAT | O_EXCL,	0666,	0);
	sem_t *escribir	=	sem_open( "/escribir",	O_CREAT | O_EXCL,	0666,	1); 
	
	signal(SIGINT, SIG_IGN);

	Decision des;
	//Creación de memoria compartida
	int shmid = shmget(	SEGMENTO_ID, sizeof(Decision), IPC_CREAT | 0666);
	int shmid2 = shmget(	SEGMENTO_ID2, sizeof(int), IPC_CREAT | 0666);

	//Vincular la memoria compartida a una variable local
	Decision *area_compartida = (Decision*)shmat(shmid,NULL,0);
	int *area_compartidaRes = (int*)shmat(shmid2,NULL,0);

	int resultado;

	Lista lista;

    char path[15];
    strcpy(path,"Gatos.txt");
	crearLista(&lista);
    cargarListaArch(&lista,path);
    while(!fin)
    {
    
        sem_wait(leer);	//P( leer )
        des = *area_compartida;
        sem_post(escribir);

        switch (des.decision)
        {
        case 1:
            resultado = insertarEnListaOrd(&lista,&des.gato,sizeof(Gato),cmp_nombre);

            
            sem_wait(escribir); //P(escribir)
            *area_compartidaRes = resultado;
            sem_post(leer);
            break;
        case 2:
            resultado = eliminarDeListaOrdPorValor(&lista,&des.gato,sizeof(Gato),cmp_nombre);

            sem_wait(escribir); //P(escribir)
            *area_compartidaRes = resultado;
            sem_post(leer);

            break;
        case 3:
            resultado = buscarEnListaDesord(&lista,&des.gato,sizeof(Gato),cmp_nombre);

            sem_wait(escribir); //P(escribir)
            *area_compartidaRes = resultado;
            *area_compartida = des;
            sem_post(leer);

            break;
        case 4:

            int j = 0;
            int resultado=1;
            Lista listaaux = lista;
            while(resultado!=0)
            {
                resultado = recorrerLista(&listaaux,&des.gato,sizeof(Gato),j);
                sem_wait(escribir); //P(escribir)
                *area_compartidaRes = resultado;
                *area_compartida = des;
                sem_post(leer);
                j++;
                sleep(0.5);
            }

            break;
        
        //default:
        //	break;
        }
        usleep(100000);

    }

    cargarArchivo(&lista, path );

	//Desvincular la memoria compartida de la variable local
	shmdt( &area_compartida );
    shmdt( &area_compartidaRes );

	//Marcar la memoria compartida para borrar
	shmctl( shmid, IPC_RMID, NULL );
    shmctl( shmid2, IPC_RMID, NULL );

	//Cierre de los semáforos
	sem_close(leer); 
	sem_close(escribir); 	 

	//Marcar los semáforos para destruirlos
	sem_unlink("/leer");
	sem_unlink("/escribir");

    //CIERRO Y BORRO ARCHIVO PARA CORROBORAR QUE HAY UNA SOLA INSTANCIA
    fclose(pf);
    remove(archivoServidor);

	return 0;
}

// demonio

void manejador( int sig ){
    fin = true;
}


//archivo


void cargarListaArch(Lista* pl, char* path)
{
    FILE *arch = fopen(path,"a+t");
    
    Gato gato;

    fscanf(arch,"%s %s %c %s",gato.nombre,gato.raza, &gato.sexo ,gato.castracion);
    while(!feof(arch))
    {       
        insertarEnListaOrd(pl,&gato,sizeof(Gato),cmp_nombre);
        fscanf(arch,"%s %s %c %s",gato.nombre,gato.raza, &gato.sexo ,gato.castracion);
    }

    fclose(arch);
}


void cargarArchivo(Lista* pl, char* path)
{
    FILE *arch = fopen(path,"wt");
    
    Gato gato;

    while(listaVacia(pl)!=0)
    {       
        eliminarDeListaUltimo(pl,&gato,sizeof(Gato));
        fprintf(arch,"%s %s %c %s\n",gato.nombre,gato.raza,gato.sexo,gato.castracion);
    }

    fclose(arch);
}


///LISTA


void crearLista(Lista* pl)
{
    *pl=NULL;
}

int insertarEnListaOrd(Lista *pl, const void* dato, size_t tamElem, Cmp cmp)
{
    while(*pl && cmp((*pl)->dato,dato)>0)
        pl= &(*pl)->sig;

    if(*pl && cmp((*pl)->dato,dato)==0)
        return DUPLICADO;

    Nodo* nue = (Nodo*)malloc(sizeof(Nodo));
    void* datoN= malloc(tamElem);

    if(!nue || !datoN)
    {
        free(nue);
        free(datoN);
        return SIN_MEM;
    }

    nue->dato=datoN;
    nue->TamElem=tamElem;
    memcpy(datoN,dato,tamElem);

    nue->sig= *pl;
    *pl=nue;

    return TODO_OK;

}

int buscarEnListaDesord(const Lista* pl, void* dato, size_t tamElem, Cmp cmp)
{
    while(*pl && cmp(dato, (*pl)->dato)!=0)
        pl = &(*pl)->sig;

    if(!*pl)
        return 0;

    Nodo* elem = *pl;

    memcpy(dato,elem->dato,min(elem->TamElem,tamElem));
    return TODO_OK;
}

int eliminarDeListaOrdPorValor(Lista* pl, void* dato, size_t tamElem, Cmp cmp)
{
    while(*pl && cmp((*pl)->dato,dato)!=0)
        pl= &(*pl)->sig;

    if(!*pl || cmp((*pl)->dato,dato)!=0)
        return 0;

    Nodo* nae=*pl;
    *pl = nae->sig;

    memcpy(dato,nae->dato,min(nae->TamElem,tamElem));

    free(nae->dato);
    free(nae);

    return TODO_OK;
}

int eliminarDeListaUltimo(Lista* pl, void* dato, size_t tamElem)
{
    if(!*pl)
        return 0;

    while((*pl)->sig)
        pl= &(*pl)->sig;

    Nodo* nae = *pl;
    *pl = nae->sig;

    memcpy(dato, nae->dato,min(tamElem,nae->TamElem));
    free(nae->dato);
    free(nae);

    return 1;
}

int listaVacia(const Lista* pl)
{
    if(!*pl)
        return 0;
    
    return TODO_OK;
}

void vaciarLista(Lista* pl)
{
    Nodo* nae;

    while(*pl)
    {
        nae = *pl;
        *pl = nae->sig;
        free(nae->dato);
        free(nae);
    }
}

int cmp_nombre(const void* v1, const void* v2){
	Gato *p1 = (Gato*) v1;
	Gato *p2 = (Gato*) v2;

	return strcmp(p1->nombre,p2->nombre);
}

int recorrerLista(const Lista* pl, void* dato, size_t tamElem, int i)
{
    int contador = 0;
    while(*pl && contador < i)
    {
        pl = &(*pl)->sig;
        contador++;
    }

    if(!*pl)
        return 0;

    Nodo* elem = *pl;

    memcpy(dato,elem->dato,min(elem->TamElem,tamElem));

    return TODO_OK;
}

void mostrarAyuda()
{
	puts("-------------- AYUDA --------------");
	puts(" \n  Este programa desea llevar un registro minimo de los rescates y adopciones de gatos del refugio.\n "
		"Para ello se crea un servidor y un cliente que se comunican a traves de memoria compartida." );
	puts("Comandos para el Servidor:");
	puts("- 1. ./servidorComp ");
    puts("Para terminarlo");
	puts("kill -SIGUSR1 Pid_Demonio");

}
