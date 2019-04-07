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
  unsigned int address;
  unsigned int data;

  if (argc>1)    sscanf(argv[1],"%d",&ch);
  else    ch=0;
  if (argc>2) sscanf(argv[2],"%x",&address);
  else address=0;

  fd=sw_open(ch);

  sw_r(fd,ch,address,&data);
  printf("%08x\n",data);

  close(fd);
  exit(0);
}

