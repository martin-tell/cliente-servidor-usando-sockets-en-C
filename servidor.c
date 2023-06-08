#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <signal.h>

#define PUERTO 8080
#define MAX 1000
#define TAM sizeof(int*)

//Varibles para el uso de los menús de restaurantes
char menu_1 [5][5][3][2][30] = {
    { { {{"Pulpo al Guajillo"},{"25"}},{{"Chistorra a la Parrilla"},{"47"}},{{"Mollejas"},{"52"}}},{{{"Sopa de Pulpo"},{"20"}},{{"Sopa de Mejillon"},{"21"}},{{"Sopa de Almeja"},{"45"}}},{{{"Porter House"},{"28"}},{{"Bife de Lomo"},{"14"}},{{"Ribe Eye"},{"82"}}},{{{"Gin Chai"},{"40"}},{{"Paradise"},{"47"}},{{"Jugo de Piña"},{"12"}}},{{{"Tartin de Chocolate"},{"35"}},{{"Tarta el Parrillaje"},{"47"}},{{"CheseeCake"},{"11"}} } },
    { { {{"Pulpo a la Morita"},{"30"}},{{"Carpacho de Pulpo"},{"41"}},{{"Carpacho de Atún"},{"87"}}},{{{"Sopa de Camaron"},{"15"}},{{"Sopa de Chile Poblano"},{"47"}},{{"Sopa de Cebolla"},{"46"}}},{{{"Bife de Chorizo"},{"32"}},{{"Picaña"},{"12"}},{{"Chuleton"},{"23"}}},{{{"Gin de Pepino"},{"47"}},{{"Jugo de Naranja"},{"74"}},{{"Jugo de Fresa"},{"41"}}},{{{"Helado de Turron"},{"45"}},{{"Pay de Limon"},{"45"}},{{"Dulce de Leche"},{"74"}} } },
    { { {{"Sopa de Pescado"},{"20"}},{{"Sopa de Elote"}, {"41"}}, {{"Sopa de Champiñon"},{"82"}}},{{{"Pulpo a la Pasilla"},{"25"}},{{"Carpacho de Lomo"},{"78"}},{{"Chorizo Argentino"},{"84"}}},{{{"Arrachera a la parrilla"},{"32"}},{{"TomaHawk"},{"12"}},{{"Pollo Asado"},{"23"}}},{{{"Gin de Lichi"},{"47"}},{{"Jugo de Toronja"},{"74"}},{{"Coca Cola"},{"41"}}},{{{"Tarta de Manzana"},{"45"}},{{"Pastel de Helado"},{"45"}},{{"Bombones Glaseados"},{"74"}} } },
    { { {{"Camariones al Ajillo"}, {"15"}},{{"Pimiento Relleno"}, {"47"}},{{"Plato de Vegetales"},{"23"}}},{{{"Sopa de Queso"},{"40"}},{{"Espaguetti"},{"14"}},{{"Tortelini"},{"14"}}},{{{"Asado de Tira"},{"38"}},{{"Tapa de Vacio"},{"47"}},{{"Cabreria"},{"74"}}},{{{"Limonada"},{"47"}},{{"Mangada"},{"36"}},{{"Refresco de Piña"},{"41"}}},{{{"Tarta de Nuez"},{"41"}},{{"Tarta de Canela"},{"47"}},{{"Helado de Vainilla"},{"45"}} } },
    { { {{"Champiñones al ajillo"},{"75"}},{{"Queso Panela"},{"65"}},{{"Dados de Atún"},{"95"}}},{{{"Sopa de Jitomate"},{"55"}},{{"Espaguetti con pulpo"},{"37"}},{{"Espaguetti con Camaron"},{"41"}}},{{{"Fajitas"},{"60"}},{{"Wagyu"},{"45"}},{{"T-Bone"},{"82"}}},{{{"King Konga"},{"47"}},{{"Mezcalian de limon"},{"55"}},{{"Jugo de Uva"},{"66"}}},{{{"Pastel de Frambuesa"},{"74"}},{{"Pastel de Zarzamora"},{"12"}},{{"Pastel de Cafe"},{"78"}} } },
};


char tmp[5][5][3][2][30];

typedef struct p{
  char platillo[5][2][30];
  int total;
}pedido;

pedido cuenta[10];

// Variables que controlan la cantidad de menus comprados (se usa Memoria Compartida)
int limite, shm_id, *ptr_shm;

char buffer[MAX], respuesta[MAX];
char buffer_1[MAX], respuesta_1[MAX], buffer_platillo[MAX], respuesta_platillo[MAX];
char menu[MAX], ticket_cuenta[MAX];
int ronda=-1, indice_menu, precio_total;

//Prototipos de funcion
void elige_restaurante();
void elige_menu();
void elige_platillo();
void asigna_menu(char [5][5][3][2][30], char [5][5][3][2][30]);
void get_menu(int);
void get_cuenta();
void get_total();
//void manejador(int);

//Varaiblles para uso de sockets
int socket_servidor, socket_cliente, tamano, num_bytes;
struct sockaddr_in direccion_servidor;

//Funcion principal
int main( int argc, char *argv[] ) {

  //signal(SIGUSR1, manejador);

  direccion_servidor.sin_family = AF_INET;
  direccion_servidor.sin_port = htons( PUERTO );
  direccion_servidor.sin_addr.s_addr = INADDR_ANY;
  memset( &(direccion_servidor.sin_zero), '\0', 8);

  if ( ( socket_servidor = socket( AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(-1);
  }

  tamano = sizeof( struct sockaddr);

  if ( bind( socket_servidor, (struct sockaddr *)&direccion_servidor, tamano) == -1) {
    perror("bind");
    exit(-1);
  }

  if ( listen( socket_servidor, 5) == -1) {
    perror("listen");
    exit(-1);
  }

  if ((shm_id = shmget( IPC_PRIVATE, TAM, 0666|IPC_CREAT)) == -1) {
    perror("shmget");
    exit(-1);
  }

  char *saludo = "\t\u203C BIENVENIDO AL SERVIDOR DE RESTAURANTES \u203C\n";

  while(1) {// bucle infinito para tratar conexiones

    if( ( socket_cliente = accept( socket_servidor,(struct sockaddr *)&direccion_servidor, &tamano)) == -1) {
      perror("accept");
      continue;
    }

    puts("Servidor: Hay un Cliente Conectado\n");

    if( !fork()) { // hijo

        close( socket_servidor); // El proceso hijo no lo necesita
        
        if ((ptr_shm = (int*) shmat( shm_id, NULL, 0)) == (void*)-1){
          perror("shmat");
          exit(-1);
        }

        if( send( socket_cliente, saludo, strlen(saludo), 0) == -1)
	  perror("send");

        int bandera = 1;

	while(bandera) {

	  puts("Servidor: Escuchando al Cliente...\n");

          if( ( num_bytes = recv( socket_cliente, buffer, MAX-1, 0)) == -1){
	    perror("recv");
            exit(1);
          }

          buffer[num_bytes] = '\0';
	  printf("Servidor: El Cliente ha Seleccionado la Petición %s...\n\n", buffer);
	  elige_restaurante();

          if(strcmp(respuesta,"salir") == 0){
            puts("Servidor: El Cliente se ha Desconectado\n");
	    strcpy(respuesta,"\n\t¡Gracias por Usar Nuestro Servidor!\n");

            if( send( socket_cliente, respuesta, MAX-1, 0) == -1)
	      perror("send");
            strcpy(respuesta,"");
            strcpy(buffer,"");
            
            shmdt(&ptr_shm);//Desasociacion
            
            bandera = 0;
            break;
          }

	  if( strcmp(respuesta, "inv") == 0){
            puts("Servidor: El Cliente ha Solicitado una Petición Inválida\n");
	    strcpy( respuesta, "\n\tOpcion Inválida\n");
	    if( send( socket_cliente, respuesta, MAX-1, 0) == -1)
	      perror("send");
            strcpy( respuesta, "");
            strcpy( buffer, "");
            continue;
          }

          if( strcmp(respuesta, "lim") == 0){
            puts("Servidor: No es Posible Vender Más Menús, La Comida se ha Agotado\n");
            strcpy( respuesta, "\n\t¡Lo Sentimos! La Comida se nos ha Agotado\n");
	    if( send( socket_cliente, respuesta, MAX-1, 0) == -1)
	      perror("send");
            strcpy( respuesta, "");
            strcpy( buffer, "");
            continue;
          }

	  if( send( socket_cliente, respuesta, strlen(respuesta), 0) == -1)
	    perror("send");
          strcpy(respuesta,"");
          strcpy(buffer,"");
	  ////////////////////////////////////////////////////////////////////// Bucle de eleccion de menu

	  int bandera_1 = 1;

	  while(bandera_1) {

            puts("Servidor: El Cliente Elegirá un Menú\n");

	    if( ( num_bytes = recv( socket_cliente, buffer_1, MAX-1, 0)) == -1) {
	      perror("recv");
              exit(1);
            }

            buffer_1[num_bytes] = '\0';
            printf("Servidor: El Cliente ha Seleccionado la Peticion #%s\n\n",buffer_1);
	    elige_menu();

            if(strcmp(respuesta_1,"cancel") == 0) {

              puts("Servidor: El Cliente ha Cancelado su Petición al Restaurante\n");
	      strcpy(respuesta_1,"\n\t¡Esperamos Pronto ser su Próxima Elección!\n");
              bandera_1 = 0;
              if( send( socket_cliente, respuesta_1, MAX-1, 0) == -1)
	        perror("send");
              strcpy(respuesta_1,"");
              strcpy(buffer_1,"");
              continue;

            }

            if(strcmp(respuesta_1,"inv") == 0) {

              puts("Servidor: El Cliente ha solicitado una Petición Inválida\n");
	      strcpy(respuesta_1,"\n\tOpcion Inválida\n");
              if( send( socket_cliente, respuesta_1, MAX-1, 0) == -1)
	        perror("send");
              strcpy(respuesta_1,"");
              strcpy(buffer_1,"");
              continue;
	    }

	    if( send( socket_cliente, respuesta_1, MAX-1, 0) == -1)
	      perror("send");
            strcpy(respuesta_1,"");
            strcpy(buffer_1,"");
///////////////////////////////////////////////////////////////////////////////// Bucle de eleccion de la entrada
            int bandera_2 = 1;

	    while(bandera_2) {

	      puts("Servidor: El cliente Elegirá una Entrada\n");

	      if( ( num_bytes = recv( socket_cliente, buffer_platillo, MAX-1, 0)) == -1) {
	        perror("recv");
                exit(1);
              }

              buffer_platillo[num_bytes] = '\0';
              printf("Servidor: El Cliente ha Seleccionado la Peticion #%s\n\n",buffer_platillo);
	      elige_platillo();

	      if(strcmp(respuesta_platillo,"regre") == 0) {

                puts("Servidor: El Cliente ha Decuelto el Platillo\n");
	        strcpy(respuesta_platillo,"\n\t¡El platillo ha sido devuelto!\n");
                bandera_2 = 0;
                if( send( socket_cliente, respuesta_platillo, MAX-1, 0) == -1)
	          perror("send");
                strcpy(respuesta_platillo,"");
                strcpy(buffer_platillo,"");
                continue;

              }

              if(strcmp(respuesta_platillo,"inv") == 0) {

                puts("Servidor: El Cliente ha solicitado una Petición Inválida\n");
	        strcpy(respuesta_platillo,"\n\tOpcion Inválida\n");
		if( send( socket_cliente, respuesta_platillo, MAX-1, 0) == -1)
	          perror("send");
                strcpy(respuesta_platillo,"");
                strcpy(buffer_platillo,"");
                continue;
	      }

	      if( send( socket_cliente, respuesta_platillo, MAX-1, 0) == -1)
	        perror("send");
              strcpy(respuesta_platillo,"");
              strcpy(buffer_platillo,"");

///////////////////////////////////////////////////////////////////////////////// Bucle eleccion de Sopa
	      int bandera_3 = 1;

	      while(bandera_3) {

	        puts("Servidor: El cliente Elegirá la Sopa\n");

	        if( ( num_bytes = recv( socket_cliente, buffer_platillo, MAX-1, 0)) == -1) {
	          perror("recv");
                  exit(1);
                }

                buffer_platillo[num_bytes] = '\0';
                printf("Servidor: El Cliente ha Seleccionado la Peticion #%s\n\n",buffer_platillo);
	        elige_platillo();

	        if(strcmp(respuesta_platillo,"regre") == 0) {

                  puts("Servidor: El Cliente ha Decuelto el Platillo\n");
	          strcpy(respuesta_platillo,"\n\t¡El platillo ha sido devuelto!\n");
                  bandera_3 = 0;
                  if( send( socket_cliente, respuesta_platillo, MAX-1, 0) == -1)
	            perror("send");
                  strcpy(respuesta_platillo,"");
                  strcpy(buffer_platillo,"");
                  continue;

                }

                if(strcmp(respuesta_platillo,"inv") == 0) {

                  puts("Servidor: El Cliente ha solicitado una Petición Inválida\n");
	          strcpy(respuesta_platillo,"\n\tOpcion Inválida\n");
		  if( send( socket_cliente, respuesta_platillo, MAX-1, 0) == -1)
	            perror("send");
                  strcpy(respuesta_platillo,"");
                  strcpy(buffer_platillo,"");
                  continue;
	        }

	        if( send( socket_cliente, respuesta_platillo, MAX-1, 0) == -1)
	          perror("send");
                strcpy(respuesta_platillo,"");
                strcpy(buffer_platillo,"");

///////////////////////////////////////////////////////////////////////////////// Bucle eleccion Plato Fuerte
		int bandera_4 = 1;

	        while(bandera_4) {

	          puts("Servidor: El cliente Elegirá el Plato Fuerte\n");

	          if( ( num_bytes = recv( socket_cliente, buffer_platillo, MAX-1, 0)) == -1) {
	            perror("recv");
                    exit(1);
                  }

                  buffer_platillo[num_bytes] = '\0';
                  printf("Servidor: El Cliente ha Seleccionado la Peticion #%s\n\n",buffer_platillo);
	          elige_platillo();

	          if(strcmp(respuesta_platillo,"regre") == 0) {

                    puts("Servidor: El Cliente ha Decuelto el Platillo\n");
	            strcpy(respuesta_platillo,"\n\t¡El platillo ha sido devuelto!\n");
                    bandera_4 = 0;
                    if( send( socket_cliente, respuesta_platillo, MAX-1, 0) == -1)
	              perror("send");
                    strcpy(respuesta_platillo,"");
                    strcpy(buffer_platillo,"");
                    continue;

                  }

                  if(strcmp(respuesta_platillo,"inv") == 0) {

                    puts("Servidor: El Cliente ha solicitado una Petición Inválida\n");
	            strcpy(respuesta_platillo,"\n\tOpcion Inválida\n");
		    if( send( socket_cliente, respuesta_platillo, MAX-1, 0) == -1)
	              perror("send");
                    strcpy(respuesta_platillo,"");
                    strcpy(buffer_platillo,"");
                    continue;
	          }

	          if( send( socket_cliente, respuesta_platillo, MAX-1, 0) == -1)
	            perror("send");
                  strcpy(respuesta_platillo,"");
                  strcpy(buffer_platillo,"");

///////////////////////////////////////////////////////////////////////////////// Bucle eleccion Bebida
		  int bandera_5 = 1;

	          while(bandera_5) {

	            puts("Servidor: El cliente Elegirá la Bebida\n");

	            if( ( num_bytes = recv( socket_cliente, buffer_platillo, MAX-1, 0)) == -1) {
	              perror("recv");
                      exit(1);
                    }

                    buffer_platillo[num_bytes] = '\0';
                    printf("Servidor: El Cliente ha Seleccionado la Peticion #%s\n\n",buffer_platillo);
	            elige_platillo();

	            if(strcmp(respuesta_platillo,"regre") == 0) {

                      puts("Servidor: El Cliente ha Decuelto el Platillo\n");
	              strcpy(respuesta_platillo,"\n\t¡El platillo ha sido devuelto!\n");
                      bandera_5 = 0;
                      if( send( socket_cliente, respuesta_platillo, MAX-1, 0) == -1)
	                perror("send");
                      strcpy(respuesta_platillo,"");
                      strcpy(buffer_platillo,"");
                      continue;

                    }

                    if(strcmp(respuesta_platillo,"inv") == 0) {

                      puts("Servidor: El Cliente ha solicitado una Petición Inválida\n");
	              strcpy(respuesta_platillo,"\n\tOpcion Inválida\n");
		      if( send( socket_cliente, respuesta_platillo, MAX-1, 0) == -1)
	                perror("send");
                      strcpy(respuesta_platillo,"");
                      strcpy(buffer_platillo,"");
                      continue;
	            }

	            if( send( socket_cliente, respuesta_platillo, MAX-1, 0) == -1)
	              perror("send");
                    strcpy(respuesta_platillo,"");
                    strcpy(buffer_platillo,"");

///////////////////////////////////////////////////////////////////////////////// Bucle eleccion Postre
		    int bandera_6 = 1;

	            while(bandera_6) {

	              puts("Servidor: El cliente Elegirá el Postre\n");

	              if( ( num_bytes = recv( socket_cliente, buffer_platillo, MAX-1, 0)) == -1) {
	                perror("recv");
                        exit(1);
                      }

                      buffer_platillo[num_bytes] = '\0';
                      printf("Servidor: El Cliente ha Seleccionado la Peticion #%s\n\n",buffer_platillo);
	              elige_platillo();

	              if(strcmp(respuesta_platillo,"regre") == 0) {

                        puts("Servidor: El Cliente ha Decuelto el Platillo\n");
	                strcpy(respuesta_platillo,"\n\t¡El platillo ha sido devuelto!\n");
                        bandera_6 = 0;
                        if( send( socket_cliente, respuesta_platillo, MAX-1, 0) == -1)
	                  perror("send");
                        strcpy(respuesta_platillo,"");
                        strcpy(buffer_platillo,"");
                        continue;

                      }

                      if(strcmp(respuesta_platillo,"inv") == 0) {

                        puts("Servidor: El Cliente ha solicitado una Petición Inválida\n");
	                strcpy(respuesta_platillo,"\n\tOpcion Inválida\n");
		        if( send( socket_cliente, respuesta_platillo, MAX-1, 0) == -1)
	                  perror("send");
                        strcpy(respuesta_platillo,"");
                        strcpy(buffer_platillo,"");
                        continue;
	              }

		      limite = *ptr_shm;
                      limite++;
                      *ptr_shm = limite;
		      ronda = -1;
                      precio_total = 0;
	              bandera_1 = 0;
	              bandera_2 = 0;
		      bandera_3 = 0;
		      bandera_4 = 0;
		      bandera_5 = 0;
		      bandera_6 = 0;
		      printf("Servidor: Se han vendido %i Menús\n\n",*ptr_shm);

	              if( send( socket_cliente, respuesta_platillo, MAX-1, 0) == -1)
	                perror("send");
                      strcpy(respuesta_platillo,"");
                      strcpy(buffer_platillo,"");

                    }
///////////////////////////////////////////////////////////////////////////////// Fin Bucle eleccion Postre

                  }
///////////////////////////////////////////////////////////////////////////////// Fin Bucle eleccion Bebida

                }
///////////////////////////////////////////////////////////////////////////////// Fin Bucle eleccion Plato Fuerte

              }
///////////////////////////////////////////////////////////////////////////////// Fin Bucle eleccion de Sopa

            }
///////////////////////////////////////////////////////////////////////////////// Fin Bucle del bucle de eleccion de la entrada

          }
///////////////////////////////////////////////////////////////////////////////// Fin del bucle de elccion de menu

        }

        close( socket_cliente);
	exit(0);

      }

      close( socket_cliente); // El proceso padre no lo necesita

    }

  return 0;

}


void asigna_menu(char tmp[5][5][3][2][30], char original[5][5][3][2][30]) {
  memcpy(tmp, original, sizeof(char)*5*5*3*2*30);
}

void get_menu(int n) {
  int longitud_nombre, longitud_precio;
  char puntos[30] = "..............................";
  char subcadena[30];
  memset(menu, '\0', 30);
  strcat(menu,"\n\tEntradas                      \t\tSopas                       \t\tPlatos Fuertes\n");
  for(int i=0; i<3; i++) {
    strcat( menu, "\n");
    for(int j=0; j<3; j++) {
      longitud_nombre = strlen(tmp[n][j][i][0]);
      longitud_precio = strlen(tmp[n][j][i][1]);
      strcat( menu, "\t");
      strncpy( subcadena, &puntos[longitud_nombre], 30-longitud_precio);
      strcat( menu, tmp[n][j][i][0]);
      strcat( menu, subcadena);
      strcat( menu, tmp[n][j][i][1]);
      memset( subcadena, '\0', 30);
    }
  }
  strcat(menu,"\n\n\tBebidas                      \t\tPostres\n");
  for(int i=0; i<3; i++) {
    strcat( menu, "\n");
    for(int j=3; j<5; j++) {
      longitud_nombre = strlen(tmp[n][j][i][0]);
      longitud_precio = strlen(tmp[n][j][i][1]);
      strcat( menu, "\t");
      strncpy( subcadena, &puntos[longitud_nombre], 30-longitud_precio);
      strcat( menu, tmp[n][j][i][0]);
      strcat( menu, subcadena);
      strcat( menu, tmp[n][j][i][1]);
      memset( subcadena, '\0', 30);
    }
  }
  strcat( menu,"\n");
}

void elige_restaurante() {
  char* retorno;
  switch(atoi(buffer)) {
    case 1:
      if( limite == 10 )
        strcpy( respuesta, "lim");
      else {
        strcpy( respuesta, "\n\tEl Parrillaje\n");
        asigna_menu( tmp, menu_1);
      }
    break;
    case 2:
      if( limite == 10)
        strcpy( respuesta, "lim");
      else {
        strcpy( respuesta, "\n\tLa Estancia Argentina\n");
      }
    break;
    case 3:
      if( limite == 10)
        strcpy( respuesta, "lim");
      else {
        strcpy( respuesta, "\n\tLa Cueva del Zorro\n");
      }
    break;
    case 4:
      strcpy( respuesta, "salir");
    break;
    default:
      strcpy( respuesta, "inv");
    break;
  }

}


void elige_menu() {

  switch(atoi(buffer_1)){
    case 1:
      strcpy( respuesta_1, "\n\t\t\t\t\t\t\tMenú 1\n");
      get_menu(0);
      strcat( respuesta_1, menu);
      indice_menu = 0;
    break;
    case 2:
      strcpy( respuesta_1, "\n\t\t\t\t\t\t\tMenú 2\n");
      get_menu(1);
      strcat( respuesta_1, menu);
      indice_menu = 1;
    break;
    case 3:
      strcpy( respuesta_1, "\n\t\t\t\t\t\t\tMenú 3\n");
      get_menu(2);
      strcat( respuesta_1, menu);
      indice_menu = 2;
    break;
    case 4:
      strcpy( respuesta_1, "\n\t\t\t\t\t\t\tMenú 4\n");
      get_menu(3);
      strcat( respuesta_1, menu);
      indice_menu = 3;
    break;
    case 5:
      strcpy( respuesta_1, "\n\t\t\t\t\t\t\tMenú 5\n");
      get_menu(4);
      strcat( respuesta_1, menu);
      indice_menu = 4;
    break;
    case 6:
      strcpy( respuesta_1, "cancel");
    break;
    default:
      strcpy( respuesta_1, "inv");
    break;
  }
}

void elige_platillo() {

  switch(atoi(buffer_platillo)) {
    case 1:
      ronda++;
      strcpy( respuesta_platillo, "\n\tPlatillo Elegido: ");
      strcat( respuesta_platillo, tmp[indice_menu][ronda][0][0]);
      strcat( respuesta_platillo, " con Precio: ");
      strcat( respuesta_platillo, tmp[indice_menu][ronda][0][1]);
      strcpy( cuenta[limite].platillo[ronda][0], tmp[indice_menu][ronda][0][0]);
      strcpy( cuenta[limite].platillo[ronda][1], tmp[indice_menu][ronda][0][1]);

      if( ronda == 4) {

        get_total();
        get_cuenta();
	strcat( respuesta_platillo, ticket_cuenta);
      }

    break;
    case 2:
    break;
    case 3:
    break;
    case 4:
      strcpy( cuenta[limite].platillo[ronda][0], "");
      strcpy( cuenta[limite].platillo[ronda][1], "");
      ronda--;
      strcpy( respuesta_platillo, "regre");
    break;
    default:
      strcpy( respuesta_platillo, "inv");
    break;
  }

}

void get_cuenta() {

  int longitud_nombre, longitud_precio;
  char puntos[30] = ".............................";
  char subcadena[30], total[4];

  memset( ticket_cuenta, '\0', 30);
  strcat( ticket_cuenta,"\n\n\t----------- CUENTA -----------\n");

  for(int i=0; i<5; i++) {

    longitud_nombre = strlen(cuenta[limite].platillo[i][0]);
    longitud_precio = strlen(cuenta[limite].platillo[i][1]);
    strcat( ticket_cuenta, "\n\t");
    strncpy( subcadena, &puntos[longitud_nombre], 30);
    strcat( ticket_cuenta, cuenta[limite].platillo[i][0]);
    strcat( ticket_cuenta, subcadena);
    strcat( ticket_cuenta, cuenta[limite].platillo[i][1]);
    memset( subcadena, '\0', 30);

  }

  strcat( ticket_cuenta, "\n\n\tTotal.......................");
  sprintf( total, "%i", precio_total);
  strcat( ticket_cuenta, total);
  strcat( ticket_cuenta, "\n\n");

}

void get_total(){

  for(int i=0; i<5; i++)
    precio_total += atoi(cuenta[limite].platillo[i][1]);

}
/*
void manejador(int senal){

  signal(SIGUSR1,manejador);
  if(shmctl(shm_id,IPC_RMID,NULL)==-1)  
    perror("shmctl");
  else puts("***Se ha Liberado el Segmento de Memoria Compartido"); 
  
}*/
