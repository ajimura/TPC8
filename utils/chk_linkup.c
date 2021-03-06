#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <endian.h>
#ifdef PCIE
#include "swpci_lib.h"
#else
#include "swsoc_lib.h"
#endif
#include "rmap_lib.h"

unsigned int key=0x02;
unsigned int srcaddr=0x80;
int port;

struct rmap_node_info n;

int main(int argc, char *argv[]) {

  int NumFADC[8];
  int sw_fd[8];

  unsigned int add, data;
  int st=0;
  int i,j;

  for(i=0;i<8;i++) NumFADC[i]=0;

  if (argc<2){
    printf("few argument...\n");
    exit(-1);
  }
  for(i=1;i<argc;i++){
    sscanf(argv[i],"%d",&(NumFADC[i-1]));
  }

  // open
  for(i=0;i<8;i++){
    if (NumFADC[i]>0){
      sw_fd[i]=sw_open(i);
    }
  }

  for(i=0;i<8;i++){
    for(j=0;j<NumFADC[i];j++){
      // set node info.
      n.out_size=0; n.in_size=0;
      n.dest_addr=0x20+j; n.src_addr=srcaddr;
      n.key=key;

      printf("%d-%2d: ", i, j);

      add=0x2100;
      st+=rmap_get_data(sw_fd[i],port,&n,add,&data,4);
      printf("(%08X)",be32toh(data));
      add=0x2124;
      st+=rmap_get_data(sw_fd[i],port,&n,add,&data,4);
      printf("%08X ",be32toh(data));

      add=0x2200;
      st+=rmap_get_data(sw_fd[i],port,&n,add,&data,4);
      printf("(%08X)",be32toh(data));
      add=0x2224;
      st+=rmap_get_data(sw_fd[i],port,&n,add,&data,4);
      printf("%08X ",be32toh(data));

      add=0x2300;
      st+=rmap_get_data(sw_fd[i],port,&n,add,&data,4);
      printf("(%08X) ",be32toh(data));
      add=0x2324;
      st+=rmap_get_data(sw_fd[i],port,&n,add,&data,4);
      printf("%08X\n",be32toh(data));
    }
  }

  if (st<0) printf("RMAP Error\n");

  for(i=0;i<8;i++) if (NumFADC[i]>0) sw_close(sw_fd[i]);
  exit(0);
}
