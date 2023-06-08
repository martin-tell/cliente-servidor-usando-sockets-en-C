#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdio_ext.h>

#define PUERTO 8080
#define MAX 1000

void menu(char*);
void submenu(char*);
void elige_platillo( char*, char[]);

char entrada[] = "Entrada";
char    sopa[] = "Sopa";
char  fuerte[] = "Plato Fuerte";
char  bebida[] = "Bebida";
char  postre[] = "Postre";

int main( int argc, char *argv[]) {

  int socket_cliente, tamano, numbytes;
  char buffer[MAX], respuesta[MAX];
  char opcion[MAX], buffer_1[MAX], respuesta_1[MAX], respuesta_platillo[MAX];
  char opcion_menu[MAX], opcion_platillo[MAX], buffer_platillo[MAX];
  struct sockaddr_in socket_servidor;

  socket_servidor.sin_family = AF_INET;
  socket_servidor.sin_port = htons(PUERTO);
  socket_servidor.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset( &(socket_servidor.sin_zero), '\0', 8);

  if((socket_cliente = socket( AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(-1);
  }

  tamano = sizeof( struct sockaddr);

  if(connect( socket_cliente, (struct sockaddr *)&socket_servidor, tamano) == -1) {
    perror("connect");
    exit(-1);
  }

  if((numbytes = recv( socket_cliente, buffer, MAX-1, 0)) == -1) {
    perror("recv");
    exit(-1);
  }

  buffer[numbytes] = '\0';
  printf( "%s", buffer);

  while(1) {

    menu(opcion);
    strcpy(buffer,opcion);

    if( send(socket_cliente, buffer, MAX-1, 0) == -1) {
      perror("send");
      exit(-1);
    }

    if( (numbytes = recv(socket_cliente, respuesta, MAX-1, 0)) == -1) {
      perror("recv");
      exit(-1);
    }

    respuesta[numbytes] = '\0';

    switch(atoi(opcion)) {

      case 1:
      case 2:
      case 3:

        printf("%s",respuesta);
        int bandera = 1;

        if( strcmp( respuesta, "\n\t¡Lo Sentimos! La Comida se nos ha Agotado\n") != 0 ){

	  while(bandera){

	    submenu(opcion_menu);
            strcpy(buffer_1,opcion_menu);

            if( send(socket_cliente, buffer_1, MAX-1, 0) == -1) {
              perror("send");
              exit(-1);
            }

            if( (numbytes = recv(socket_cliente, respuesta_1, MAX-1, 0)) == -1) {
              perror("recv");
              exit(-1);
            }

	    respuesta_1[numbytes] = '\0';

            switch(atoi(opcion_menu)) {

              case 1:
              case 2:
              case 3:
              case 4:
              case 5:

	        printf("%s", respuesta_1);
////////////////////////////////////////////////////  Bucle de eleccion de entrada

	        int bandera_1 = 1;

                while(bandera_1) {

                  elige_platillo( opcion_platillo, entrada);
		  strcpy( buffer_platillo, opcion_platillo);

 		  if( send(socket_cliente, buffer_platillo, MAX-1, 0) == -1) {
            	    perror("send");
            	    exit(-1);
          	  }

          	  if( (numbytes = recv(socket_cliente, respuesta_platillo, MAX-1, 0)) == -1) {
            	    perror("recv");
            	    exit(-1);
          	  }

	  	  respuesta_platillo[numbytes] = '\0';

		  switch(atoi(opcion_platillo)) {

		    case 1:
                    case 2:
                    case 3:
		      printf("%s", respuesta_platillo);
		      //////////////////////////////////////////////////////// Bucle eleccion sopa
		      int bandera_2 = 1;

                      while(bandera_2) {
                        elige_platillo( opcion_platillo, sopa);
		        strcpy( buffer_platillo, opcion_platillo);

 		        if( send(socket_cliente, buffer_platillo, MAX-1, 0) == -1) {
            	          perror("send");
            	          exit(-1);
          	        }

          	        if( (numbytes = recv(socket_cliente, respuesta_platillo, MAX-1, 0)) == -1) {
            	          perror("recv");
            	          exit(-1);
          	        }

	  	        respuesta_platillo[numbytes] = '\0';

		        switch(atoi(opcion_platillo)) {

		          case 1:
                          case 2:
                          case 3:
		            printf("%s", respuesta_platillo);
                            ////////////////////////////////////////////////////////////////////////// Bucle eleccion Plato Fuerte 
                            int bandera_3 = 1;

                            while(bandera_3) {
                              elige_platillo( opcion_platillo, fuerte);
		              strcpy( buffer_platillo, opcion_platillo);

 		              if( send(socket_cliente, buffer_platillo, MAX-1, 0) == -1) {
            	                perror("send");
            	                exit(-1);
          	              }

          	              if( (numbytes = recv(socket_cliente, respuesta_platillo, MAX-1, 0)) == -1) {
            	                perror("recv");
            	                exit(-1);
          	              }

	  	              respuesta_platillo[numbytes] = '\0';

		              switch(atoi(opcion_platillo)) {

		                case 1:
                                case 2:
                                case 3:
		          	  printf("%s", respuesta_platillo);
                          	  ///////////////////////////////////////////////////////////////////// Bucle eleccion Bebida
                          	  int bandera_4 = 1;

                    		  while(bandera_4) {
                      		    elige_platillo( opcion_platillo, bebida);
		      		    strcpy( buffer_platillo, opcion_platillo);

 		      		    if( send(socket_cliente, buffer_platillo, MAX-1, 0) == -1) {
            	        	      perror("send");
            	        	      exit(-1);
          	      		    }

          	      		    if( (numbytes = recv(socket_cliente, respuesta_platillo, MAX-1, 0)) == -1) {
            	        	      perror("recv");
            	        	      exit(-1);
          	      		    }

	  	      		    respuesta_platillo[numbytes] = '\0';

		      		    switch(atoi(opcion_platillo)) {

		        	      case 1:
                        	      case 2:
                        	      case 3:
		          	        printf("%s", respuesta_platillo);
                          	        //////////////////////////////////////////////////////////////// Bucle eleccion postre
                          	        int bandera_5 = 1;

                    		        while(bandera_5) {
                      			  elige_platillo( opcion_platillo, postre);
		      			  strcpy( buffer_platillo, opcion_platillo);

 		      			  if( send(socket_cliente, buffer_platillo, MAX-1, 0) == -1) {
            	        		    perror("send");
            	        		    exit(-1);
          	      			  }

          	      			  if( (numbytes = recv(socket_cliente, respuesta_platillo, MAX-1, 0)) == -1) {
            	        		    perror("recv");
            	        		    exit(-1);
          	      			  }

	  	      			  respuesta_platillo[numbytes] = '\0';

		      			  switch(atoi(opcion_platillo)) {

		        		    case 1:
                        		    case 2:
                        		    case 3:
		          		      printf("%s", respuesta_platillo);
	                  		      bandera = 0;
                          		      bandera_1 = 0;
                          		      bandera_2 = 0;
                          		      bandera_3 = 0;
                          		      bandera_4 = 0;
                          		      bandera_5 = 0;
		        		    break;

		        		    case 4:
		          		      printf("%s", respuesta_platillo);
		          		      bandera_5 = 0;
		        		    break;

		        		    default:
		          		      printf("%s", respuesta_platillo);
		        		    break;

 		      		          }

	            		        }
                          	        //////////////////////////////////////////////////////////////// Fin Bucle eleccion postre
		        	      break;

		        	      case 4:
		          	        printf("%s", respuesta_platillo);
		          	        bandera_4 = 0;
		        	      break;

		        	      default:
		          	        printf("%s", respuesta_platillo);
		        	      break;

 		                    }

	            		  }
                          	  ///////////////////////////////////////////////////////////////////// Fin Bucle eleccion Bebida
		                break;

		                case 4:
		          	  printf("%s", respuesta_platillo);
		          	  bandera_3 = 0;
		                break;

		                default:
		          	  printf("%s", respuesta_platillo);
		                break;

 		      	      }

	                    }
                            ////////////////////////////////////////////////////////////////////////// Fin Bucle Eleccion Plato Fuerte
		          break;

		          case 4:
		            printf("%s", respuesta_platillo);
		            bandera_2 = 0;
		          break;

		          default:
		            printf("%s", respuesta_platillo);
		          break;

 		        }

	              }
		      //////////////////////////////////////////////////////// Fin Bucle eleccion sopa 
		    break;

		    case 4:
		      printf("%s", respuesta_platillo);
		      bandera_1 = 0;
		    break;

		    default:
		      printf("%s", respuesta_platillo);
		    break;

 		  }

	        }
///////////////////////////////////////////////////// Fin del Bucle de eleccion de entrada
              break;

              case 6:

	        printf("%s", respuesta_1);
                bandera = 0;

              break;

              default:
	        printf("%s", respuesta_1);
              break;

          }

        }

      }

      break;

      case 4:
        printf("%s",respuesta);
        exit(0);
      break;

      default:
        printf("%s",respuesta);
      break;
    }

  }
  close(socket_cliente);
  return 0;

}

void menu(char *c) {
  puts("\n\tSeleccione el Número del Restaurante al que Desea Asistir");
  puts("\n\t1) El Parrillaje");
  puts("\t2) La Estancia Argentina");
  puts("\t3) La Cueva del Zorro");
  puts("\t4) Salir");
  printf("\n\tEleccion -> ");
  scanf("%s",c);
  __fpurge(stdin);
}

void submenu(char *c){
  puts("\n\tSeleccione uno de los 5 Menús Disponibles\n");
  puts("\t\u2022 Menú     1)");
  puts("\t\u2022 Menú     2)");
  puts("\t\u2022 Menú     3)");
  puts("\t\u2022 Menú     4)");
  puts("\t\u2022 Menú     5)");
  puts("\t  Cancelar 6)");
  printf("\n\tElección -> ");
  scanf("%s",c);
  __fpurge(stdin);
}

void elige_platillo( char *c, char platillo[]) {
  printf("\n\n\t***Seleccione su %s***\n\n",platillo);
  puts("\t   Pulse 4) para regresar\n");
  printf("\t   Eleccion -> ");
  scanf("%s",c);
  __fpurge(stdin);
}







