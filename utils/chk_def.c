#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include "tpclib.h"

int main(int argc, char *argv[]) {

  int NumFADC[8];
  int i;

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

  // set adc info
  fadc_set_adc_info();

  // initialize adc
  fadc_check_default_value();

  fadc_close();
  exit(0);
}
