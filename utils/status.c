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
  int fd,ret;
  int ch;
  if (argc>1){
    sscanf(argv[1],"%d",&ch);
    ch=ch%8;
  }else{
    ch=0;
  }

  fd=open(DevName[ch],O_RDWR);

  sw_print_status(fd,ch);
}
