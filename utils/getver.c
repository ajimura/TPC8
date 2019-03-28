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
  int fd,ret;
  int ch;
  unsigned int data;

  if (argc>1){
    sscanf(argv[1],"%d",&ch);
    ch=ch%8;
  }else{
    ch=0;
  }

  fd=sw_open(ch);

  sw_r(fd,0,ADD_CM_REG,&data);

  printf("Ver[%d] = %08X\n",ch,data);

}
