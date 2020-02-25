#include <stdio.h>
#include <stdlib.h>
#include "tpclib.h"

int main(int argc, char *argv[]) {

  int NumFADC[8];
  int st;
  int i,j;
  int end;

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
	if (j==(NumFADC[i]-1)) end=-1;
	else end=1;
	if ((st=fadc_node_init(i,j,end))<0){
	  printf("Errora at %d-%d\n",i,j);
	  exit(-1);
	}
      }

  fadc_close();
  exit(0);
}
