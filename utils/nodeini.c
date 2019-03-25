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
  int NumFADC[8];
  int st;
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
  if (sw_open_check(NumFADC)<0) exit(0);

  for(i=0;i<8;i++)
    if (NumFADC[i]>0)
      for(j=0;j<NumFADC[i];j++){
	st=fadc_node_init(i, j);
      }

  fadc_close();
  exit(0);
}
