#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <getopt.h>

//--serv_port 10050 --bufsize 100
#define SADDR struct sockaddr

int main(int argc, char *argv[]) {
  const size_t kSize = sizeof(struct sockaddr_in);

  int SERV_PORT = -1;
  int BUFSIZE = -1;

while (1) {
        int current_optind = optind ? optind : 1;
    
        static struct option options[] = {{"serv_port", required_argument, 0, 0},
                                          {"bufsize", required_argument, 0, 0},
                                          {0, 0, 0, 0}};
    
        int option_index = 0;
        int c = getopt_long(argc, argv, "", options, &option_index); 
        
        if (c == -1) break;
    
        switch (c) {
          case 0:
            switch (option_index) {
              case 0:
                SERV_PORT = atoi(optarg);
                if (SERV_PORT < 0) { 
                    SERV_PORT=-1;
                    return 1;
                }
                break;
                
              case 1:
                BUFSIZE = atoi(optarg);
                if (BUFSIZE < 1) { 
                    BUFSIZE=-1;
                    return 1;
                }
                break;
              defalut:
                printf("Index %d is out of options\n", option_index);
            }
            break;
          case '?':
            break; 
            
          default:
            printf("getopt returned character code 0%o?\n", c);
        }
      }

  if (BUFSIZE == -1 || SERV_PORT == -1) {
  printf("Usage: %s --BUFSIZE 100 --SERV_PORT 10050 \n", argv[0]);
  return 1;
  }

  int lfd, cfd;
  int nread;
  char buf[BUFSIZE];
  struct sockaddr_in servaddr;
  struct sockaddr_in cliaddr;

  if ((lfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    exit(1);
  }

  memset(&servaddr, 0, kSize);
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(SERV_PORT);

  //Привязываем к сокету lfd адрес servaddr
  if (bind(lfd, (SADDR *)&servaddr, kSize) < 0) {
    perror("bind");
    exit(1);
  }

//Сервер готов принимать соединения по сокету lfd, макс. количество - 5
  if (listen(lfd, 5) < 0) {
    perror("listen");
    exit(1);
  }

  while (1) {
    unsigned int clilen = kSize;

    if ((cfd = accept(lfd, (SADDR *)&cliaddr, &clilen)) < 0) {  // функция для принятия связи на сокет (по указанному адресу клиента, указанной длины структуры адреса)
      perror("accept");
      exit(1);
    }
    printf("connection established\n");

    while ((nread = read(cfd, buf, BUFSIZE)) > 0) { //считываем с cfd в buf
      write(1, &buf, nread);  //выводим buf(размером nread)
    }

    if (nread == -1) {   //ошибка при чтении
      perror("read");
      exit(1);
    }
    close(cfd);
  }
}
