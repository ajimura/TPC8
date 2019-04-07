#include <stdio.h>
#include <stdlib.h>
#ifdef PCIE
#include "swpci_lib.h"
#else
#include "swsoc_lib.h"
#endif

int main(int argc, char *argv[]) {
  int fd;
  int i;
  for(i=0;i<8;i++){
    fd=sw_open(i);
    sw_rx_flush(fd,i);
    sw_close(fd);
  }
  exit(0);
}

