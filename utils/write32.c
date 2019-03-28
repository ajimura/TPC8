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
  int ch;
  unsigned int address;
  unsigned  int data;

  if (argc>1)    sscanf(argv[1],"%d",&ch);
  else    ch=0;
  if (argc>2) sscanf(argv[2],"%x",&address);
  else address=0;
  if (argc>3) sscanf(argv[3],"%x",&data);
  else data=0xcccc5555;

  fd=sw_open(ch);

  sw_w(fd,ch,address,data);

  close(fd);
}

