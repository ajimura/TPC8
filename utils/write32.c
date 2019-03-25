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
  int ret;
  int fd;
  int ch;
  unsigned int input;
  unsigned int address;
  unsigned  int data;
  unsigned int length=0;
  int i;
  int j;
  int wbuf[1096];
  int rbuf[1096];
  int loopnum;
  int size,rsize;
  int port;
  char DevName[8][16]={"/dev/swsoc0","/dev/swsoc1","/dev/swsoc2","/dev/swsoc3",
		       "/dev/swsoc4","/dev/swsoc5","/dev/swsoc6","/dev/swsoc7"};

  if (argc>1)    sscanf(argv[1],"%d",&ch);
  else    ch=0;
  if (argc>2) sscanf(argv[2],"%x",&address);
  else address=0;
  if (argc>3) sscanf(argv[3],"%x",&data);
  else data=0xcccc5555;

  fd=open(DevName[ch],O_RDWR);

  sw_w(fd,ch,address,data);

  close(fd);
}

