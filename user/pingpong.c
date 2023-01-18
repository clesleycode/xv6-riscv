#include <stdbool.h>

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int p1[2];
  int p2[2];

  pipe(p1);
  // p1[0] == write end
  // p1[1] == read end
  pipe(p2);
  // p2[0] == write end
  // p2[1] == read end

  int pid = fork();

  if(pid > 0) {
    // Parent context; assign "pong" role.
    printf("parent context (pong)\n");

    char b[1];

    while(b[0] != 127) {
      read(p1[0], b, 1);

      printf("parent context: child wrote a byte to us = %c\n", b[0]);
      b[0] = b[0] + 1;

      write(p2[1], b, 1);
    }

    int status;
    int childpid = wait(&status);

    printf("parent context: child pid = %d exited with status = %d\n",
      childpid, status);
  } else if(pid == 0) {
    // Child context; assign "ping" role.
    char b[1];
    b[0] = '!';

    while (b[0] != 127) {
      write(p1[1], b, 1);

      read(p2[0], b, 1);
      printf("child context: parent wrote a byte to us = %c\n",
        b[0]);
    }

    exit(0);
  } else {
    printf("fork error\n");
  }

  exit(0);
}

/*
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

char buf[512];

void
cat(int fd)
{
  int n;

  while((n = read(fd, buf, sizeof(buf))) > 0) {
    if (write(1, buf, n) != n) {
      fprintf(2, "cat: write error\n");
      exit(1);
    }
  }
  if(n < 0){
    fprintf(2, "cat: read error\n");
    exit(1);
  }
}

int
main(int argc, char *argv[])
{
  int fd, i;

  if(argc <= 1){
    cat(0);
    exit(0);
  }

  for(i = 1; i < argc; i++){
    if((fd = open(argv[i], 0)) < 0){
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      exit(1);
    }
    cat(fd);
    close(fd);
  }
  exit(0);
}
*/
