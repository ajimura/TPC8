#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "swsoc_lib.h"

int main(int argc, char *argv[]) {
  int fd;
  int i;

  for(i=0;i<8;i++){
    fd=sw_open(i);
    sw_rx_flush(fd,i);
    close(fd);
  }

}

