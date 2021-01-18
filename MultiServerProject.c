  #include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h> 
#include <sys/types.h>
#include <stdbool.h>
#include <time.h>
#include <sys/stat.h>
#include <pthread.h>

void *create(void *para);
void *LsLong(void *para);
void *Cat(void *para);
void *CatGrep(void *para);
void *PlayGame(void *para);
void *CatGrepGrep(void *para);
bool sendBuffer(int s, unsigned char *buf, int size);
void *handle_connection(void *arg);

#define PORT 9000
int main()
{

  int sock1, sock2;
  struct sockaddr_in address;
  int addrlen = sizeof(address);

  if ((sock1 = socket(AF_INET, SOCK_STREAM, 0)) == 0)
  {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  if (bind(sock1, (struct sockaddr *)&address,
           sizeof(address)) < 0)
  {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  while (1)
  {
    if (listen(sock1, 20) < 0)
    {
      perror("Listen Failed");
      exit(EXIT_FAILURE);
    }
    if ((sock2 = accept(sock1, (struct sockaddr *)&address,
                        (socklen_t *)&addrlen)) < 0)
    {
      perror("accept");
      exit(EXIT_FAILURE);
    }
    pthread_t main_thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&main_thread, &attr, handle_connection, &sock2);
  }
  close(sock1);
  return 0;
}

  void *handle_connection(void *arg)
  {
    int *sock = (int *)arg;
    int newsock = *sock;

    int buffer;
    int valrecv;

    buffer = recv(newsock, &valrecv, sizeof(int), 0);
    if (valrecv < 0)
    {
      perror("Error in receiving");
    }
    else if (valrecv == 1)
    {
      chdir("/Users/hikmatsultan/Desktop/Game");
      char *exp[] = {"touch", "Game/GameHistory.txt", NULL};
      char buffer[] = "The file GameHistory was created inside the Directory Game: \n";
      send(newsock, buffer, strlen(buffer), 0);
      mkdir("/Users/hikmatsultan/Desktop/Game", 0777);

      int fd[2];
      pipe(fd);
      pid_t pid = fork();
      if (pid == 0)
      {
        close(fd[0]);
        dup2(fd[1], 1);
        close(fd[1]);
        execvp(*exp, exp);
      }
      else if (pid > 0)
      {
        wait(NULL);
        char buffer[1024];
        close(fd[1]);
        read(fd[0], buffer, sizeof(buffer));
        send(newsock, buffer, sizeof(buffer), 0);
      }
    }

    else if (valrecv == 2)
    {
      chdir("/Users/hikmatsultan/Desktop/Game");
      char *exp3[] = {"/bin/ls", "-l", NULL};
      int fd[2];
      pipe(fd);
      pid_t pid = fork();
      if (pid == 0)
      {
        close(fd[0]);
        dup2(fd[1], 1);
        close(fd[1]);
        execvp(*exp3, exp3);
      }
      else if (pid > 0)
      {
        char buffer[1024];
        close(fd[1]);
        read(fd[0], buffer, sizeof(buffer));
        send(newsock, buffer, sizeof(buffer), 0);
      }
    }

    else if (valrecv == 3)
    {
      
      chdir("/Users/hikmatsultan/Desktop/Game");
      char *exp4[] = {"/bin/cat", "GameHistory.txt", NULL};
      int fd[2];
      pipe(fd);
      pid_t pid = fork();
      if (pid == 0)
      {
        close(fd[0]);
        dup2(fd[1], 1);
        close(fd[1]);
        execvp(*exp4, exp4);
      }
      else if (pid > 0)
      {
        char buffer[1024];
        close(fd[1]);
        read(fd[0], buffer, sizeof(buffer));
        send(newsock, buffer, sizeof(buffer), 0);
      }
    }
    else if (valrecv == 4)
    {
      char str[20];
      char command[1024];
      int bites = 0;
      int total = 0;
      chdir("/Users/hikmatsultan/Desktop/Game");

      char message[] = "Enter a name to view his or her history:\n";
      send(newsock, message, strlen(message), 0);

      recv(newsock, &str, sizeof(str), 0);

      sprintf(command, "%s %s %s %s", "grep", "-i", str, "GameHistory.txt");

      int fd[2];
      pipe(fd);
      if (fd < 0)
      {
        perror("Error in pipe");
      }
      pid_t pid;
      pid = fork();
      if (pid < 0)
      {
        perror("Fork failed");
        exit(EXIT_FAILURE);
      }
      if (pid == 0)
      {

        close(1);
        dup(fd[1]);
        //dup2(1,fd[1]);
        //close(1); I was using this method instead , it worked but it kept giving me an error message "bad file descriptor".
        close(fd[0]);
        close(fd[1]);
        char *exp1[] = {"cat", "GameHistory.txt", (char *)NULL};
        execvp(*exp1, exp1);
      }
      else if (pid > 0)
      {

        // dup2(0, fd[0]);
        //close(0); same here.
        close(0);
        dup(fd[0]);
        close(fd[1]);
        close(fd[0]);
        FILE *fp;
        if ((fp = popen(command, "r")) == NULL)
        {
          perror("popen failed");
        }

        int stringSize = 5000;
        char *returnnum1 = malloc(stringSize);
        if (!returnnum1)
        {
          perror("returnnum1 allocation failed");
        }
        returnnum1[0] = '\0';

        char buff[5000];
        int n;
        while ((n = fread(buff, 1, sizeof(buff) - 1, fp)) > 0)
        {
          buff[n] = '\0';

          strcat(returnnum1, buff);
        }

        pclose(fp);
        char finalSize[strlen(returnnum1) + 1];
        strcpy(finalSize, returnnum1);
        send(newsock, finalSize, sizeof(finalSize), 0);
        sendBuffer(newsock, returnnum1, strlen(returnnum1) + 1);
      }
      else
      {
        perror("Fork failed");
        exit(EXIT_FAILURE);
      }
      close(fd[0]);
      close(fd[1]);
    }
    else if (valrecv == 5)
    {
      char name[20];
      bool status = false;
      int count = 0;
      int num;

      int fd[2];
      int fd1[2];
      pipe(fd);
      pipe(fd1);

      if (fd < 0)
      {
        perror("Error in the first pipe");
      }
      if (fd1 < 0)
      {
        printf("Error in the second pipe");
      }
      char message1[] = "Enter your name:";
      send(newsock, message1, strlen(message1), 0);

      recv(newsock, &name, sizeof(name), 0);

      char message2[] = "Enter a number between 1 and 52:";
      send(newsock, &message2, strlen(message2), 0);

      pid_t pid1;
      pid1 = fork();

      if (pid1 < 0)
      {
        perror("Fork failed");
        exit(EXIT_FAILURE);
      }
      srand(time(NULL));
      int randomNumber = (rand() % 52) + 1;

      while (status == false)
      {

        int point;
        if (pid1 == 0)
        {

          recv(newsock, &num, sizeof(int), 0);

          close(fd[0]);
          write(fd[1], &num, sizeof(int));

          close(fd1[1]);
          read(fd1[0], &point, sizeof(int));

          if (point == 0)
          {
            exit(0);
          }
          close(fd1[1]);
        }
        else if (pid1 > 0)
        {
          int num1;

          close(fd1[0]);
          int pointer;
          close(fd[1]);
          read(fd[0], &num1, sizeof(int));

          if (num1 == randomNumber)
          {
            count++;
            char message3[] = "You Win!   ";
            send(newsock, message3, strlen(message3), 0);

            FILE *file;
            file = fopen("/Users/hikmatsultan/Desktop/Game/GameHistory.txt", "a+");
            if (file != NULL)
            {
              fprintf(file, name);
              fprintf(file, " tried %d", count);
              fprintf(file, " times.\n");
              fclose(file);
            }
            else
            {
              printf("Could not open file\n");
            }
            status = true;
            break;
          }
          else if (num1 < randomNumber)
          {
            char message3[] = "Too Low: ";
            send(newsock, message3, strlen(message3), 0);

            count++;
            pointer = 1;
            write(fd1[1], &pointer, sizeof(int));
          }
          else if (num1 > randomNumber)
          {
            char message3[] = "Too High: ";
            send(newsock, message3, strlen(message3), 0);

            count++;
            pointer = 1;
            close(fd1[0]);
            write(fd1[1], &pointer, sizeof(pointer));
          }
        }
      }
    }

    else if (valrecv == 6)
    {
      char str[20];
      char command[1024];

      chdir("/Users/hikmatsultan/Desktop/Game");
      char message[] = "Enter the name of the player to view how many times he or she has played:\n";
      send(newsock, message, strlen(message), 0);

      recv(newsock, &str, sizeof(str), 0);

      sprintf(command, "%s %s %s %s", "grep", "-c", str, "GameHistory.txt");

      int fd[2];
      pipe(fd);
      if (fd < 0)
      {
        perror("Error in pipe");
      }
      pid_t pid1;
      pid1 = fork();
      if (pid1 < 0)
      {
        perror("Fork failed");
        exit(EXIT_FAILURE);
      }
      if (pid1 == 0)
      {

        close(1);
        dup(fd[1]);
        close(fd[0]);
        close(fd[1]);
        char *exp1[] = {"cat", "GameHistory.txt", (char *)NULL};
        execvp(*exp1, exp1);
      }
      else
      {
        int fd1[2];
        pipe(fd1);
        if (fd1 < 0)
        {
          perror("Error in the second pipe");
        }
        pid_t pid2;
        pid2 = fork();
        if (pid2 == 0)
        {

          close(0);
          dup(fd[0]);
          close(fd[1]);
          close(fd[0]);
          char *exp2[] = {"grep", "-i", str, "GameHistory.txt", (char *)NULL};
          close(1);
          dup(fd1[1]);
          close(fd1[0]);
          close(fd1[1]);
          execvp(*exp2, exp2);
        }
        else if (pid2 > 0)
        {
          close(0);
          dup(fd1[0]);
          close(fd1[1]);
          close(fd1[0]);
          chdir("/Users/hikmatsultan/Desktop/Game");
          FILE *fp;
          if ((fp = popen(command, "r")) == NULL)
          {
            perror("popen failed");
          }

          int stringSize = 1024;
          char *returnnum1 = malloc(stringSize);
          if (!returnnum1)
          {
            perror("returnnum1 failed");
          }
          returnnum1[0] = '\0';

          char buff[128];
          int n;
          while ((n = fread(buff, 1, sizeof(buff) - 1, fp)) > 0)
          {
            buff[n] = '\0';

            strcat(returnnum1, buff);
          }

          pclose(fp);
          send(newsock, returnnum1, sizeof(returnnum1), 0);
        }
        else
        {
          perror("Fork failed");
        }
      }

      close(fd[0]);
      close(fd[1]);
    }
    close(newsock);
    return NULL;
  }

bool sendBuffer(int s, unsigned char *buf, int size)
{
  int sz = 0;
  int sz2 = 0;
  while (size > 0)
  {
    sz = send(s, buf, size, 0);
    if (sz < 0)
    {
      return false;
    }
    size -= sz;
    buf += sz;
    sz2 += sz;
  }
  return true;
}
 