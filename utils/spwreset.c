#include <stdio.h>
#include <unistd.h>
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

  int fd[8];

  int NumFADC[8];

  int i,j;
  unsigned int add, data;
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

#ifdef PCIE
  fd[0]=sw_open(0);
  for(i=1;i<8;i++) fd[i]=fd[0];
#else
  for(i=0;i<8;i++)
    fd[i]=sw_open(i);
#endif

  for(i=0;i<8;i++){
    if (NumFADC[i]>0){
      for(j=0;j<NumFADC[i];j++){
	printf("resetting %d-%d...\n",i,j);
	logaddr=j+32;
	// set node info.
	n.out_size=0; n.in_size=0;
	n.dest_addr=logaddr; n.src_addr=srcaddr;
	n.key=key;
	/* link reset port1 */
	add=0x2100; data=LinkDN|LinkUP;
	st =rmap_throw_word(fd[i],i,&n,add,data);
	if (st<0){ printf("RMAP Error(1)\n"); exit(-1);}
	usleep(5000);
	/* link reset port3 */
	add=0x2300; data=LinkDN;
	st =rmap_put_word(fd[i],i,&n,add,data);
	add=0x2300; data=LinkUP;
	st +=rmap_put_word(fd[i],i,&n,add,data);
	if (st<0){ printf("RMAP Error(3) %d\n",st); exit(-1);}
	/* link reset port2 */
	add=0x2200; data=LinkDN;
	st =rmap_put_word(fd[i],i,&n,add,data);
	add=0x2200; data=LinkUP;
	st+=rmap_put_word(fd[i],i,&n,add,data);
	if (st<0){ printf("RMAP Error(2)\n"); exit(-1);}
      }
    }
  }
  for(i=0;i<8;i++) sw_close(fd[i]);
  exit(0);
}
