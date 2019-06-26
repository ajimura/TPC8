#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef PCIE
#include "swpci_lib.h"
#else
#include "swsoc_lib.h"
#endif
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
  add=0x2100; data=LinkDN|LinkUP;
  st =rmap_throw_word(sw_fd,port,&n,add,data);
  if (st<0) printf("RMAP Error\n");
  sw_close(sw_fd);
  exit(0);
}
