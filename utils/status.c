#include <stdio.h>
#include <stdlib.h>
#ifdef PCIE
#include "swpci_lib.h"
#else
#include "swsoc_lib.h"
#endif

int main(int argc, char *argv[]) {
  int fd;
  int ch;
  if (argc>1){
    sscanf(argv[1],"%d",&ch);
    ch=ch%8;
  }else{
    ch=0;
  }
  fd=sw_open(ch);
  sw_print_status(fd,ch);
  sw_close(fd);
  exit(0);
}
