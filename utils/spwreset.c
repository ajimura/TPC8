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

  int NumFADC[8];

  unsigned int add, data;
  unsigned int nodeid;
  unsigned int logaddr;
  int st;

  for(i=0;i<8;i++) NumFADC[i]=0;
  if (argc<2){
    printf("few argument...\n");
    exit(-1);
  }
  for(i=1;i<argc;i++){
    sscanf(argv[i],"%d",&(NumFADC[i-1]));
  }

  // open
  if (sw_open_check(NumFADC)<0) exit(0);

  for(i=0;i<8;i++){
    if (NumFADC[i]>0){
      for(j=0;j<NumFADC[i];j++){
	logaddr=j+32;
	// set node info.
	n.out_size=0; n.in_size=0;
	n.dest_addr=logaddr; n.src_addr=srcaddr;
	n.key=key;
	/* link reset port1 */
	//	add=0x2100; data=LinkDN|LinkUP;
	//	st =rmap_put_word(sw_fd,port,&n,add,data);
	//	add=0x2100; data=LinkUP;
	//	st+=rmap_put_word(sw_fd,port,&n,add,data);
	//	if (st<0){ printf("RMAP Error\n"); exit(-1);}
	/* link reset port3 */
	add=0x2300; data=LinkDN;
	st =rmap_put_word(sw_fd,port,&n,add,data);
	add=0x2300; data=LinkUP;
	st+=rmap_put_word(sw_fd,port,&n,add,data);
	if (st<0){ printf("RMAP Error\n"); exit(-1);}
	/* link reset port2 */
	add=0x2200; data=LinkDN;
	st =rmap_put_word(sw_fd,port,&n,add,data);
	add=0x2200; data=LinkUP;
	st+=rmap_put_word(sw_fd,port,&n,add,data);
	if (st<0){ printf("RMAP Error\n"); exit(-1);}
      }
    }
  }
  fadc_close();
  exit(0);
}
