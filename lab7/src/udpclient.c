#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <getopt.h>

// --serv_port 20001 --bufsize 1024 
#define SADDR struct sockaddr
#define SLEN sizeof(struct sockaddr_in)

int main(int argc, char **argv) {
  int BUFSIZE = -1;
  int SERV_PORT = -1;

  while (1) {
	    int current_optind = optind ? optind : 1;
	
	    static struct option options[] = {{"serv_port", required_argument, 0, 0},
	                                      {"bufsize", required_argument, 0, 0},
	                                      {0, 0, 0, 0}};
	  int option_index = 0;
	    int c = getopt_long(argc, argv, "",options, &option_index);
	
	    if (c == -1) break;
	
	    switch (c) {
	      case 0:
	        switch (option_index) {
	          case 0:
	            SERV_PORT = atoi(optarg);
	            if (SERV_PORT <= 0) {
	                printf("SERV_PORT must be a positive number\n");
	                return 1;
	              }
	            break;
	
	          case 1:
                BUFSIZE = atoi(optarg);
	            if (BUFSIZE <= 0) {
	                printf("BUFSIZE must be a positive number\n");
	                return 1;
	              }
	            break;
	
	        }
	
	      case '?':
	        break;
	
	      default:
	        printf("getopt returned character code 0%o?\n", c);
	    }
	
	  }
	  if (BUFSIZE == -1 || SERV_PORT == -1 ) {
	    printf("Usage: %s --bufsize \"buffer_size\" --serv_port \"serv_port\"\n",
	           argv[0]);
	    return 1;
	  }  
  
  int sockfd, n;
  char sendline[BUFSIZE], recvline[BUFSIZE + 1];
  struct sockaddr_in servaddr;
  struct sockaddr_in cliaddr;


  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(SERV_PORT);

  //int inet_pton(int af, const char *src, void *dst);
//Данная функция преобразует строку символов src (первый аргумент КС) в сетевой адрес (типа af), затем копирует полученную структуру с адресом в dst.
//Возвращается отр. значение, если первый аргумент не содержит правильного типа адреса
//Возвращается 0, если src не содержит строку символов, представляющую правильный сетевой адрес (для указанного типа адресов)
//Если сетевой адрес был успешно преобразован, то возвращается положительное значение. 

  if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) < 0) {
    perror("inet_pton problem");
    exit(1);
  }

  //Создание сокета
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket problem");
    exit(1);
  }

  write(1, "Enter string\n", 13);

//Чтение с терминала sendline и попытка отправки по сокету 
  while ((n = read(0, sendline, BUFSIZE)) > 0) {
    if (sendto(sockfd, sendline, n, 0, (SADDR *)&servaddr, SLEN) == -1) {
      perror("sendto problem");
      exit(1);
    }

// получение по сокету строки recvline 
    if (recvfrom(sockfd, recvline, BUFSIZE, 0, NULL, NULL) == -1) {
      perror("recvfrom problem");
      exit(1);
    }

    printf("REPLY FROM SERVER= %s\n", recvline);
  }
  close(sockfd);
}
