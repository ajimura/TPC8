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
#include "address.h"

#define NumEvtBuffer 3

unsigned int key=0xcc;
unsigned int srcaddr=0x80;
int port;

struct rmap_node_info n;

int main(int argc, char *argv[]) {

  int sw_fd;

  unsigned int add, data=0;
  unsigned int nodeid;
  unsigned int logaddr;
  int st;

  printf("Port#? ");
  scanf("%d",&port);

  //  printf("Logical Address? ");
  //  scanf("%d",&logaddr);
  printf("Node ID? ");
  scanf("%d",&nodeid);
  logaddr=nodeid+160;

  // set node info.
  n.out_size=0; n.in_size=0;
  n.dest_addr=logaddr; n.src_addr=srcaddr;
  n.key=key;

  // open
  sw_fd=sw_open(port);
  if (sw_link_test(sw_fd,port)){
    printf("Link #%d is not ready\n",port); exit(0);
  }

  add=CMN_HardRst;
  st=rmap_put_word(sw_fd,port,&n,add,data);
  if (st<0) printf("RMAP Error\n");
  sw_close(sw_fd);
  exit(0);
}
