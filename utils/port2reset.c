#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "swsoc_lib.h"
#include "rmap_lib.h"

#define LinkUP 0x00000500
#define LinkDN 0x00000800

unsigned int key=0x02;
unsigned int srcaddr=0x80;
int port;

struct rmap_node_info n;

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
  sw_fd=sw_open(port);
  if (sw_link_test(sw_fd,port)){
    printf("Link #%d is not ready\n",port); exit(0);
  }

  // set node info.
  n.out_size=0; n.in_size=0;
  n.dest_addr=logaddr; n.src_addr=srcaddr;
  n.key=key;

  /* link reset port2 */
  add=0x2200; data=LinkDN;
  st=rmap_put_word(sw_fd,port,&n,add,data);
  add=0x2200; data=LinkUP;
  st=rmap_put_word(sw_fd,port,&n,add,data);

  sw_close(sw_fd);
  exit(0);
}

int conv_int_bit4(int in){
  int out;
  out=(in&1)+((in>>1)&1)*10+((in>>2)&1)*100+((in>>3)&1)*1000;
  return(out);
}
