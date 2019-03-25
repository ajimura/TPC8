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
  int fd[8],ret;
  int ch;
  int i;
  unsigned int com[8],tx[8],rx[8];

  for(i=0;i<8;i++)
    fd[i]=open(DevName[i],O_RDWR);

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
}
