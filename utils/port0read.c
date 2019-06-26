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
int port=-1;
int nodeid=-1;

struct rmap_node_info n;

int verbose=0;

int main(int argc, char *argv[]) {

  int sw_fd;

  unsigned int add, data;
  unsigned int logaddr;
  int st=0;
  int i,j;

  for(i=1;i<argc;i++){
    if (argv[i][0]=='-'){
      if (argv[i][1]=='v') verbose=1;
    }else{
      if (port<0) sscanf(argv[i],"%d",&port);
      else if (nodeid<0) sscanf(argv[i],"%d",&nodeid);
    }
  }

  if (port<0){
    printf("Port#? ");
    scanf("%d",&port);
  }

  if (nodeid<0){
    printf("Node ID? ");
    scanf("%d",&nodeid);
  }
  
  logaddr=nodeid+32;

  // open
  sw_fd=sw_open(port);
  if (sw_link_test(sw_fd,port)){
    printf("Link #%d is not ready\n",port); exit(0);
  }

  // set node info.
  n.out_size=0; n.in_size=0;
  n.dest_addr=logaddr; n.src_addr=srcaddr;
  n.key=key;

  if (verbose>0)
    for(i=32;i<255;i+=8){
      printf("[%3d(%02x)] ",i,i);
      for(j=0;j<8;j++){
	add=(i+j)*4;
	st=rmap_get_data(sw_fd,port,&n,add,&data,4);
	printf("%08X ",be32toh(data));
      }printf("\n");
    }

  add=0x0908;
  st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("[%04X]%08X ",add,be32toh(data));
  add=0x090c;
  st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("[%04X]%08X ",add,be32toh(data));
  add=0x0918;
  st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("[%04X]%08X\n",add,be32toh(data));

  add=0x2100;
  st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("[%04X]%08X ",add,be32toh(data));
  add=0x2104;
  st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("[%04X]%08X ",add,be32toh(data));
  add=0x2108;
  st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("[%04X]%08X ",add,be32toh(data));
  add=0x2124;
  st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("[%04X]%08X ",add,be32toh(data));
  add=0x2128;
  st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("[%04X]%08X\n",add,be32toh(data));

  add=0x2200;
  st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("[%04X]%08X ",add,be32toh(data));
  add=0x2204;
  st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("[%04X]%08X ",add,be32toh(data));
  add=0x2208;
  st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("[%04X]%08X ",add,be32toh(data));
  add=0x2224;
  st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("[%04X]%08X ",add,be32toh(data));
  add=0x2228;
  st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("[%04X]%08X\n",add,be32toh(data));

  add=0x2300;
  st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("[%04X]%08X ",add,be32toh(data));
  add=0x2304;
  st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("[%04X]%08X ",add,be32toh(data));
  add=0x2308;
  st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("[%04X]%08X ",add,be32toh(data));
  add=0x2324;
  st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("[%04X]%08X ",add,be32toh(data));
  add=0x2328;
  st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("[%04X]%08X\n",add,be32toh(data));

  if (st<0) printf("RMAP Error\n");
  sw_close(sw_fd);
  exit(0);
}

int conv_int_bit4(int in){
  int out;
  out=(in&1)+((in>>1)&1)*10+((in>>2)&1)*100+((in>>3)&1)*1000;
  return(out);
}
