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

// structs //

struct sHttpRequest {
  char method[8];
  char url[256];

};

typedef struct sHttpRequest httpreq;


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

httpreq *parse_http(char *str) {
  httpreq *req;
  char *p;

  req = malloc(sizeof(httpreq));

  for (p=str; *p && *p !=' '; p++);
  if( *p == ' ') {
    *p = 0;
  } else {
    error = "parse_http() error";
    free(req);
    return 0;
  }
  
  strncpy(req->method, str, 7);

  for (str=++p; *p && *p !=' '; p++);
  if( *p == ' ') {
    *p = 0;
  } else {
    error = "parse_http() error";
    free(req);
    return 0;
  }
  
  strncpy(req->url, str, 255);


  return req;
 
}

char *cli_read(int c) {
  static char buf[512];
  memset(buf, 0, 512);
  if (read(c, buf, 511) < 0) {
    error = "read() error";
    return 0;
  } else {
    return buf;
  }

}

void client_conn(int s, int c) {

  httpreq *req;
  char buf[512];
  char *p;

  p = cli_read(c);

  if(!p) {
    close(c);
    return ;
  }


  req = parse_http(p);

  if(!req) {
    close(c);
    return ;
  }

  printf("%s\n", req->method);
  printf("%s\n", req->url);

  free(req);
  close(c);

  return ;

}


int main(int argc, char *argv[]) {
  int s, c, n;
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

    
    printf("Listening on %s:%s\n", LISTENADDR, port);

    while(1) {
      c = cli_accept(s);
      if(!c) {
        fprintf(stderr, "%s\n", error);
        continue;
      }
      printf("Incoming connection:  \n");
      
      if( !fork()) {
        client_conn(s, c);
      }

      return -1;

    }



  }
  


}
