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

  if (argc>1)    sscanf(argv[1],"%d",&ch);
  else    ch=0;

  fd=sw_open(ch);

  sw_w(fd,ch,0x3c,0);

  close(fd);
  exit(0);
}

