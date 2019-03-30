#define DEFAULT_IP "172.16.207.238"
//#define DEFAULT_IP "192.168.208.85"
#define DEFAULT_PORT 4660

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "trigio_lib.h"

int main(int argc, char* argv[]){

  char* sitcpIpAddr;
  unsigned int sitcpPort;

  int st;

  unsigned char data[2048];
  unsigned int addr;

  if(argc != 3){
    sitcpIpAddr = DEFAULT_IP;
      sitcpPort   = DEFAULT_PORT;
  }else{
    sitcpIpAddr = argv[1];
    sitcpPort   = atoi(argv[2]);
  }

  if (trigio_ini(sitcpIpAddr, sitcpPort)<0) exit(-1);

  addr=0x00000000; st=trigio_rd(addr,data,16);
  if (st<0) printf("Error...\n");
  printf("Current Event Tag:\t0x%02X\n",data[11]);
  printf("Trig Count:\t\t0x%02X\n",data[12]);
  printf("Buf Write Pointer:\t0x%02X\n",data[13]);
  printf("Buf Read Pointer:\t0x%02X\n",data[14]);
  printf("Buf Full:\t\t0x%02X\n",data[15]);

  trigio_fin();
}
