#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <stdbool.h>
#include <stdint.h>
#define PORT 9000
bool recvBuffer(int s, unsigned char *buf, int size);



int main(int argc, char const *argv[])
{
  int sock = 0, valread;
  struct sockaddr_in serv_addr;
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    printf("\n Socket creation error \n");
    return -1;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
  {
    printf("\nInvalid address/ Address not supported \n");
    return -1;
  }

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    printf("\nConnection Failed \n");
    return -1;
  }
  char message[5000]={0};
  bool status=false;
  char message1[100]={0};
  char name[20];
  char message2[100]={0};
  int num;
  char message3[5000] = {0};
  char trial[100]= "You Win!";

  char memo[256]={0};
  char str[20];
  unsigned char final_memo[8192];
  char finalSize[5000];

  char mem[256] = {0};
  char string[20];
  char final_mem[1024] = {0};



  int i;
  if (argc >= 2)
  {
    for (i = 1; i < argc; i++)
    {
      int input = atoi(argv[i]);
      if (send(sock, &input, sizeof(int), 0) < 0)
      {
        perror("Error in sending");
        return -1;
      }
      if (input == 5)
      {
        recv(sock, &message1, sizeof(message1),0);
        printf("%s\n", message1);
        scanf("%s", name);
        send(sock, &name, sizeof(name), 0);

        recv(sock, &message2, sizeof(message2), 0);
        printf("%s\n", message2);
       
         while(1){
          scanf("%d", &num);
          send(sock, &num, sizeof(int), 0);
         
          recv(sock, &message3, sizeof(message3), 0);

          int comp = strncmp(message3, trial,7);
          printf("%s\n", message3);

          if(comp==0){
            break;
          }
        }
      }
      else if (input == 4)
      {
        recv(sock, &memo, sizeof(memo), 0);
        printf("%s", memo);
        scanf("%s", str);
        send(sock, str, sizeof(str), 0);

        recv(sock, finalSize, sizeof(finalSize), 0);
        recvBuffer(sock, final_memo, strlen(finalSize)+1);
        printf("%s\n", final_memo);

      }
      else if (input == 6)
      {
        recv(sock, &mem, sizeof(mem), 0);
        printf("%s", mem);
        scanf("%s", string);
        send(sock, string, sizeof(str), 0);

        recv(sock, final_mem, sizeof(final_mem), 0);
        printf("%s\n", final_mem);
      }

      else
      {
        recv(sock, message, sizeof(message), 0);
        printf("%s\n", message);
      }
    }
  }
  close(sock);

  return 0;
}
bool recvBuffer(int s, unsigned char *buf, int size)
{
  int sz = 0;

  while (size > 0)
  {
    sz = recv(s, buf, size, 0);
    if (sz < 0){
      return false; 
    }
    size -= sz;     
    buf += sz;      
  }
  
  return true; 
}






