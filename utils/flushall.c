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

char DevName[8][16]={"/dev/swsoc0","/dev/swsoc1","/dev/swsoc2","/dev/swsoc3",
		     "/dev/swsoc4","/dev/swsoc5","/dev/swsoc6","/dev/swsoc7"};

int main(int argc, char *argv[]) {
  int fd;
  int i;

  for(i=0;i<8;i++){
    fd=open(DevName[i],O_RDWR);
    sw_rx_flush(fd,i);
    close(fd);
  }

}

