#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#ifdef PCIE
#include "swpci_lib.h"
#else
#include "swsoc_lib.h"
#endif

int main(int argc, char *argv[]) {
  int fd;
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

  exit(0);
}
