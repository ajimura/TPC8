#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "tpclib.h"
#include "tpcmath.h"
#include "address.h"

unsigned int FadcNum4Header0;
unsigned int FadcNum4Header1;

int main(int argc, char *argv[]) {
  int NumFADC[8];
  int fadc_totnum;
  //  int st;
  int i;
  //  unsigned int rdata[2+(257*16+5)*10];
  unsigned int *rdata;
  int totsize;
  int outfile;
  int input;
  int cmptype;
  int trigenab;
  int thres;
  int maxloop;
  /* stat */
  int datanum=0;
  double mean, sigma2;
  /* for time */
  struct timespec ts0, ts1, ts2, ts3;
  double t0,t1,t2,t3;
  int seqnum=0;

  for(i=0;i<8;i++) NumFADC[i]=0;
  if (argc<2){
    printf("few argument...\n");
    exit(-1);
  }
  for(i=1;i<argc;i++) sscanf(argv[i],"%d",&(NumFADC[i-1]));

  // open space-wire port and check link status
  if ((fadc_totnum=sw_open_check(NumFADC))<0) exit(-1);
  //  printf("TotNum: %d\n",fadc_totnum);
  FadcNum4Header0=NumFADC[0]+256*(NumFADC[1]+256*(NumFADC[2]+256*NumFADC[3]));
  FadcNum4Header1=NumFADC[4]+256*(NumFADC[5]+256*(NumFADC[6]+256*NumFADC[7]));
  rdata=(unsigned int *)malloc((2+(257*16+5)*fadc_totnum)*4);

  printf("Input max loop number: ");
  scanf("%d",&maxloop);

  printf("Trig Ext(0), Clock(1): ");
  scanf("%d",&input);
  if (input==0) trigenab=FADC_ExtTrig;
  else if (input==1) trigenab=FADC_ClkTrig;
  //  else trigenab=FADC_IntTrig;

  printf("Comp type: Simple Zero Sup.(0), PDS(1), P(2), Q(3), R(4): ");
  scanf("%d",&input);
  if (input==0) cmptype=FADC_Sup; //0x0010;
  else if (input==1) cmptype=FADC_Peak|FADC_Dip|FADC_Mark; //0x000f;
  else if (input==2) cmptype=FADC_Peak; //0x0008;
  else if (input==3) cmptype=FADC_Peak2; //0x0020;
  else if (input==4) cmptype=FADC_Peak3; //0x0040;
  else cmptype=0x0808;

  printf("Read pedestal.txt for threshold? (yes=1): ");
  scanf("%d",&input);
  if (input==1) thres=1; else thres=0;

  // set adc info
  fadc_set_adc_info();
  // setup adc card
  fadc_setup_all();
  // set threshold
  fadc_set_thres_zero();
  if (thres==1) fadc_read_thres("pedestal.txt",100);
  // set compression type and threshold
  fadc_set_comp_all(cmptype);

  //Set TrigIO
  fadc_enable_localtrig_all();
  fadc_read_set_trigio("trigio.txt");
  fadc_read_readychk("readychk.txt");

  // open output file
  if ((outfile=open("output.dat",O_CREAT|O_WRONLY|O_TRUNC,S_IRWXU))<0){
    printf("Cannot open file...\n"); exit(0);
  }

  fadc_reset_trigcount();

  //TGC TrigEnable
  fadc_trig_enable(trigenab);

  while(maxloop>0){

    // wait trigger
    
    //    if (fadc_wait_data_ready_sel()<0){
    if (fadc_wait_data_ready_all()<0){
      printf("Timeout...\n");
      fadc_close();
      exit(0);
    }

    clock_gettime(CLOCK_MONOTONIC,&ts0);
    t0=(ts0.tv_sec*1.)+(ts0.tv_nsec/1000000000.);

    //get data size
    fadc_get_totsizeM2();

    clock_gettime(CLOCK_MONOTONIC,&ts1);
    t1=(ts1.tv_sec*1.)+(ts1.tv_nsec/1000000000.);

    // get data
    totsize=fadc_get_event_dataM2(rdata+3, seqnum);
    *rdata=totsize+16;
    *(rdata+1)=FadcNum4Header0;
    *(rdata+2)=FadcNum4Header1;
    *(rdata+3+totsize/4)=0xf0f0f0f0;

    clock_gettime(CLOCK_MONOTONIC,&ts2);
    t2=(ts2.tv_sec*1.)+(ts2.tv_nsec/1000000000.);

    // flush data buf
    // fadc_release_buffer();

    clock_gettime(CLOCK_MONOTONIC,&ts3);
    t3=(ts3.tv_sec*1.)+(ts3.tv_nsec/1000000000.);

    printf("%4d: %lf %lf %lf %lf\n",seqnum,t3,t3-t2,t2-t1,t1-t0);

    //    printf("%lf\n",t0);
    //    printf("%lf %lf\n",t1,t1-t0);
    //    printf("%lf %lf %lf\n",t2,t2-t1,t2-t0);
    //    printf("%lf %lf %lf %lf\n",t3,t3-t2,t3-t1,t3-t0);

    fadc_calc_sigmaD(datanum++,t3-t0,&mean,&sigma2);

    //increment next
    fadc_prepare_next();

    write(outfile, rdata, totsize+16);

    maxloop--;
    seqnum++;
  }

  printf("Stat: %d, %lf, %lf\n", datanum, mean, sqrt(sigma2));

  //TGC TrigDisable
  fadc_trig_disable();

  close(outfile);
  fadc_close();
  exit(0);
}
