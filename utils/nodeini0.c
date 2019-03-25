#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "rmap_lib.h"
#include "tpclib.h"

int main(int argc, char *argv[]) {

  int port;
  int node;
  int NumFADC[8];
  int st;
  int i,j;

  if (argc<2){
    printf("few argument...\n");
    exit(-1);
  }else{
    sscanf(argv[1],"%d",&port);
    sscanf(argv[2],"%d",&node);
  }

  for(i=0;i<8;i++) NumFADC[i]=0;
  NumFADC[port]=1;

  // open
  if (sw_open_check(NumFADC)<0) exit(0);

  st=fadc_node_init(port, node);

  fadc_close();
  exit(0);
}
