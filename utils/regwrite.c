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

//unsigned int key=0x02;
unsigned int srcaddr=0x80;
int port;

struct rmap_node_info n;

int main(int argc, char *argv[]) {

  int sw_fd;

  unsigned int add, data;
  unsigned int logaddr;
  unsigned int key;
  int st;
  int nodeid,offset;

  printf("Port#? ");
  scanf("%d",&port);

  printf("node id? ");
  scanf("%d",&nodeid);

  printf("Router(0), FADC(1)? ");
  scanf("%d",&offset);
  if (offset==0){ logaddr=nodeid+32; key=0x02; }
  else { logaddr=nodeid+160; key=0xcc; }

  printf("address? 0x");
  scanf("%x",&add);

  printf("data? 0x");
  scanf("%x",&data);
  if (offset==0) data=htobe32(data);
  
  // open
  sw_fd=sw_open(port);
  if (sw_link_test(sw_fd,port)){
    printf("Link #%d is not ready\n",port); exit(0);
  }

  // set node info.
  n.out_size=0; n.in_size=0;
  n.dest_addr=logaddr; n.src_addr=srcaddr;
  n.key=key;

  st=rmap_put_word(sw_fd,port,&n,add,data);
  if (st<0) printf("RMAP Error\n");

  sw_close(sw_fd);
  exit(0);
}

int conv_int_bit4(int in){
  int out;
  out=(in&1)+((in>>1)&1)*10+((in>>2)&1)*100+((in>>3)&1)*1000;
  return(out);
}
