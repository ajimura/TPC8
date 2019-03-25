#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <endian.h>
#include "swsoc_lib.h"
#include "rmap_lib.h"

unsigned int key=0x02;
unsigned int srcaddr=0x80;
int port;

struct rmap_node_info n;

char DevName[8][16]={"/dev/swsoc0","/dev/swsoc1","/dev/swsoc2","/dev/swsoc3",
		     "/dev/swsoc4","/dev/swsoc5","/dev/swsoc6","/dev/swsoc7"};

int main(int argc, char *argv[]) {

  int sw_fd;

  unsigned int add, data;
  unsigned int nodeid;
  unsigned int logaddr;
  int st;
  int i,j;

  printf("Port#? ");
  scanf("%d",&port);

  //  printf("Logical Address? ");
  //  scanf("%d",&logaddr);
  printf("Node ID? ");
  scanf("%d",&nodeid);
  logaddr=nodeid+32;
  
  // open
  sw_fd=open(DevName[port],O_RDWR);
  //  sw_fd=sw_open();
  if (sw_link_test(sw_fd,port)){
    printf("Link #%d is not ready\n",port); exit(0);
  }

  // set node info.
  n.out_size=0; n.in_size=0;
  n.dest_addr=logaddr; n.src_addr=srcaddr;
  n.key=key;

  for(i=32;i<255;i+=8){
    printf("[%3d(%02x)] ",i,i);
    for(j=0;j<8;j++){
      add=(i+j)*4;
      st=rmap_get_data(sw_fd,port,&n,add,&data,4);
      printf("%08X ",be32toh(data));
    }printf("\n");
  }
  add=0x0908;
  st=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("[%04X] %08X\n",add,be32toh(data));
  add=0x090c;
  st=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("[%04X] %08X\n",add,be32toh(data));
  add=0x2100;
  st=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("[%04X] %08X   ",add,be32toh(data));
  add=0x2124;
  st=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("[%04X] %08X\n",add,be32toh(data));
  add=0x2200;
  st=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("[%04X] %08X   ",add,be32toh(data));
  add=0x2224;
  st=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("[%04X] %08X\n",add,be32toh(data));
  add=0x2300;
  st=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("[%04X] %08X   ",add,be32toh(data));
  add=0x2324;
  st=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("[%04X] %08X\n",add,be32toh(data));

  sw_close(sw_fd);
  exit(0);
}

int conv_int_bit4(int in){
  int out;
  out=(in&1)+((in>>1)&1)*10+((in>>2)&1)*100+((in>>3)&1)*1000;
  return(out);
}
