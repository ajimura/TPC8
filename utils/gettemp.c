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
#include "tpclib.h"
#include "address.h"

int main(int argc, char *argv[]) {

  int NumFADC[8];
  int fadc_totnum;

  unsigned int add, data;
  int st;
  int i,j;
  int numfadc;
  int port, nodeid, temp;

  for(i=0;i<8;i++) NumFADC[i]=0;

  if (argc<2){
    printf("few argument...\n");
    exit(-1);
  }
  for(i=1;i<argc;i++) sscanf(argv[i],"%d",&(NumFADC[i-1]));

  // open space-wire port and check link status
  if ((fadc_totnum=sw_open_check(NumFADC))<0) exit(-1);

  // set adc info
  printf("fadc_set_adc_info ...\n");
  fadc_set_adc_info();

  //Init TMP103
  printf("fadc_init_temp ...\n");
  fadc_init_temp();

  //Get Temparature
  for(i=0;i<8;i++){
    for(j=0;j<NumFADC[i];j++){
      if (fadc_get_temp_each(i,j,&temp)<0){
	printf("invalid port/nodeid\n");
      }else{
	printf("Port#%d Node#%d: %d\n",i,j,temp);
      }
    }
  }

  fadc_close();
  exit(0);
}
