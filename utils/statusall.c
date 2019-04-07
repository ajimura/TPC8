#include <stdio.h>
#include <stdlib.h>
#ifdef PCIE
#include "swpci_lib.h"
#else
#include "swsoc_lib.h"
#endif

int main(int argc, char *argv[]) {
  int fd[8];
  int i;
  unsigned int com[8],tx[8],rx[8];

#ifdef PCIE
  fd[0]=sw_open(0);
  for(i=1;i<8;i++) fd[i]=fd[0];
#else
  for(i=0;i<8;i++)
    fd[i]=sw_open(i);
#endif

  for(i=0;i<8;i++){
    sw_r(fd[i],i,ADD_ST_REG,&(com[i]));
    sw_r(fd[i],i,ADD_TX_CSR,&(tx[i]));
    sw_r(fd[i],i,ADD_RX_CSR,&(rx[i]));
  }

  for(i=0;i<4;i++){
    printf("%d:",i);
    if ((com[i]&0x80000000)>0) printf("UP,"); else printf("DN,");
    if ((tx[i]&0x80000000)>0 || (rx[i]&0x80000000)>0) printf("NoEmpty "); else printf("Empty   ");
  }printf("\n");
  for(i=4;i<8;i++){
    printf("%d:",i);
    if ((com[i]&0x80000000)>0) printf("UP,"); else printf("DN,");
    if ((tx[i]&0x80000000)>0 || (rx[i]&0x80000000)>0) printf("NoEmpty "); else printf("Empty   ");
  }printf("\n");

  exit(0);
}
