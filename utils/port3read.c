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
#include "address.h"

#define NumEvtBuffer 3

unsigned int key=0xcc;
unsigned int srcaddr=0x80;
int port;

struct rmap_node_info n;

int main(int argc, char *argv[]) {

  int sw_fd;

  unsigned int add, data, data1, data2;
  unsigned int nodeid;
  unsigned int logaddr;
  int st;
  int i,j;

  printf("Port#? ");
  scanf("%d",&port);

  //  printf("Logical Address? ");
  //  scanf("%d",&logaddr);
  printf("Node ID? ");
  scanf("%d",&nodeid);
  logaddr=nodeid+160;

  // set node info.
  n.out_size=0; n.in_size=0;
  n.dest_addr=logaddr; n.src_addr=srcaddr;
  n.key=key;

  // open
  sw_fd=sw_open(port);
  if (sw_link_test(sw_fd,port)){
    printf("Link #%d is not ready\n",port); exit(0);
  }

  //L1D
  printf("L1D-------------\n");
  add=L1D_Delay;
  st =rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("Delay : %02X  \n", data);
  printf("Prompt: ");
  for(i=0;i<16;i++){
    add=L1D_Delayed+ChBase*i;
    st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
    printf("%04X ", data);
  }printf("\n");
  //TGC
  printf("TGC-------------\n");
  add=TGC_FreeBuf;
  st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("FreeBuf: %01X ", data);
  add=TGC_NextBuf;
  st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("NextBuf; %01X ", data);
  add=TGC_Count;
  st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("Count: %08X\n", data);
  printf("CountEach: ");
  for(i=0;i<NumEvtBuffer;i++){
    add=TGC_CountEach+BufBase*i;  st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
    printf("%08X ", data);
  } printf("\n");
  printf("TrigID: ");
  for(i=0;i<NumEvtBuffer;i++){
    add=TGC_TrigID+BufBase*i; st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
    printf("%08X ", data);
  } printf("\n");
  printf("Clk: ");
  for(i=0;i<NumEvtBuffer;i++){
    add=TGC_FClk+BufBase*i;  st+=rmap_get_data(sw_fd,port,&n,add,&data1,4);
    add=TGC_CClk+BufBase*i;  st+=rmap_get_data(sw_fd,port,&n,add,&data2,4);
    printf("%04X%08X ", data2,data1);
  } printf("\n");
  add=TGC_TrigEnab;
  st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("TrigEnab: %04X  ", data);
  add=TGC_TrigInOut;
  st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("TrigInOut: %04X  ", data);
  add=TGC_BufEnab;
  st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("BufEnab: %08X  ", data);
  add=TGC_ClkTrig;
  st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("ClkTrig: %08X\n", data);
  add=TGC_NoEmpty;
  st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("NoEmpty: %08X  ", data);
  add=TGC_Busy;
  st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("Busy: %08X  ", data);
  //ROC
  add=ROC_Ready;
  st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("Ready: %01X\n", data);
  printf("EBM-------------\n");
  printf("Count: ");
  for(i=0;i<NumEvtBuffer;i++){
    add=EBM_Count+BufBase*i; st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
    printf("%08X  ", data);
  } printf("\n");
  printf("Status: ");
  for(i=0;i<NumEvtBuffer;i++){
    add=EBM_Status+BufBase*i; st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
    printf("%08X  ", data);
  } printf("\n");
  for(j=0;j<NumEvtBuffer;j++){
    printf("TotSize#%d: ",j);
    add=EBM_TotSize+BufBase*j; st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
    printf("%04X \n",data);
    printf("DataSize#%d: ",j);
    for(i=0;i<16;i++){
      add=EBM_DataSize+ChBase*i+BufBase*j; st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
      printf("%04X ",data);
    }printf("\n");
  }
  add=EBM_Range; st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("Range: %04X\n", data);
  printf("Thres: ");
  for(i=0;i<16;i++){
    add=EBM_Thres+ChBase*i; st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
    printf("%04X ",data);
  }printf("\n");
  add=EBM_CmpType; st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("CmpType: %04X\n", data);
  //CMN
  printf("CMN-------------\n");
  add=CMN_Version;
  st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("Version %08X, ",data);
  add=CMN_LogAddr;
  st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("Logical address %02X\n",data);
  add=CMN_SpWStatus0;
  st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf("SpW status: %08X",data);
  add=CMN_SpWStatus1;
  st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf(" UP:%08X",data);
  add=CMN_SpWStatus2;
  st+=rmap_get_data(sw_fd,port,&n,add,&data,4);
  printf(" DN:%08X\n",data);
  if (st<0) printf("RMAP Error %d\n",st);
  sw_close(sw_fd);
  exit(0);
}

int conv_int_bit4(int in){
  int out;
  out=(in&1)+((in>>1)&1)*10+((in>>2)&1)*100+((in>>3)&1)*1000;
  return(out);
}
