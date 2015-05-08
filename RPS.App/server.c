/* Estos son los ficheros de cabecera usuales */
#include <stdio.h>          
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 3550 /* El puerto que será abierto */
#define PORT2 3555 /* Puerto Abierto para enviar resultados*/
#define PORT3 3556 /* Puerto Abierto para enviar resultados para el cliente 2*/
#define BACKLOG 2 /* El número de conexiones permitidas */
#define MAXDATASIZE 100 

int ganador(int a, int b)
{
   if(a==b)
   {
      return 0;
   }
   else if(a==1 && b==2)
   {
      return b;
   }
   else if(a==1 && b==3)
   {
      return a;      
   }
   else if(a==2 && b==1)
   {
      return a;
   }
   else if(a==2 && b==3)
   {
      return b;
   }
   else if(a==3 && b==1)
   {
      return b;
   }
   else if(a==3 && b==2)
   {
      return a;
   }
}

void send_message_server(struct sockaddr_in server, int port, int fd, char * message)
{
   

   if ((fd=socket(AF_INET, SOCK_STREAM, 0))==-1){  
      /* llamada a socket() */
      printf("socket() error\n");
      exit(-1);
   }

   server.sin_family = AF_INET;
   server.sin_port = htons(port); 
   /* htons() es necesaria nuevamente ;-o */
   bzero(&(server.sin_zero),8);

   if(connect(fd, (struct sockaddr *)&server,
      sizeof(struct sockaddr))==-1){ 
      /* llamada a connect() */
      printf("connect() error\n");
      exit(-1);
   }
   send(fd,message,22,0);
   close(fd);
   int i = 0;
}
main()
{

   int fd, fd2; /* los ficheros descriptores */

   struct sockaddr_in server; 
   /* para la información de la dirección del servidor */

   struct sockaddr_in client; 
   /* para la información de la dirección del cliente */
   
   
   struct sockaddr_in clients[2];/*Para guardar los clientes que se conectan*/
   
   
   int sin_size,numbytes,connex;
   
   char buf[MAXDATASIZE];
   char jug[11];
   int answers[2];

   /* A continuación la llamada a socket() */
   if ((fd=socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {  
      printf("error en socket()\n");
      exit(-1);
   }

   server.sin_family = AF_INET;         

   server.sin_port = htons(PORT); 
   /* ¿Recuerdas a htons() de la sección "Conversiones"? =) */

   server.sin_addr.s_addr = INADDR_ANY; 
   /* INADDR_ANY coloca nuestra dirección IP automáticamente */

   bzero(&(server.sin_zero),8); 
   /* escribimos ceros en el reto de la estructura */


   /* A continuación la llamada a bind() */
   if(bind(fd,(struct sockaddr*)&server,
           sizeof(struct sockaddr))==-1) {
      printf("error en bind() \n");
      exit(-1);
   }     

   if(listen(fd,BACKLOG) == -1) {  /* llamada a listen() */
      printf("error en listen()\n");
      exit(-1);
   }
   connex = 0;
   printf("\t\t**NUEVO JUEGO**\n");
   while(1) {
      printf("esperando jugador...\n");
      sin_size=sizeof(struct sockaddr_in);
      /* A continuación la llamada a accept() */
      if ((fd2 = accept(fd,(struct sockaddr *)&client,
                        &sin_size))==-1) {
         printf("error en accept()\n");
         exit(-1);
      }

      printf("Se obtuvo una conexión desde %s\n",
             inet_ntoa(client.sin_addr) ); 
      /* que mostrará la IP del cliente */

      send(fd2,"Bienvenido al juego!!!\n",22,0); 
      /* que enviará el mensaje de bienvenida al cliente */
      numbytes=recv(fd2,buf,MAXDATASIZE,0);
      printf("Mensaje de %s  :: Jugador %d : %s\n",inet_ntoa(client.sin_addr),connex+1,buf);
      
      if(strcmp(buf,"piedra")==0 || strcmp(buf,"Piedra")==0 || strcmp(buf,"PIEDRA")==0)
      {
         answers[connex] = 1;
         if(connex==0)
            send(fd2,"Jugador 1\n",11,0);
         else
            send(fd2,"Jugador 2\n",11,0);
         clients[connex] = client;
      }
      else if(strcmp(buf,"papel")==0 || strcmp(buf,"Papel")==0 || strcmp(buf,"PAPEL")==0)
      {
         answers[connex] = 2;
         if(connex==0)
            send(fd2,"Jugador 1\n",11,0);
         else
            send(fd2,"Jugador 2\n",11,0);
         clients[connex] = client;
      }
      else if(strcmp(buf,"tijera")==0 || strcmp(buf,"Tijera")==0 || strcmp(buf,"TIJERA")==0)
      {
         answers[connex] = 3;
         if(connex==0)
            send(fd2,"Jugador 1\n",11,0);
         else
            send(fd2,"Jugador 2\n",11,0);
         clients[connex] = client;
      }
      else
      {
         send(fd2,"Opcion invalida\n",22,0);
         connex--;
         printf("Conexion rechazada!!!\n");
      }
     
      close(fd2); /* cierra fd2 */
      ++connex;
      if(connex==2)
      {
         printf("\t\t**RESULTADOS**\n");
         connex = 0;
         buf[0] = '\0';
         if(ganador(answers[0],answers[1])==answers[0])
         {
            printf("gana Jugador 1\n");
            send_message_server(clients[0],PORT2,fd2,"gana Jugador 1");
            send_message_server(clients[1],PORT3,fd2,"gana Jugador 1");
         }
         else if(ganador(answers[0],answers[1])==answers[1])
         {
            printf("gana Jugador 2\n");
            send_message_server(clients[0],PORT2,fd2,"gana Jugador 2");
            send_message_server(clients[1],PORT3,fd2,"gana Jugador 2");
         }
         else
         {
            printf("empate\n");
            send_message_server(clients[0],PORT2,fd2,"empate");
            send_message_server(clients[1],PORT3,fd2,"empate");
         }
         printf("\t\t**NUEVO JUEGO**\n");
      }
      
      
   }
   
}
