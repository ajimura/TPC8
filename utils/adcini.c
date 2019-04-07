#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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

  // reset fadc
  fadc_hardreset_all();

  // initialize adc
  fadc_init_all_adc();

  usleep(100000);

  fadc_close();
  exit(0);
}
