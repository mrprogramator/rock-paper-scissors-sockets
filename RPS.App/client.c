#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>  
#include <stdlib.h>
#include <string.h>

      
/* netbd.h es necesitada por la estructura hostent guiño */

#define PORT 3550         
/* El Puerto Abierto del nodo remoto */

#define PORT2 3555 /* Puerto Abierto para recibir resultados*/
#define BACKLOG 2 /* El número de conexiones permitidas */
#define MAXDATASIZE 22   
/* El número máximo de datos en bytes */

int main(int argc, char *argv[])
{
   int fd,fd2, numbytes;       
   /* ficheros descriptores */
   
   char buf[MAXDATASIZE];  
   /* en donde es almacenará el texto recibido */

   char jug[MAXDATASIZE];
   
   char res[MAXDATASIZE];
   struct hostent *he;         
   /* estructura que recibirá información sobre el nodo remoto */

   struct sockaddr_in server, client;
   int sin_size;  
   /* información sobre la dirección del servidor */

   if (argc !=3) { 
      /* esto es porque nuestro programa sólo necesitará un
      argumento, (la IP) */
      printf("Uso: %s <Dirección IP>\n",argv[0]);
      exit(-1);
   }

   if ((he=gethostbyname(argv[1]))==NULL){       
      /* llamada a gethostbyname() */
      printf("gethostbyname() error\n");
      exit(-1);
   }

   if ((fd=socket(AF_INET, SOCK_STREAM, 0))==-1){  
      /* llamada a socket() */
      printf("socket() error\n");
      exit(-1);
   }

   server.sin_family = AF_INET;
   server.sin_port = htons(PORT); 
   /* htons() es necesaria nuevamente ;-o */
   server.sin_addr = *((struct in_addr *)he->h_addr);  
   /*he->h_addr pasa la información de ``*he'' a "h_addr" */
   bzero(&(server.sin_zero),8);

   if(connect(fd, (struct sockaddr *)&server,
      sizeof(struct sockaddr))==-1){ 
      /* llamada a connect() */
      printf("connect() error\n");
      exit(-1);
   }

   if ((numbytes=recv(fd,buf,MAXDATASIZE,0)) == -1){  
      /* llamada a recv() */
      printf("Error en recv() \n");
      exit(-1);
   }

   buf[numbytes]='\0';

   printf("%s\n",buf); 
   /* muestra el mensaje de bienvenida del servidor =) */
   
   send(fd,argv[2],22,0);
   
   if ((numbytes=recv(fd,jug,MAXDATASIZE,0)) == -1)
   {  
      /* llamada a recv() */
      printf("Error en recv JUG() \n");
      exit(-1);
   }
   
   jug[numbytes]='\0';
   printf("%s\n",jug); 
   close(fd);   /* cerramos fd =) */

   printf("esperando resultados...\n");
   if ((fd2=socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {  
      printf("error en socket()\n");
      exit(-1);
   }

   client.sin_family = AF_INET;

   client.sin_port = htons(PORT2); 
   /* ¿Recuerdas a htons() de la sección "Conversiones"? =) */

   client.sin_addr.s_addr = INADDR_ANY; 
   /* INADDR_ANY coloca nuestra dirección IP automáticamente */

   bzero(&(client.sin_zero),8); 
   /* escribimos ceros en el reto de la estructura */


   /* A continuación la llamada a bind() */
   if(bind(fd,(struct sockaddr*)&client,
           sizeof(struct sockaddr))==-1) {
      printf("error en bind() \n");
      exit(-1);
   } 
   
   if(listen(fd2,BACKLOG) == -1) {  /* llamada a listen() */
      printf("error en listen()\n");
      exit(-1);
   }
   if ((fd = accept(fd,(struct sockaddr *)&server,
                     &sin_size))==-1) {
      printf("error en accept()\n");
      exit(-1);
   }
   
   if ((numbytes=recv(fd,res,MAXDATASIZE,0)) == -1)
   {  
      /* llamada a recv() */
      printf("Error en recv RES() \n");
      exit(-1);
   }
   close(fd); /*Cierra la conexión con el servidor*/
   res[numbytes]='\0';
   printf("%s\n",res);

}