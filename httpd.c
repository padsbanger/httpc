#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define LISTENADDR "127.0.0.1"

char *error;


int srv_init(int portno) {
  int s;
  
  struct sockaddr_in srv;

  s = socket(AF_INET, SOCK_STREAM, 0);

  if (s < 1) {
    error ="socket() error";

    return 0;
  }

  srv.sin_family = AF_INET;
  srv.sin_addr.s_addr = inet_addr(LISTENADDR);
  srv.sin_port = htons(portno);



  if (bind(s, (struct sockaddr *)&srv, sizeof(srv))) {
    close(s);
    error = "bind() error";

    return 0;
  }
  
  if(listen(s, 5)) {
    close(s);
    error = "listen() error";

    return 0;
  }

  return s;


}

int cli_accept(int s) {

  int c;
  socklen_t addrlen;
  struct sockaddr_in cli;
  
  addrlen = 0;

  memset(&cli, 0, sizeof(cli));
  
  c = accept(s, (struct sockaddr *)&cli, &addrlen);
  
  if(c < 0) {
    error = "accept() error";
    return 0;
  }

  return c;

}

void client_conn(int s, int c) {
  return ;
}


int main(int argc, char *argv[]) {
  int s, c;
  char *port;

  if (argc < 2) {
    fprintf(stderr, "Usage: %s <listening port> \n", argv[0]);
    return -1;
  } else {
  
    port = argv[1];
    s = srv_init(atoi(port));

    if(!s) {
      fprintf(stderr, "%s\n", error);
      return -1;
    }

    
    printf("Listennin on %s:%s\n", LISTENADDR, port);

    while(1) {
      c = cli_accept(s);
      if(!c) {
        fprintf(stderr, "%s\n", error);
        continue;
      }
      printf("Incoming conneciton \n");
      
      if( !fork()) {
        client_conn(s, c);
      }

      return -1;

    }



  }
  


}
