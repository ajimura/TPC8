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
  int clockdiv;
  if (argc>1){
    sscanf(argv[1],"%d",&ch);
    ch=ch%8;
  }else{
    ch=0;
  }
  if (argc>2){
    sscanf(argv[2],"%d",&clockdiv);
  }else{
    clockdiv=0;
  }

  fd=sw_open(ch);

  sw_w(fd,ch,ADD_CK_REG,clockdiv);

  sw_print_status(fd,ch);
}
