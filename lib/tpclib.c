/* by Shuhei Ajimura */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include "swsoc_lib.h"
#include "rmap_lib.h"
#include "address.h"

#define SW_Port0 0x01000000
#define SW_Port1 0x02000000
#define SW_Port2 0x04000000
#define SW_Port3 0x08000000

#define FADC_Zero 0x00000101
#define FADC_Mark 0x00000202
#define FADC_Dip 0x00000404
#define FADC_Peak 0x00000808
#define FADC_Sup 0x00001010

#define FADC_ExtGTrigIn 0x00000010
#define FADC_ExtRstIn 0x00000020
#define FADC_ExtLTrigIn 0x00000040
#define FADC_ExtBusyIn 0x00000080

#define FADC_TrigOut 0x00000004
#define FADC_BsyOut  0x00000008

#define DevsNum 8
#define NumEvtBuffer 3

#define LinkUP 0x00000500
#define LinkDN 0x00000800


struct fadc_info {
  int port;
  int nodeid;
  struct rmap_node_info node;
  unsigned short thres[16];
  unsigned int size[16];
  unsigned int totsize;
  unsigned int tgcreg[3];
  unsigned int *nextptr;
  int next;
  int roc;
};

int readychk_port[16], readychk_node[16], readynum;

void fadc_set_logaddr(struct rmap_node_info *, int, int, int);
int fadc_softreset_each(struct fadc_info *);
int fadc_hardreset_each(struct fadc_info *);
int fadc_init_each_adc(struct fadc_info *);
int fadc_setup_each(struct fadc_info *);
int fadc_reset_trigcount_each(struct fadc_info *);
int fadc_set_comp_each(struct fadc_info *, int);
int fadc_disable_localtrig_each(struct fadc_info *);
int fadc_enable_localtrig_each(struct fadc_info *);
int fadc_trig_enable_each(struct fadc_info *, int);
int fadc_trig_disable_each(struct fadc_info *);
int fadc_show_buf_stat(int);
int fadc_get_data_size_each(struct fadc_info *);
int fadc_get_totsize_each(struct fadc_info *);
int fadc_get_data_each(struct fadc_info *, unsigned int *);
int fadc_get_data_eachM(struct fadc_info *, unsigned int *);
int fadc_release_buffer_each(struct fadc_info *);
void fadc_prepare_next_each(struct fadc_info *);
int fadc_init_temp_each(struct fadc_info *);
int fadc_check_default_value_each(struct fadc_info *);
int fadc_set_default_value_each(struct fadc_info *);

struct fadc_info *fadcinfo[DevsNum];
int fadc_num[DevsNum], fadc_tot;

unsigned int FadcNum4Header0;
unsigned int FadcNum4Header1;

int sw_fd[DevsNum];

char devname[DevsNum][16]={"/dev/swsoc0","/dev/swsoc1","/dev/swsoc2","/dev/swsoc3",
		     "/dev/swsoc4","/dev/swsoc5","/dev/swsoc6","/dev/swsoc7"};

int sw_open_check(int *NumFADC){
  int i;

  if (NumFADC[0]>93 || NumFADC[0]>93 || NumFADC[0]>93 || NumFADC[0]>93 ||
      NumFADC[4]>93 || NumFADC[5]>93 || NumFADC[6]>93 || NumFADC[7]>93) return -1;
  fadc_tot=NumFADC[0]+NumFADC[1]+NumFADC[2]+NumFADC[3]+NumFADC[4]+NumFADC[5]+NumFADC[6]+NumFADC[7];
  for(i=0;i<DevsNum;i++) fadc_num[i]=NumFADC[i];
  //  printf("We have %d %d %d %d boards\n",fadc_num[0],fadc_num[1],fadc_num[2],fadc_num[3]);
  FadcNum4Header0=NumFADC[0]+256*(NumFADC[1]+256*(NumFADC[2]+256*NumFADC[3]));
  FadcNum4Header1=NumFADC[4]+256*(NumFADC[5]+256*(NumFADC[6]+256*NumFADC[7]));

  for(i=0;i<DevsNum;i++){
    if (NumFADC[i]>0){
      //      printf("open %s ...\n",devname[i]);
      sw_fd[i]=open(devname[i],O_RDWR);
      //      sw_link_reset(sw_fd[i],i); usleep(500);
      //      sw_rx_flush(sw_fd[i],i); usleep(500);
      //      sw_link_reset(sw_fd[i],i); usleep(500);
      //      sw_rx_flush(sw_fd[i],i); usleep(500);
      //      sw_print_status(sw_fd[i],i);
      if (sw_link_check(sw_fd[i],i)){
	printf("Link #%d is not ready\n",i);
	return -1;
      }
    }
  }
  return fadc_tot;
}

void fadc_close(){
  int i;
  for(i=0;i<DevsNum;i++){
    if (fadc_num[i]>0){
      sw_close(sw_fd[i]);
    }
  }
}

void fadc_set_adc_info(){
  int i,j;

  for(i=0;i<DevsNum;i++)
    fadcinfo[i]=(struct fadc_info *)malloc(sizeof(struct fadc_info)*fadc_num[i]);

  for(i=0;i<DevsNum;i++){
    if (fadc_num[i]>0){
      for(j=0;j<fadc_num[i];j++){
	(fadcinfo[i]+j)->port=i;
	(fadcinfo[i]+j)->nodeid=j;
	fadc_set_logaddr(&(fadcinfo[i]+j)->node,0xA0+j,0x80,0xcc);
      }
    }
  }
}

void fadc_set_logaddr(struct rmap_node_info *n,
		 int destaddr, int srcaddr, int key) {
  n->dest_addr=destaddr&0xff;
  n->src_addr=srcaddr&0xff;
  n->key=key&0xff;
  n->out_size=0;
  n->in_size=0;
}

int fadc_softreset_all(){
  int i,j;
  int st;
  struct fadc_info *adc;
  st=0;
  for(i=0;i<DevsNum;i++){
    adc=fadcinfo[i];
    for(j=0;j<fadc_num[i];j++){
      st+=fadc_softreset_each(adc+j);
    }
  }
  if (st<0) return -1; else return 0;
}
  
int fadc_softreset_each(struct fadc_info *adc){
  unsigned int add, data;
  int st;
  add=CMN_Reset; data=0;
  st=rmap_put_word(sw_fd[adc->port],adc->port,&(adc->node),add,data);
  return st;
}

int fadc_hardreset_all(){
  int i,j;
  int st;
  struct fadc_info *adc;
  st=0;
  for(i=0;i<DevsNum;i++){
    adc=fadcinfo[i];
    for(j=0;j<fadc_num[i];j++){
      st+=fadc_hardreset_each(adc+j);
    }
  }
  if (st<0) return -1; else return 0;
}
  
int fadc_hardreset_each(struct fadc_info *adc){
  unsigned int add, data;
  int st;
  add=CMN_HardRst; data=0;
  st=rmap_put_word(sw_fd[adc->port],adc->port,&(adc->node),add,data);
  return st;
}

int fadc_init_all_adc(){
  int i,j;
  int st;
  struct fadc_info *adc;
  st=0;
  for(i=0;i<DevsNum;i++){
    adc=fadcinfo[i];
    for(j=0;j<fadc_num[i];j++){
      st+=fadc_init_each_adc(adc+j);
    }
  }
  if (st<0) return -1; else return 0;
}

int fadc_init_each_adc(struct fadc_info *adc){

  unsigned int add, data;
  int st;
  int div=0,phase=0,phase1=3;

  add=ASC+0x020; data=0x03;
  st=rmap_put_word(sw_fd[adc->port],adc->port,&(adc->node),add,data);
  //  sw_print_status(sw_fd[adc->port],adc->port);
  add=ASC+0x020; data=0x00;
  st+=rmap_put_word(sw_fd[adc->port],adc->port,&(adc->node),add,data);
  //  sw_print_status(sw_fd[adc->port],adc->port);
  add=ASC+0x000; data=0x3c;
  st+=rmap_put_word(sw_fd[adc->port],adc->port,&(adc->node),add,data);
  //  sw_print_status(sw_fd[adc->port],adc->port);
  add=ASC+0x050; data=0x40;
  st+=rmap_put_word(sw_fd[adc->port],adc->port,&(adc->node),add,data);
  //  sw_print_status(sw_fd[adc->port],adc->port);

  add=ASC+0x02c; data=div;
  st+=rmap_put_word(sw_fd[adc->port],adc->port,&(adc->node),add,data);
  //  sw_print_status(sw_fd[adc->port],adc->port);
  add=ASC+0x058; data=phase1+phase*16;
  st+=rmap_put_word(sw_fd[adc->port],adc->port,&(adc->node),add,data);
  //  sw_print_status(sw_fd[adc->port],adc->port);
  add=ASC+0x424; data=0x03;
  st+=rmap_put_word(sw_fd[adc->port],adc->port,&(adc->node),add,data);
  //  sw_print_status(sw_fd[adc->port],adc->port);
  add=ASC+0x808; data=0x01;
  st+=rmap_put_word(sw_fd[adc->port],adc->port,&(adc->node),add,data);
  //  sw_print_status(sw_fd[adc->port],adc->port);

  return st;
}

int fadc_node_init(int port, int nodeid){
  int key0=0x02;
  int key3=0xcc;
  int logaddr;
  int srcaddr=0x80;

  unsigned int add, data, ret;
  int st;
  int i,j;
  struct rmap_node_info n,n3;

  // set node info
  logaddr=0xfe;
  fadc_set_logaddr(&n, logaddr, srcaddr, key0);

  // set routing for 0x20+nodeid
  add=(0x20+nodeid)*4; data=SW_Port0;
  st=rmap_put_word(sw_fd[port],port,&n,add,data);

  // set logical address to 0x20+nodeid 
  add=0x090c;
  st=rmap_get_data(sw_fd[port],port,&n,add,&ret,4);
  data=(ret&0xff00ffff)|((nodeid+0x20)<<16);
  st=rmap_put_word(sw_fd[port],port,&n,add,data);
  logaddr=0x20+nodeid;

  // set node info
  fadc_set_logaddr(&n, logaddr, srcaddr, key0);

  // set routing for 0xa0+nodeid 
  add=(0xa0+nodeid)*4; data=SW_Port3;
  st=rmap_put_word(sw_fd[port],port,&n,add,data);

  // set routing for 0x21+nodeid to 0x7c
  for(i=0x21+nodeid;i<0x7d;i++){
    add=i*4; data=SW_Port2;
    st=rmap_put_word(sw_fd[port],port,&n,add,data);
  }
  // set routing for 0xa1+nodeid to 0xfc
  for(i=0xa1+nodeid;i<0xfd;i++){
    add=i*4; data=SW_Port2;
    st=rmap_put_word(sw_fd[port],port,&n,add,data);
  }
  // set routing for 0x80
  add=0x80*4; data=SW_Port1;
  st=rmap_put_word(sw_fd[port],port,&n,add,data);

  /* set routing for 0xfd */
  //  add=0xfd*4; data=SW_Port3;
  //  st=rmap_put_word(sw_fd[port],port,&n,add,data);

  /* link reset port2 */
  //  add=0x2200; data=LinkDN; //0x00080000;
  //  st=rmap_put_word(sw_fd[port],port,&n,add,data);
  //  add=0x2200; data=LinkUP; //0x00050000;
  //  st=rmap_put_word(sw_fd[port],port,&n,add,data);

  /* show port2 status */
  add=0x2200;
  st=rmap_get_data(sw_fd[port],port,&n,add,&ret,4);
  printf("%d-%d: port2(%08x) ",port,nodeid,ret);

  /* show port3 status */
  add=0x2300;
  st=rmap_get_data(sw_fd[port],port,&n,add,&ret,4);
  printf("port3(%08x)\n",ret);

  /* link reset port3 */
  //  add=0x2300; data=LinkDN; //0x00000800;
  //  st=rmap_put_word(sw_fd[port],port,&n,add,data);
  //  add=0x2300; data=LinkUP; //0x00000500;
  //  st=rmap_put_word(sw_fd[port],port,&n,add,data);

  /* show port3 status */
  //  add=0x2300;
  //  st=rmap_get_data(sw_fd[port],port,&n,add,&ret,4);
  //  printf("(%08x)\n",ret);

  /* set node info for port 3 */
  //  fadc_set_logaddr(&n3,0xfd,srcaddr,key3);

  /* set logaddress on port#3 */
  //  add =CMN_LogAddr;
  //  data=0xa0+nodeid;
  //  st=rmap_put_word(sw_fd[port],port,&n3,add,data);

  /* set routing for 0xfe, 0xfd */
  add=0xfe*4; data=SW_Port2;
  st=rmap_put_word(sw_fd[port],port,&n,add,data);
  add=0xfd*4; data=SW_Port2;
  st=rmap_put_word(sw_fd[port],port,&n,add,data);

}

int fadc_node_def(int port, int nodeid){
  int key0=0x02;
  int key3=0xcc;
  int logaddr;
  int srcaddr=0x80;

  unsigned int add, data, ret;
  int st;
  int i,j;
  struct rmap_node_info n,n3;

  // set node info
  logaddr=nodeid+0x20;
  fadc_set_logaddr(&n, logaddr, srcaddr, key0);

  // set routing
  for(i=0x20;i<0x20+nodeid;i++){
    add=i*4; data=0;
    st=rmap_put_word(sw_fd[port],port,&n,add,data);
  }
  for(i=0x20+nodeid+1;i<0x80;i++){
    add=i*4; data=0;
    st=rmap_put_word(sw_fd[port],port,&n,add,data);
  }
  for(i=0x81;i<0xfe;i++){
    add=i*4; data=0;
    st=rmap_put_word(sw_fd[port],port,&n,add,data);
  }
  add=0xfe*4; data=SW_Port0;
  st=rmap_put_word(sw_fd[port],port,&n,add,data);
  // set logical address to 0xfe
  add=0x090c;
  st=rmap_get_data(sw_fd[port],port,&n,add,&ret,4);
  data=(ret&0xff00ffff)|((0xfe)<<16);
  st=rmap_put_word(sw_fd[port],port,&n,add,data);

  // set node info
  logaddr=0xfe;
  fadc_set_logaddr(&n, logaddr, srcaddr, key0);

  add=(0x20+nodeid)*4; data=0;
  st=rmap_put_word(sw_fd[port],port,&n,add,data);
  add=0x80*4; data=0;
  st=rmap_put_word(sw_fd[port],port,&n,add,data);

}

int fadc_setup_all(){
  int i,j;
  int st;
  struct fadc_info *adc;
  st=0;
  for(i=0;i<DevsNum;i++){
    adc=fadcinfo[i];
    for(j=0;j<fadc_num[i];j++){
      st+=fadc_setup_each(adc+j);
    }
  }
  if (st<0) return -1; else return 0;
}

int fadc_setup_each(struct fadc_info *adc){
  unsigned int add, data;
  int st;

  //TGC TrigDisable
  fadc_trig_disable_each(adc);

  //flush
  add=ROC_Done;  data=0x0f;
  st=rmap_put_word(sw_fd[adc->port],adc->port,&(adc->node),add,data);
  add=ROC_Done;  data=0x00;
  st+=rmap_put_word(sw_fd[adc->port],adc->port,&(adc->node),add,data);

  //Get NextBuf ID -> next
  add=TGC_NextBuf;
  st+=rmap_get_data(sw_fd[adc->port],adc->port,&(adc->node),add,&(adc->next), 4);
  //  printf("NextBuf: %d-%d %d\n",adc->port,adc->nodeid,adc->next);
  return st;
}

void fadc_set_thres_zero(){
  int i,j,k;
  struct fadc_info *adc;

  for(i=0;i<DevsNum;i++){
    adc=fadcinfo[i];
    for(j=0;j<fadc_num[i];j++){
      for(k=0;k<16;k++){
	(adc+j)->thres[k]=0;
      }
    }
  }
}

int fadc_read_thres(const char *filename, int factor){
  FILE *infile;
  char line[100];
  int i,j;
  int port, node, ch;
  double thres, rms;

  if ((infile=fopen(filename,"r"))==NULL){
    printf("file not find...\n");
    return -1;
  }

  //  printf("***** pedestal ******\n");
  while(fgets(line,100,infile)!=NULL){
    sscanf(line,"%d %d %d %lf %lf",&port,&node,&ch,&thres,&rms);
    for(i=0;i<DevsNum;i++){
      for(j=0;j<fadc_num[i];j++){
	if ((fadcinfo[i]+j)->port==port && (fadcinfo[i]+j)->nodeid==node){
	  //	  (fadcinfo[i]+j)->thres[ch]=(int)(thres+rms*10.);
	  (fadcinfo[i]+j)->thres[ch]=(int)(thres+rms*factor);
	  //	printf("%d %d %d %f %f\n",port,node,ch,thres,rms);
	}
      }
    }
  }
  fclose(infile);
  return 0;
}

int fadc_reset_trigcount(){
  int i,j;
  struct fadc_info *adc;
  for(i=0;i<DevsNum;i++){
    adc=fadcinfo[i];
    for(j=0;j<fadc_num[i];j++){
      if (fadc_reset_trigcount_each(adc+j)<0) return -1;
    }
  }
  return 0;
}

int fadc_reset_trigcount_each(struct fadc_info *adc){
  int add,data;
  int i;
  add=TGC_CntRst; data=0;
  if (rmap_put_word(sw_fd[adc->port],adc->port,&(adc->node),add,data)<0) return -1;
  for(i=0;i<NumEvtBuffer;i++){
    add=EBM_CntRst+BufBase*i; data=0;
    if (rmap_put_word(sw_fd[adc->port],adc->port,&(adc->node),add,data)<0) return -1;
  }
  return 0;
}

int fadc_set_comp_all(int cmptype){
  int i,j;
  int st;
  struct fadc_info *adc;
  st=0;
  for(i=0;i<DevsNum;i++){
    adc=fadcinfo[i];
    for(j=0;j<fadc_num[i];j++){
      st+=fadc_set_comp_each(adc+j,cmptype);
    }
  }
  if (st<0) return -1; else return 0;
}

int fadc_set_comp_each(struct fadc_info *adc, int cmptype){
  int i,j;
  int add,data,st;

  st=0;
  for(j=0;j<16;j++){
    add=EBM_Thres+ChBase*j; data=adc->thres[j];
    //    printf("add, data: %08x %08x\n",add,data);
    st+=rmap_put_word(sw_fd[adc->port],adc->port,&(adc->node),add,data);
  }
  add=EBM_CmpType; data=cmptype;
  //    printf("add, data: %08x %08x\n",add,data);
  st+=rmap_put_word(sw_fd[adc->port],adc->port,&(adc->node),add,data);
  return st;
}

int fadc_disable_localtrig_all(){
  int i, j, st;
  struct fadc_info *adc;
  st=0;
  for(i=0;i<DevsNum;i++){
  adc=fadcinfo[i];
    for(j=0;j<fadc_num[i];j++){
      st+=fadc_disable_localtrig_each(adc+j);
    }
  }
  if (st<0) return -1; else return 0;
}

int fadc_disable_localtrig_each(struct fadc_info *adc){
  int add, data, st;
  add=TGC_TrigInOut;
  data=FADC_ExtGTrigIn|FADC_ExtRstIn|FADC_TrigOut|FADC_BsyOut;
  st=rmap_put_word(sw_fd[adc->port],adc->port,&(adc->node),add,data);
  if (st<0) return -1; else return 0;
}

int fadc_enable_localtrig_all(){
  int i, j, st;
  struct fadc_info *adc;
  st=0;
  for(i=0;i<DevsNum;i++){
    adc=fadcinfo[i];
    for(j=0;j<fadc_num[i];j++){
      st+=fadc_enable_localtrig_each(adc+j);
    }
  }
  if (st<0) return -1; else return 0;
}

int fadc_enable_localtrig_each(struct fadc_info *adc){
  int add, data, st;
  add=TGC_TrigInOut;
  data=FADC_ExtGTrigIn|FADC_ExtRstIn|FADC_ExtBusyIn|FADC_ExtLTrigIn|FADC_TrigOut|FADC_BsyOut;
  st=rmap_put_word(sw_fd[adc->port],adc->port,&(adc->node),add,data);
  if (st<0) return -1; else return 0;
}

int fadc_read_set_trigio(const char *filename){
  FILE *infile;
  char line[100];
  int i,j;
  struct fadc_info *adc;
  int port, node;

  if ((infile=fopen(filename,"r"))==NULL){
    printf("file not find...\n");
    return -1;
  }

  if (fadc_enable_localtrig_all()<0){
    return -1;
  }

  while(fgets(line,100,infile)!=NULL){
    if (line[0]!='#'){
      sscanf(line,"%d %d",&port,&node);
      for(i=0;i<DevsNum;i++){
	adc=fadcinfo[i];
	for(j=0;j<fadc_num[i];j++){
	  if ((adc+j)->port==port && (adc+j)->nodeid==node){
	    printf("disable TrigIO on %d-%d\n",port,node);
	    if (fadc_disable_localtrig_each(adc+j)<0){
	      fclose(infile);
	      return -1;
	    }
	  }
	}
      }
    }
  }
  fclose(infile);
  return 0;
}

int fadc_read_readychk(const char *filename){
  FILE *infile;
  char line[100];
  int i,j;
  int port, node;

  if ((infile=fopen(filename,"r"))==NULL){
    printf("file not find...\n");
    return -1;
  }

  readynum=0;
  while(fgets(line,100,infile)!=NULL){
    if (line[0]!='#'){
      sscanf(line,"%d %d",&port,&node);
      if (port<DevsNum){
	if (node<fadc_num[port]){
	  readychk_port[readynum]=port;
	  readychk_node[readynum]=node;
	  readynum++;
	  printf("ready polling list: (%d) %d %d\n",readynum,port,node);
	}
      }
    }
  }
  fclose(infile);
  return 0;
}

int fadc_trig_enable(int trigenab){
  int i,j;
  int st;
  struct fadc_info *adc;
  st=0;
  for(i=0;i<DevsNum;i++){
    adc=fadcinfo[i];
    for(j=0;j<fadc_num[i];j++){
      st+=fadc_trig_enable_each(adc+j,trigenab);
    }
  }
  if (st<0) return -1; else return 0;
}

int fadc_trig_enable_each(struct fadc_info *adc, int trigenab){
  int add, data, st;

  add=TGC_TrigEnab; data=trigenab;
  st=rmap_put_word(sw_fd[adc->port],adc->port,&(adc->node),add,data);

  return st;
}

int fadc_trig_disable(){
  int i,j;
  int st;
  struct fadc_info *adc;
  st=0;
  for(i=0;i<DevsNum;i++){
    adc=fadcinfo[i];
    for(j=0;j<fadc_num[i];j++){
      st+=fadc_trig_disable_each(adc+j);
    }
  }
  if (st<0) return -1; else return 0;
}

int fadc_trig_disable_each(struct fadc_info *adc){
  int add, data, st;

  add=TGC_TrigEnab; data=0;
  st=rmap_put_word(sw_fd[adc->port],adc->port,&(adc->node),add,data);
  //  add=TGC_TrigInOut;
  //  data=FADC_TrigOut|FADC_BsyOut;
  //  st+=rmap_put_word(sw_fd,adc->port,&(adc->node),add,data);

  return st;
}

int fadc_wait_data_ready(){ // polling 1st fadc on port#0
  int add, data, st;
  int wait;
  int times;

  add=ROC_Ready;
  times=0;
  while(1){
    st=rmap_get_data(sw_fd[fadcinfo[0]->port],fadcinfo[0]->port,&(fadcinfo[0]->node),add,&data,4);
    if ((data>>fadcinfo[0]->next)&1) return 0;
    else{
      if (times++>15000) return -1;
      usleep(200);
    }
  }
}

int fadc_wait_data_ready_all(){
  int add, data, st;
  int i,j;
  int wait;
  int times;
  struct fadc_info *adc;

  add=ROC_Ready;
  times=0;
  while(1){
    for(i=0;i<DevsNum;i++){
      adc=fadcinfo[i];
      for(j=0;j<fadc_num[i];j++){
	st=rmap_get_data(sw_fd[(adc+j)->port],(adc+j)->port,&((adc+j)->node),add,&((adc+j)->roc),4);
	//	printf("ROC: %d-%d %08x\n",i,j,(adc+j)->roc);
      }
    }
    wait=0;
    for(i=0;i<DevsNum;i++){
      adc=fadcinfo[i];
      for(j=0;j<fadc_num[i];j++)
	if ((adc+j)->roc>>(adc+j)->next&1) wait++;
    }
    if (wait==fadc_tot) return 0;
    if (times++>15000) return -1;
    //    fadc_show_buf_stat(times);
    usleep(200);
  }
}

int fadc_wait_data_ready_sel(){
  int add, data, st;
  int i,j;
  int wait;
  int times;
  struct fadc_info *adc;
  int regready;
  int count;

  add=TGC_IOstat;
  times=0;
  while(1){
    st=0; count=0;
    for(i=0;i<readynum;i++){
      st=rmap_get_data(sw_fd[readychk_port[i]],readychk_port[i],&((fadcinfo[readychk_port[i]]+readychk_node[i])->node),
		       add,&(regready),4);
      //      printf("ROC#%d-%d: %08x\n",readychk_port[i],readychk_node[i],regready);
      //    printf("ROC: %08x %08X\n",adc->roc,(adc+1)->roc);
      //      printf("IOstat: %08x\n",regready);
      if ((regready&0x000000f0)>0) count++;
    }

    //    printf("%d %d\n",count, readynum);
    if (count==readynum) return 0;
    if (times++>25000) return -1;
    //    fadc_show_buf_stat(times);
    usleep(100);
  }
}

int fadc_show_buf_stat(int t){
  int add, data, st;
  unsigned int add0, add1, add2, add3;
  unsigned int data0, data1, data2, data3;
  int i,j;
  int wait;
  int times;
  struct fadc_info *adc;
  unsigned int roc;


  add0=TGC_IOstat;
  add1=TGC_FreeBuf;
  add2=TGC_NoEmpty;
  add3=TGC_Busy;
  for(i=0;i<DevsNum;i++){
    adc=fadcinfo[i];
    for(j=0;j<fadc_num[i];j++){
      st=rmap_get_data(sw_fd[(adc+j)->port],(adc+j)->port,&((adc+j)->node),add0,&data0,4);
      st=rmap_get_data(sw_fd[(adc+j)->port],(adc+j)->port,&((adc+j)->node),add1,&data1,4);
      st=rmap_get_data(sw_fd[(adc+j)->port],(adc+j)->port,&((adc+j)->node),add2,&data2,4);
      st=rmap_get_data(sw_fd[(adc+j)->port],(adc+j)->port,&((adc+j)->node),add3,&data3,4);
      printf("BufStat(%1d): %d-%d %08X %08X %08X %08X \n",t,(adc+j)->port,(adc+j)->nodeid,
	     data0, data1, data2, data3);
    }
  }
}

int fadc_get_data_size(){
  int i,j;
  int st;
  struct fadc_info *adc;
  st=0;
  for(i=0;i<DevsNum;i++){
    adc=fadcinfo[i];
    for(j=0;j<fadc_num[i];j++){
      st+=fadc_get_data_size_each(adc+j);
    }
  }
  if (st<0) return -1; else return 0;
}

int fadc_get_data_size_each(struct fadc_info *adc){
  int i,j;
  int add,data,st;

  //  printf("[%02d-%02d]",adc->port,adc->nodeid);
  st=0;
  for(j=0;j<16;j++){
    add=EBM_DataSize+ChBase*j+BufBase*(adc->next);
    st+=rmap_get_data(sw_fd[adc->port],adc->port,&(adc->node),add,&(adc->size[j]),4);
    //    printf("%3d(%04x) ",adc->size[j],adc->size[j]);
  } //printf("\n");
  return st;
}

int fadc_get_totsize(){
  int i,j;
  int count;
  int add,data,st;
  unsigned int tid,size;
  struct fadc_info *adc;

  st=0;  count=4;  j=0;
  while(count>0){
    printf("(1)%d\n",j);
    count=0;
    for(i=0;i<DevsNum;i++){
      if (fadc_num[i]>j){
	st+=fadc_get_totsize_each(fadcinfo[i]+j);
	count++;
      }
    }
    j++;
  }
  if (st<0) return -1; else return 0;
}

int fadc_get_totsize_each(struct fadc_info *adc){
  int i,j;
  int add,data,st;
  unsigned int tid,size;

  //  printf("[%02d-%02d]",adc->port,adc->nodeid);
  st=0;
  add=EBM_TotSize+BufBase*(adc->next);
  st+=rmap_req_data(sw_fd[adc->port],adc->port,&(adc->node),adc->port*100+adc->nodeid,add,4);
  st+=rmap_rcv_header(sw_fd[adc->port],adc->port,&tid,&size);
  st+=rmap_rcv_data(tid,&(adc->totsize)); // <==== must check
  //  printf("TotSize: %3d(%04x) %d\n",adc->totsize,adc->totsize,st);
  return st;
}

int fadc_get_totsizeM(){
  int i,j;
  int count;
  int add[4],data,st;
  unsigned int tid[4],size;
  struct fadc_info *adc;

  st=0;  count=4;  j=0;
  while(count>0){
    count=0;
    for(i=0;i<DevsNum;i++){
      if (fadc_num[i]>j){
	add[i]=EBM_TotSize+BufBase*((fadcinfo[i]+j)->next);
	tid[i]=i*100+(fadcinfo[i]+j)->nodeid;
	st+=rmap_req_data(sw_fd[(fadcinfo[i]+j)->port],(fadcinfo[i]+j)->port,&((fadcinfo[i]+j)->node),tid[i],add[i],4);
	count++;
      }
    }
    for(i=0;i<DevsNum;i++){
      if (fadc_num[i]>j){
	st+=rmap_rcv_header(sw_fd[(fadcinfo[i]+j)->port],(fadcinfo[i]+j)->port,&(tid[i]),&size);
	st+=rmap_rcv_data(tid[i],&((fadcinfo[i]+j)->totsize));
	//	printf("TotSize(%d-%d): %3d(%04x) %d\n",i,j,(fadcinfo[i]+j)->totsize,(fadcinfo[i]+j)->totsize,st);
      }
    }
    j++;
  }
  if (st<0) return -1; else return 0;
}

int fadc_get_totsizeM2(){
  int i,j;
  int count;
  int add,data,st;
  unsigned int tid,size;
  int portid, nodeid;
  struct fadc_info *adc;

  st=0; count=4; j=0;
  while(count>0){
    count=0;
    for(i=0;i<DevsNum;i++){
      if (j<fadc_num[i]){
	count++;
	add=EBM_TotSize+BufBase*((fadcinfo[i]+j)->next);
	tid=i*1000+j;
	st+=rmap_req_data(sw_fd[i],i,&((fadcinfo[i]+j)->node),tid,add,4);
	//	printf("rmap_req: %d-%d TID=%d\n",i,j,tid);
      }
    }
    for(i=0;i<DevsNum;i++){
      if (j<fadc_num[i]){
	st+=rmap_rcv_header(sw_fd[i],i,&tid,&size);
	portid=tid/1000;
	nodeid=tid%1000;
	if (portid==i && nodeid<fadc_num[i]){
	  st+=rmap_rcv_data(tid,&((fadcinfo[i]+j)->totsize));
	  //	  printf("TotSize(%d-%d): %3d(%04x) %d\n",
	  //		 i,j,(fadcinfo[i]+j)->totsize,(fadcinfo[i]+j)->totsize,st);
	}else{
	  printf("Wrong TID: %d %d (%d %d)\n",portid,nodeid,i,j);
	  return -1;
	}
      }
    }
    j++;
  }
  if (st<0) return -1; else return 0;
}

int fadc_get_event_data(unsigned int *rdata, int check){
  int i,j;
  unsigned int *curpos;
  struct fadc_info *adc;
  int totsize, eachsize;

  //  printf("start read data\n");

  curpos=rdata;

  curpos++; totsize=4;	// reserved for total size
  *(curpos++)=FadcNum4Header0; totsize+=4;	// fadc num
  *(curpos++)=FadcNum4Header1; totsize+=4;	// fadc num
  //  curpos++; totsize+=4;	// reserved

  for(i=0;i<DevsNum;i++){
    adc=fadcinfo[i];
    for(j=0;j<fadc_num[i];j++){
      if ((eachsize=fadc_get_data_each(adc+j,curpos))<0) return -1;
      if (check>=0){
	if (*(curpos+1)!=check){
	  printf("Event mismatch!\n");
	  printf("%d-%d: %d should be %d\n",*(curpos)>>16,*(curpos)&0xffff,*(curpos+1),check+1);
	  return -1;
	}
      }
      curpos+=(eachsize/4);
      totsize+=eachsize;
    }
  }
  *curpos=0xf0f0f0f0; totsize+=4;
  *rdata=totsize;
  return totsize;
}

int fadc_get_data_each(struct fadc_info *adc, unsigned int *rdata){
  int i,j;
  int add,data,st;
  int bufid;
  unsigned int *bufdata;
  unsigned int tcount, fclk,cclk;
  int retsize;

  bufdata=rdata; retsize=0;

  bufid=adc->next;

  //TGC Count
  add=TGC_TrigID+BufBase*bufid;
  st=rmap_get_data(sw_fd[adc->port],adc->port,&(adc->node),add,&tcount,4);
  //  printf("Get #%d Event ! port=%d node=%d bufid=%d\n",
  //	   tcount,adc->port,adc->nodeid,bufid);
  //Get TGC TimeStamp -> EBM TimeStamp
  add=TGC_FClk+BufBase*bufid;
  st+=rmap_get_data(sw_fd[adc->port],adc->port,&(adc->node),add,&fclk,4);
  add=TGC_CClk+BufBase*bufid;
  st+=rmap_get_data(sw_fd[adc->port],adc->port,&(adc->node),add,&cclk,4);
  //  printf("TimeStamp: %d %d\n",fclk,cclk);

  if (st<0) return -1;

  *bufdata++ = (adc->port<<16)|adc->nodeid; retsize+=4;
  *bufdata++ = tcount; retsize+=4;
  *bufdata++ = fclk; retsize+=4;
  *bufdata++ = cclk; retsize+=4;
  bufdata++; retsize+=4;	// reserved
  bufdata++; retsize+=4;	// reserved
  st=0;
  for(j=0;j<16;j++){
    //    *bufdata++ = (j<<16)|(adc->size[j]); retsize+=4;
    //    bufdata++; retsize+=4;	// reserved
    add=MemBase + ChBase*j + BufBase*bufid;
    st+=rmap_get_data(sw_fd[adc->port],adc->port,&(adc->node),add,bufdata,(adc->size[j]+1)/2*4);
    //    st+=rmap_get_dma(sw_fd[adc->port],adc->port,&(adc->node),add,bufdata,(adc->size[j]+1)/2*4);
    bufdata+=((adc->size[j]+1)/2); retsize+=((adc->size[j]+1)/2*4);
  }
  *bufdata++ = 0xffff0000; retsize+=4;
  if (st<0) return -1; else return retsize;
}

int fadc_get_event_dataM(unsigned int *rdata, int check){
  int i,j;
  unsigned int *curpos;
  struct fadc_info *adc;
  int totsize, eachsize;

  curpos=rdata;

  curpos++; totsize=4;	// reserved for total size
  *(curpos++)=FadcNum4Header0; totsize+=4;	// fadc num
  *(curpos++)=FadcNum4Header1; totsize+=4;	// fadc num
  //  curpos++; totsize+=4;	// reserved

  //  printf("start read data\n");

  for(i=0;i<DevsNum;i++){
    adc=fadcinfo[i];
    for(j=0;j<fadc_num[i];j++){
      if ((eachsize=fadc_get_data_eachM(adc+j,curpos))<0) return -1;
      if (check>=0){
	if (*(curpos+1)!=check){
	  printf("Event mismatch!\n");
	  printf("%d-%d: %d should be %d\n",*(curpos)>>16,*(curpos)&0xffff,*(curpos+1),check+1);
	  return -1;
	}
      }
      curpos+=(eachsize/4);
      totsize+=eachsize;
    }
  }
  *curpos=0xf0f0f0f0; totsize+=4;
  *rdata=totsize;
  return totsize;
}

int fadc_get_data_eachM(struct fadc_info *adc, unsigned int *rdata){
  int i,j;
  unsigned int add,data,st;
  int bufid;
  unsigned int *bufdata;
  unsigned int tcount, fclk,cclk;
  int retsize;
  unsigned int totsize;
  unsigned int tgc_reg[3];

  bufdata=rdata; retsize=0;

  bufid=adc->next;

  //TGC TrigID, FClk, CClk
  add=TGC_TrigID+BufBase*bufid;
  st=rmap_get_data(sw_fd[adc->port],adc->port,&(adc->node),add,tgc_reg,12);

  if (st<0) return -1;

  *bufdata++ = (adc->port<<16)|adc->nodeid; retsize+=4;
  memcpy(bufdata,tgc_reg,12); bufdata+=3; retsize+=12;
  bufdata++; retsize+=4;	// reserved
  bufdata++; retsize+=4;	// reserved
  st=0;
  /* get total size */
  totsize=adc->totsize;
  if (totsize>0x80){
    add=EBM + BufBase*bufid + 0x80000000;
    //#ifdef DMA
    //    st+=rmap_get_dma_W(sw_fd[adc->port],adc->port,&(adc->node),add,bufdata,(totsize+1)/2*4);
    //    //  printf("DMA!!\n");
    //#else
    st+=rmap_get_data_W(sw_fd[adc->port],adc->port,&(adc->node),add,bufdata,(totsize+1)/2*4);
    //  printf("no DMA!!\n");
    //#endif
    bufdata+=((totsize+1)/2); retsize+=((totsize+1)/2*4);
    *bufdata++ = 0xffff0000; retsize+=4;
    if (st<0) return -1; else return retsize;
  }else{
    //    fadc_release_buffer_each(adc);
  }
}

int fadc_get_event_dataM1(unsigned int *rdata, int check){
  int i,j;
  int curnode[DevsNum],nextnode[DevsNum];
  unsigned int tid,size;
  unsigned int *curpos;
  int portid, nodeid;
  int add,st;
  int count;
  struct fadc_info *adc;
  int totsize, eachsize;

  st=0;

  /* get TGC registers */
  count=4;
  for(i=0;i<DevsNum;i++) curnode[i]=0;
  //  for(i=0;i<4;i++) nextnode[i]=0;
  while(count>0){
    count=0;
    for(i=0;i<DevsNum;i++){
      for(j=curnode[i];j<fadc_num[i];j++){
	if ((fadcinfo[i]+j)->totsize>0x80){
	  count++;
	  nextnode[i]=j;
	  tid=i*1000+j;
	  add=TGC_TrigID+BufBase*((fadcinfo[i]+j)->next);
	  st+=rmap_req_data(sw_fd[i],i,&((fadcinfo[i]+j)->node),tid,add,12);
	  //	  printf("REQ: %d-%d\n",i,j);
	  break;
	}else{
	  nextnode[i]=fadc_num[i];
	  (fadcinfo[i]+j)->totsize=0;
	}
      }
    }
    if (count>0){
      for(i=0;i<DevsNum;i++){
	if (nextnode[i]<fadc_num[i]){
	  st+=rmap_rcv_header(sw_fd[i],i,&tid,&size);
	  //	  printf("RCVH: %d-%d size: %d(%x)\n",i,nextnode[i],size,size);
	  portid=tid/1000;
	  nodeid=tid%1000;
	  if (portid==i && nodeid<fadc_num[i]){
	    st+=rmap_rcv_data(tid,(fadcinfo[i]+nodeid)->tgcreg);
	    //	    printf("RCVD: %d-%d\n",i,nextnode[i]);
	    //	    printf("TrigID(%d-%d): %3d %x %x\n",
	    //		   portid,nodeid,
	    //		   (fadcinfo[i]+nodeid)->tgcreg[0],
	    //		   (fadcinfo[i]+nodeid)->tgcreg[1],
	    //		   (fadcinfo[i]+nodeid)->tgcreg[2]);
	  }else{
	    printf("Wrong TID: %d %d (%d %d)\n",portid,nodeid,i,j);
	    return -1;
	  }
	  curnode[i]=nextnode[i]+1;
	}
      }
    }
  }

  curpos=rdata;

  curpos++; totsize=4;	// reserved for total size
  *(curpos++)=FadcNum4Header0; totsize+=4;	// fadc num
  *(curpos++)=FadcNum4Header1; totsize+=4;	// fadc num
  //  curpos++; totsize+=4;	// reserved

  //  printf("start read data\n");

  for(i=0;i<DevsNum;i++){
    adc=fadcinfo[i];
    for(j=0;j<fadc_num[i];j++){
      if ((adc+j)->totsize>0){
	*(curpos++)=(i<<16)|j; totsize+=4;
	memcpy(curpos,(adc+j)->tgcreg,12); curpos+=3; totsize+=12;
	curpos++; totsize+=4;
	curpos++; totsize+=4;
	add=EBM+BufBase*(adc+j)->next+0x80000000;
	tid=i*1000+j;
	st+=rmap_req_data(sw_fd[i],i,&((adc+j)->node),tid,add,((adc+j)->totsize+1)/2*4);
	//	printf("REQ: %d-%d\n",i,j);
	st+=rmap_rcv_header(sw_fd[i],i,&tid,&size);
	//	printf("RCVH: %d-%d size: %d(%x)\n",i,nextnode[i],size,size);
	portid=tid/1000;
	nodeid=tid%1000;
	if (portid==i && nodeid<fadc_num[i]){
	  st+=rmap_rcv_data(tid,curpos);
	  if (check>=0){
	    if (*(curpos+1)!=check){
	      printf("Event mismatch!\n");
	      printf("%d-%d: %d should be %d\n",*(curpos)>>16,*(curpos)&0xffff,*(curpos+1),check+1);
	      return -1;
	    }
	  }
	  //	  printf("RCVD: %d-%d at %d(%x)\n",i,nextnode[i],curpos-rdata,curpos-rdata);
	}else{
	  printf("Wrong TID: %d %d (%d %d)\n",portid,nodeid,i,j);
	  return -1;
	}
	(adc+j)->nextptr=curpos+size/4;
	curpos+=((adc+j)->totsize+1)/2; totsize+=(((adc+j)->totsize+1)/2*4);
	(adc+j)->totsize-=size/2;
	*(curpos++)=0xffff0000; totsize+=4;
      }
    }
  }
  for(i=0;i<DevsNum;i++){
    adc=fadcinfo[i];
    for(j=0;j<fadc_num[i];j++){
      if ((adc+j)->totsize>0){
	add=EBM+BufBase*(adc+j)->next+0x80000000;
	tid=i*1000+j;
	st+=rmap_req_data(sw_fd[i],i,&((adc+j)->node),tid,add,((adc+j)->totsize+1)/2*4);
	//	printf("REQ: %d-%d\n",i,j);
	st+=rmap_rcv_header(sw_fd[i],i,&tid,&size);
	//	printf("RCVH: %d-%d size: %d(%x)\n",i,nextnode[i],size,size);
	portid=tid/1000;
	nodeid=tid%1000;
	if (portid==i && nodeid<fadc_num[i]){
	  st+=rmap_rcv_data(tid,(adc+j)->nextptr);
	  //	  printf("RCVD: %d-%d at %d(%x)\n",i,nextnode[i],(adc+j)->nextptr-rdata,(adc+j)->nextptr-rdata);
	}else{
	  printf("Wrong TID: %d %d (%d %d)\n",portid,nodeid,i,j);
	  return -1;
	}
      }
    }
  }
  *curpos=0xf0f0f0f0; totsize+=4;
  *rdata=totsize;
  return totsize;
}

int fadc_get_event_dataM2(unsigned int *rdata, int check){
  int i,j;
  int curnode[DevsNum],nextnode[DevsNum];
  unsigned int tid,size;
  unsigned int *curpos;
  int portid, nodeid;
  int add,st;
  int count;
  struct fadc_info *adc;
  int totsize, eachsize;

  st=0;

  /* get TGC registers */
  count=4;
  for(i=0;i<DevsNum;i++) curnode[i]=0;
  while(count>0){
    count=0;
    for(i=0;i<DevsNum;i++){
      nextnode[i]=fadc_num[i];
      for(j=curnode[i];j<fadc_num[i];j++){
	if ((fadcinfo[i]+j)->totsize>0x80){
	  count++;
	  nextnode[i]=j;
	  tid=i*1000+j;
	  add=TGC_TrigID+BufBase*((fadcinfo[i]+j)->next);
	  st+=rmap_req_data(sw_fd[i],i,&((fadcinfo[i]+j)->node),tid,add,12);
	  //	  printf("REQ: %d-%d\n",i,j);
	  break;
	}else{
	  (fadcinfo[i]+j)->totsize=0;
	}
      }
    }
    if (count>0){
      for(i=0;i<DevsNum;i++){
	if (nextnode[i]<fadc_num[i]){
	  st+=rmap_rcv_header(sw_fd[i],i,&tid,&size);
	  //	  printf("RCVH: %d-%d size: %d(%x)\n",i,nextnode[i],size,size);
	  portid=tid/1000;
	  nodeid=tid%1000;
	  if (portid==i && nodeid<fadc_num[i]){
	    st+=rmap_rcv_data(tid,(fadcinfo[i]+nodeid)->tgcreg);
	    //	    	    printf("RCVD: %d-%d\n",i,nextnode[i]);
	    //	    	    printf("TrigID(%d-%d): %3d %x %x\n",
	    //	    		   portid,nodeid,
	    //	    		   (fadcinfo[i]+nodeid)->tgcreg[0],
	    //	    		   (fadcinfo[i]+nodeid)->tgcreg[1],
	    //	    		   (fadcinfo[i]+nodeid)->tgcreg[2]);
	    if (check>=0){
	      if ((fadcinfo[i]+nodeid)->tgcreg[0]!=check){
		printf("Event mismatch!\n");
		printf("%d-%d: %d should be %d\n",
		       *(curpos)>>16,*(curpos)&0xffff,(fadcinfo[i]+nodeid)->tgcreg[0],check);
		return -1;
	      }
	    }
	  }else{
	    printf("Wrong TID: %d %d (%d %d)\n",portid,nodeid,i,j);
	    return -1;
	  }
	  curnode[i]=nextnode[i]+1;
	}
      }
    }
  }

  curpos=rdata;

  //now these header obsoleted(20181004)
  //  curpos++; totsize=4;	// reserved for total size
  //  *(curpos++)=FadcNum4Header0; totsize+=4;	// fadc num
  //  *(curpos++)=FadcNum4Header1; totsize+=4;	// fadc num
  totsize=0;

  //  printf("start read data\n");

  /* get event data */
  count=4;
  for(i=0;i<DevsNum;i++) curnode[i]=0;
  //  for(i=0;i<4;i++) nextnode[i]=fadc_num[i];
  while(count>0){
    count=0;
    for(i=0;i<DevsNum;i++){
      nextnode[i]=fadc_num[i];
      for(j=curnode[i];j<fadc_num[i];j++){
	if ((fadcinfo[i]+j)->totsize>0){
	  count++;
	  nextnode[i]=j;
	  tid=i*1000+j;
	  add=EBM+BufBase*(fadcinfo[i]+j)->next+0x80000000;
	  st+=rmap_req_data(sw_fd[i],i,&((fadcinfo[i]+j)->node),tid,add,((fadcinfo[i]+j)->totsize+1)/2*4);
	  //	  printf("REQ: %d-%d\n",i,j);
	  break;
	}
      }
    }
    if (count>0){
      for(i=0;i<DevsNum;i++){
	if (nextnode[i]<fadc_num[i]){
	  adc=fadcinfo[i]+nextnode[i];
	  *(curpos++)=(i<<16)|nextnode[i]; totsize+=4;
	  memcpy(curpos,(adc)->tgcreg,12); curpos+=3; totsize+=12;
	  //	  curpos++; totsize+=4; // <- here, the size info for each fadc should be placed?
	  // put datasize for each fadc. notice ch header included (unit: byte)
	  *(curpos++)=(adc)->totsize*2; totsize+=4;
	  *(curpos++)=0; totsize+=4;
	  //#ifdef DMA
	  //	  st+=rmap_rcv_header_dma(sw_fd,i,&tid,&size);
	  //#else
	  st+=rmap_rcv_header(sw_fd[i],i,&tid,&size);
	  //#endif
	  //	  printf("RCVH: %d-%d size: %d(%x)\n",i,nextnode[i],size,size);
	  portid=tid/1000;
	  nodeid=tid%1000;
	  if (portid==i && nodeid<fadc_num[i]){
	    st+=rmap_rcv_data(tid,curpos);
	    //	    printf("RCVD: %d-%d at %d(%x)\n",i,nextnode[i],curpos-rdata,curpos-rdata);
	  }else{
	    printf("Wrong TID: %d %d (%d %d)\n",portid,nodeid,i,nextnode[i]);
	    return -1;
	  }
	  curnode[i]=nextnode[i]+1;
	  (adc)->nextptr=curpos+size/4;
	  curpos+=((adc)->totsize+1)/2; totsize+=(((adc)->totsize+1)/2*4);
	  (adc)->totsize-=size/2;
	  *(curpos++)=0xffff0000; totsize+=4;
	}
      }
    }
  }
  count=4;
  for(i=0;i<DevsNum;i++) curnode[i]=0;
  while(count>0){
    count=0;
    //    printf("curnode: %d %d %d %d\n",curnode[0],curnode[1],curnode[2],curnode[3]);
    for(i=0;i<DevsNum;i++){
      nextnode[i]=fadc_num[i];
      for(j=curnode[i];j<fadc_num[i];j++){
	if ((fadcinfo[i]+j)->totsize>0){
	  count++;
	  nextnode[i]=j;
	  tid=i*1000+j;
	  add=EBM+BufBase*(fadcinfo[i]+j)->next+0x80000000;
	  st+=rmap_req_data(sw_fd[i],i,&((fadcinfo[i]+j)->node),tid,add,((fadcinfo[i]+j)->totsize+1)/2*4);
	  //	  printf("REQ: %d-%d\n",i,j);
	  break;
	}
      }
    }
    if (count>0){
      for(i=0;i<DevsNum;i++){
	if (nextnode[i]<fadc_num[i]){
	  adc=fadcinfo[i]+nextnode[i];
	  //#ifdef DMA
	  //	  st+=rmap_rcv_header_dma(sw_fd,i,&tid,&size);
	  //#else
	  st+=rmap_rcv_header(sw_fd[i],i,&tid,&size);
	  //#endif
	  //	  printf("RCVH: %d-%d size: %d(%x)\n",i,nextnode[i],size,size);
	  portid=tid/1000;
	  nodeid=tid%1000;
	  if (portid==i && nodeid<fadc_num[i]){
	    st+=rmap_rcv_data(tid,adc->nextptr);
	    //	    printf("RCVD: %d-%d at %d(%x)\n",i,nextnode[i],adc->nextptr-rdata,adc->nextptr-rdata);
	  }else{
	    printf("Wrong TID: %d %d (%d %d)\n",portid,nodeid,i,nextnode[i]);
	    return -1;
	  }
	  curnode[i]=nextnode[i]+1;
	}
      }
    }
  }
  // now this footer is obsolted
  //  *curpos=0xf0f0f0f0; totsize+=4;
  //  *rdata=totsize;
  return totsize;
}

int fadc_release_buffer(){
  int i,j;
  int st;
  struct fadc_info *adc;
  st=0;
  for(i=0;i<DevsNum;i++){
    adc=fadcinfo[i];
    for(j=0;j<fadc_num[i];j++){
      st+=fadc_release_buffer_each(adc+j);
    }
  }
  if (st<0) return -1; else return 0;
}

int fadc_release_buffer_each(struct fadc_info *adc){
  int st;
  unsigned int add, data;
  add=ROC_Done; data=1<<(adc->next);
  st=rmap_put_word(sw_fd[adc->port],adc->port,&(adc->node),add,data);
  //  add=ROC_Done; data=0;
  //  st+=rmap_put_word(sw_fd[adc->port],adc->port,&(adc->node),add,data);
  return st;
}

void fadc_prepare_next(){
  int i,j;
  struct fadc_info *adc;
  for(i=0;i<DevsNum;i++){
    adc=fadcinfo[i];
    for(j=0;j<fadc_num[i];j++){
      fadc_prepare_next_each(adc+j);
    }
  }
}

void fadc_prepare_next_each(struct fadc_info *adc){
  //  printf("next: %d -> ",adc->next);
  if (adc->next==NumEvtBuffer-1){
    (adc->next)=0;
  }else{
    (adc->next)++;
  }
  //  printf("%d\n",adc->next);
}

int fadc_init_temp(){
  int i,j;
  int st;
  struct fadc_info *adc;
  st=0;
  for(i=0;i<DevsNum;i++){
    adc=fadcinfo[i];
    for(j=0;j<fadc_num[i];j++){
      st+=fadc_init_temp_each(adc+j);
    }
  }
  if (st<0) return -1; else return 0;
}

int fadc_init_temp_each(struct fadc_info *adc){
  int st;
  unsigned int add, data;
  add=TMP_Conf; data=0x03;
  st=rmap_put_word(sw_fd[adc->port],adc->port,&(adc->node),add,data);
  return st;
}

int fadc_get_temp_each(int port, int nodeid, int *temp){
  int i,j;
  int st;
  int id0,id1;
  unsigned int add;
  struct fadc_info *adc;
  id0=-1;
  for(i=0;i<DevsNum;i++){
    adc=fadcinfo[i];
    for(j=0;j<fadc_num[i];j++){
      if ((adc+j)->port==port && (adc+j)->nodeid==nodeid){
	id0=i; id1=j;
      }
    }
  }
  if (id0==-1) return -1;
  add=TMP_Tmp;
  st=rmap_get_data(sw_fd[(fadcinfo[id0]+id1)->port],(fadcinfo[id0]+id1)->port,&((fadcinfo[id0]+id1)->node),add,temp,4);
  printf("TEMP: %d\n",st);
  return st;
}

void fadc_check_roc_trig(){
  int add,data,st;
  int i,j;
  struct fadc_info *adc;
  add=ROC_Ready;
  printf("ROC_Ready ---------------------\n");
  for(i=0;i<DevsNum;i++){
    printf("Port#%d: ",i);
    adc=fadcinfo[i];
    for(j=0;j<fadc_num[i];j++){
      st=rmap_get_data(sw_fd[(adc+j)->port],(adc+j)->port,&((adc+j)->node),add,&((adc+j)->roc),4);
      if ((adc+j)->roc>>(adc+j)->next&1) printf("%d ",j);
      //    printf("ROC: %08x %08X\n",adc->roc,(adc+1)->roc);                                                                                                                                            
    }printf("\n");
  }
  add=TGC_IOstat;
  printf("TGC_IOstat --------------------\n");
  for(i=0;i<DevsNum;i++){
    printf("Port#%d: ",i);
    adc=fadcinfo[i];
    for(j=0;j<fadc_num[i];j++){
      st=rmap_get_data(sw_fd[(adc+j)->port],(adc+j)->port,&((adc+j)->node),add,&data,4);
      if ((data&0x000000f0)>0) printf("%d ",j);
      //    printf("ROC: %08x %08X\n",adc->roc,(adc+1)->roc);                                                                                                                                            
    }printf("\n");
  }
}

void fadc_show_roc(){
  int add,data,st;
  int i,j;
  struct fadc_info *adc;
  add=ROC_Ready;
  printf("ROC_Ready ---------------------\n");
  for(i=0;i<DevsNum;i++){
    printf("Port#%d: ",i);
    adc=fadcinfo[i];
    for(j=0;j<fadc_num[i];j++){
      st=rmap_get_data(sw_fd[(adc+j)->port],(adc+j)->port,&((adc+j)->node),add,&((adc+j)->roc),4);
      if ((adc+j)->roc>>(adc+j)->next&1) printf("%d ",j);
      printf("%d:%08x ",j,(adc+j)->roc);
    }printf("\n");
  }
}

void fadc_check_busy(){
  int add,data,st;
  int i,j;
  struct fadc_info *adc;
  add=TGC_IOstat;
  printf("Busy -----------------------\n");
  for(i=0;i<DevsNum;i++){
    printf("Port#%d: ",i);
    adc=fadcinfo[i];
    for(j=0;j<fadc_num[i];j++){
      st=rmap_get_data(sw_fd[(adc+j)->port],(adc+j)->port,&((adc+j)->node),add,&data,4);
      if ((data&0x00000f00)>0) printf("%d ",j);
      //    printf("ROC: %08x %08X\n",adc->roc,(adc+1)->roc);                                                                                                                                            
    }printf("\n");
  }
  add=TGC_IOstat;
  printf("Busy Or --------------------\n");
  for(i=0;i<DevsNum;i++){
    printf("Port#%d: ",i);
    adc=fadcinfo[i];
    for(j=0;j<fadc_num[i];j++){
      st=rmap_get_data(sw_fd[(adc+j)->port],(adc+j)->port,&((adc+j)->node),add,&data,4);
      if ((data&0x0000f000)>0) printf("%d ",j);
      //    printf("ROC: %08x %08X\n",adc->roc,(adc+1)->roc);                                                                                                                                            
    }printf("\n");
  }
}

void fadc_check_default_value(){
  int i,j;
  int st;
  struct fadc_info *adc;
  st=0;
  for(i=0;i<DevsNum;i++){
    adc=fadcinfo[i];
    for(j=0;j<fadc_num[i];j++){
      st+=fadc_check_default_value_each(adc+j);
    }
  }
  if (st<0) printf("Some module(s) has non-default register value.\n");
}

void fadc_set_default_value(){
  int i,j;
  int st;
  struct fadc_info *adc;
  st=0;
  for(i=0;i<DevsNum;i++){
    adc=fadcinfo[i];
    for(j=0;j<fadc_num[i];j++){
      st+=fadc_set_default_value_each(adc+j);
    }
  }
}

int fadc_check_default_value_each(struct fadc_info *adc){
  unsigned int add,data;
  int st;
  int first;

  first=1;

  printf("%d-%d: ",adc->port,adc->nodeid);

  add=L1D_Delay;
  st=rmap_get_data(sw_fd[adc->port],adc->port,&(adc->node),add,&data,4);
  if (data!=0x000000b0){
    printf("L1D_Delay(%02X) ",data);
    first=0;
  }
  add=TGC_TrigEnab;
  st=rmap_get_data(sw_fd[adc->port],adc->port,&(adc->node),add,&data,4);
  if (data!=0x00000000){
    printf("TGC_TrigEnab(%01X) ",data);
    first=0;
  }
  add=TGC_TrigInOut;
  st=rmap_get_data(sw_fd[adc->port],adc->port,&(adc->node),add,&data,4);
  if (data!=0x0000000C){
    printf("TGC_TrigInOut(%02X) ",data);
    first=0;
  }
  //  add=TGC_BufEnab;
  //  st=rmap_get_data(sw_fd[adc->port],adc->port,&(adc->node),add,&data,4);
  //  if (data!=0x0000000F){
  //    printf("TGC_BufEnab(%01X) ",data);
  //    first=0;
  //  }
  add=TGC_ClkTrig;
  st=rmap_get_data(sw_fd[adc->port],adc->port,&(adc->node),add,&data,4);
  if (data!=0x02625A00){
    printf("TGC_ClkTrig(%08X) ",data);
    first=0;
  }
  add=EBM_Range;
  st=rmap_get_data(sw_fd[adc->port],adc->port,&(adc->node),add,&data,4);
  if (data!=0x00000320){
    printf("EBM_Range(%04X) ",data);
    first=0;
  }
  add=EBM_CmpType;
  st=rmap_get_data(sw_fd[adc->port],adc->port,&(adc->node),add,&data,4);
  if (data!=0x00000010){
    printf("EBM_CmpType(%04X) ",data);
    first=0;
  }
  add=CMN_Version;
  st=rmap_get_data(sw_fd[adc->port],adc->port,&(adc->node),add,&data,4);
  if (data!=0x20181216){
    printf("CMN_Version(%08X) ",data);
    first=0;
  }

  if (first) printf("OK\n"); else printf("\n");

  return (first-1);
}

int fadc_set_default_value_each(struct fadc_info *adc){
  unsigned int add,data;
  int st;

  add=L1D_Delay; data=0xb0;
  st=rmap_put_word(sw_fd[adc->port],adc->port,&(adc->node),add,data);
  add=TGC_TrigEnab; data=0x0;
  st=rmap_put_word(sw_fd[adc->port],adc->port,&(adc->node),add,data);
  add=TGC_TrigInOut; data=0x0C;
  st=rmap_put_word(sw_fd[adc->port],adc->port,&(adc->node),add,data);
  //  add=TGC_BufEnab; data=0xF;
  //  st=rmap_put_word(sw_fd[adc->port],adc->port,&(adc->node),add,data);
  add=TGC_ClkTrig; data=0x02625A00;
  st=rmap_put_word(sw_fd[adc->port],adc->port,&(adc->node),add,data);
  add=EBM_Range; data=0x0320;
  st=rmap_put_word(sw_fd[adc->port],adc->port,&(adc->node),add,data);
  add=EBM_CmpType; data=0x10;
  st=rmap_put_word(sw_fd[adc->port],adc->port,&(adc->node),add,data);

}

void fadc_set_full_range(unsigned int range){
  int i,j;
  unsigned int add,data;
  int st;
  struct fadc_info *adc;

  st=0;
  for(i=0;i<DevsNum;i++){
    adc=fadcinfo[i];
    for(j=0;j<fadc_num[i];j++){
      add=EBM_Range; data=range;
      st=rmap_put_word(sw_fd[(adc+j)->port],(adc+j)->port,&((adc+j)->node),add,data);
    }
  }
}

void fadc_check_TGC(int port, int node){
  int add, data, st;
  int data1,data2;
  int i;
  struct fadc_info *adc;

  adc=fadcinfo[port];
    
  printf("TGC-------------\n");
  add=TGC_FreeBuf;
  st=rmap_get_data(sw_fd[port],port,&((adc+node)->node),add,&data,4);
  printf("FreeBuf: %01X ", data);
  add=TGC_NextBuf;
  st=rmap_get_data(sw_fd[port],port,&((adc+node)->node),add,&data,4);
  printf("NextBuf; %01X ", data);
  add=TGC_Count;
  st=rmap_get_data(sw_fd[port],port,&((adc+node)->node),add,&data,4);
  printf("Count: %08X\n", data);
  printf("CountEach: ");
  for(i=0;i<NumEvtBuffer;i++){
    add=TGC_CountEach+BufBase*i;  st=rmap_get_data(sw_fd[port],port,&((adc+node)->node),add,&data,4);
    printf("%08X ", data);
  } printf("\n");
  printf("TrigID: ");
  for(i=0;i<NumEvtBuffer;i++){
    add=TGC_TrigID+BufBase*i; st=rmap_get_data(sw_fd[port],port,&((adc+node)->node),add,&data,4);
    printf("%08X ", data);
  } printf("\n");
  printf("Clk: ");
  for(i=0;i<NumEvtBuffer;i++){
    add=TGC_FClk+BufBase*i;  st=rmap_get_data(sw_fd[port],port,&((adc+node)->node),add,&data1,4);
    add=TGC_CClk+BufBase*i;  st=rmap_get_data(sw_fd[port],port,&((adc+node)->node),add,&data2,4);
    printf("%04X%08X ", data2,data1);
  } printf("\n");
  add=TGC_TrigEnab;
  st=rmap_get_data(sw_fd[port],port,&((adc+node)->node),add,&data,4);
  printf("TrigEnab: %04X  ", data);
  add=TGC_TrigInOut;
  st=rmap_get_data(sw_fd[port],port,&((adc+node)->node),add,&data,4);
  printf("TrigInOut: %04X  ", data);
  add=TGC_BufEnab;
  st=rmap_get_data(sw_fd[port],port,&((adc+node)->node),add,&data,4);
  printf("BufEnab: %08X  ", data);
  add=TGC_ClkTrig;
  st=rmap_get_data(sw_fd[port],port,&((adc+node)->node),add,&data,4);
  printf("ClkTrig: %08X\n", data);
  add=TGC_NoEmpty;
  st=rmap_get_data(sw_fd[port],port,&((adc+node)->node),add,&data,4);
  printf("NoEmpty: %08X  ", data);
  add=TGC_Busy;
  st=rmap_get_data(sw_fd[port],port,&((adc+node)->node),add,&data,4);
  printf("Busy: %08X  ", data);
  //ROC
  add=ROC_Ready;
  st=rmap_get_data(sw_fd[port],port,&((adc+node)->node),add,&data,4);
  printf("Ready: %01X\n", data);
  printf("EBM-------------\n");
  printf("Count: ");
  for(i=0;i<NumEvtBuffer;i++){
    add=EBM_Count+BufBase*i; st=rmap_get_data(sw_fd[port],port,&((adc+node)->node),add,&data,4);
    printf("%08X  ", data);
  } printf("\n");
    printf("TotSize: ");
  for(i=0;i<NumEvtBuffer;i++){
    add=EBM_TotSize+BufBase*i; st=rmap_get_data(sw_fd[port],port,&((adc+node)->node),add,&data,4);
    printf("%04X  ",data);
  }printf("\n");
}
