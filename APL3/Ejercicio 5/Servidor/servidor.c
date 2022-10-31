/*
-------------------------- ENCABEZADO --------------------------
Nombre del programa: servidor.c
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
#include <signal.h>
#include <netdb.h>		
#include <stdio.h>	
#include <string.h>	
#include <strings.h>	
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <pthread.h>

#define MAX_QUEUE 5
#define SIN_MEM             0
#define TODO_OK             1
#define DUPLICADO           2

int clientes = 0;

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
int cmp_gato(const void* v1, const void* v2);

void crearLista(Lista* pl);
int insertarEnListaOrd(Lista *pl, const void* dato, size_t tamElem, Cmp cmp);
int buscarEnListaDesord(const Lista* pl, void* dato, size_t tamElem, Cmp cmp);
int eliminarDeListaOrdPorValor(Lista* pl, void* dato, size_t tamElem, Cmp cmp);
int eliminarDeListaUltimo(Lista* pl, void* dato, size_t tamElem);
int listaVacia(const Lista* pl);
int recorrerLista(const Lista* pl, void* dato, size_t tamElem, int i);
void vaciarLista(Lista* pl);


Lista lista;

//archivo
void cargarListaArch(Lista* pl, char* path);
void cargarArchivo(Lista* pl, char* path);

// demonio

bool fin = false;
void manejador( int );

//hilos

void* manejador_hilos(void* cliente);
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;


// Ayuda
void mostrarAyuda();

//sockets

int main(int argc, char *argv[])
{
    socklen_t cl=sizeof(struct sockaddr_in);
    struct sockaddr_in sa;
    struct sockaddr_in ca;
 	int server_socket;
	int client_socket;
	int habilitar = 1;

	if(argc>1 && (strcmp("-h", argv[1]) == 0 || strcmp("--help", argv[1]) == 0))
    {
			mostrarAyuda();
            return 0;
    }
    else if(argc!=2)
    {
        puts("----------Error de parametros----------");
        puts("\nPara mostrar la ayuda, ejecute:");
	    puts("- ./servidorComp -h");
	    puts("- ./servidorComp --help");
        return 0;
    }
	signal(SIGINT, SIG_IGN);

    char archivoServidor[100] = "servidor.txt";

    //SI YA HAY UNA INSTANCIA DE SERVIDOR, TERMINA EL PROGRAMA
    if(access(archivoServidor, F_OK) == 0)
    {
        puts("YA HAY UNA INSTANCIA DEL PROCESO SERVIDOR!");
        return 10;
    }

    //SI NO EXISTE EL ARCHIVO, LO CREO
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


      /// HILOS ///

    pthread_t hilo;


      // ---------------------------------- \\ /


		
	//Espera al menos un parámetro: Puerto de escucha
	//argc se establece en 1 si no se ingresan parámetros
	if( argc < 2 ){
		printf("Debe ingresar el puerto de escucha !!!\n");	
		return 1;	
	}
		
	//Crear el socket de escucha del servidor
	if((server_socket=socket(AF_INET,SOCK_STREAM,0)) ==-1){
		printf("No se pudo crear el socket !!!\n");	
		return 1;
	}
    

	//Configurar opciones del socket
	//En este caso que se pueda reutilizar la dirección y puerto 	
	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &habilitar, sizeof(int)) < 0){
   	printf("No se pudo configurar opciones del socket !!!\n");	
		return 1;
	}
	
	//Configuración del servidor
	bzero((char *) &sa, sizeof(struct sockaddr_in));
 	sa.sin_family 		= AF_INET;
 	sa.sin_port 		= htons(atoi(argv[1]));
	sa.sin_addr.s_addr= INADDR_ANY;
 	
	//Vincular socket con configuracion del servidor
	bind(server_socket,(struct sockaddr *)&sa,sizeof(struct sockaddr_in));    
	
	//Marcar el socket como "Socket Pasivo" y Establecer la máxima 
	//cantidad de peticiones que pueden ser encoladas
	listen(server_socket,MAX_QUEUE);
	
	
	Decision des;

	// REFUGIO

    char path[15];
    strcpy(path,"Gatos.txt");
	crearLista(&lista);
    cargarListaArch(&lista,path);
    int devolucion=0;
    //El servidor ser bloquea a la espera de una peticion de conexión
    //desde el cliente (connect) 
    while(!fin)
    {	
        client_socket=accept(server_socket,(struct sockaddr *) &ca,&cl);

        if(clientes<3)
        {
            devolucion=1;
            send(client_socket,&devolucion,sizeof(devolucion),0);
            pthread_create(&hilo,NULL,&manejador_hilos, (void *)&client_socket);
        }
        if(clientes==3)
        {
            devolucion = 0;
            send(client_socket,&devolucion,sizeof(devolucion),0);
            close(client_socket);        
        }

        usleep(100000);
    }

    cargarArchivo(&lista, path );
    pthread_mutex_destroy(&mtx);
 	close(client_socket);
 	close(server_socket);

    //CIERRO Y BORRO ARCHIVO PARA CORROBORAR QUE HAY UNA SOLA INSTANCIA
    fcloes(pf);
    remove(archivoServidor);
    
 	return 0; 
}

//Hilos
void* manejador_hilos(void* cliente){


    int* hilo = (int*) cliente;
    int socketCliente = *hilo;
    clientes++;

    int resultado, j;
    Decision des;
    des.decision=0;

    while(des.decision!=5)
    {
        bzero(&des,sizeof(Decision));
        recv(socketCliente,&des,sizeof(Decision),0);
        pthread_mutex_lock(&mtx); //P(mtx)

        switch (des.decision)
        {
        case 1:

            des.decision = insertarEnListaOrd(&lista,&des.gato,sizeof(Gato),cmp_gato);
            sleep(0.5);
            send(socketCliente,&des,sizeof(Decision),0);
            break;

        case 2:

            des.decision = eliminarDeListaOrdPorValor(&lista,&des.gato,sizeof(Gato),cmp_gato);
            sleep(0.5);
            send(socketCliente,&des,sizeof(Decision),0);
            break;

        case 3:

            des.decision = buscarEnListaDesord(&lista,&des.gato,sizeof(Gato),cmp_gato);
            sleep(0.5);
            send(socketCliente,&des,sizeof(Decision),0);
            break;

        case 4:

            j = 0;
            resultado=1;
            //Lista listaaux = lista;
            while(resultado!=0)
            {
                resultado = recorrerLista(&lista,&des.gato,sizeof(Gato),j);
                sleep(0.5);
                des.decision = resultado;
                send(socketCliente,&des,sizeof(Decision),0);
                j++;
            }
            break;
            
        }
        pthread_mutex_unlock(&mtx); //V(mtx)
        sleep(3);

        usleep(100000);

    }


    pthread_detach(pthread_self());
    clientes--;
    pthread_exit(0);

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
        insertarEnListaOrd(pl,&gato,sizeof(Gato),cmp_gato);
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

int cmp_gato(const void* v1, const void* v2){
	Gato *g1 = (Gato*) v1;
	Gato *g2 = (Gato*) v2;

    return strcasecmp(g1->nombre,g2->nombre);
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
		"Para ello se crea un servidor y clientes que se comunican a traves de Sockets." );
	puts("Comandos para el Servidor:");
	puts("- 1. ./servidorComp \"puerto\"");
    puts("Para terminarlo");
	puts("kill -SIGUSR1 Pid_Demonio");

}
