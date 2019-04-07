#define DEFAULT_IP "172.16.207.238"
#define DEFAULT_PORT 4660

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "trigio_lib.h"

int main(int argc, char* argv[]){

  char* sitcpIpAddr;
  unsigned int sitcpPort;

  if(argc != 3){
    sitcpIpAddr = DEFAULT_IP;
      sitcpPort   = DEFAULT_PORT;
  }else{
    sitcpIpAddr = argv[1];
    sitcpPort   = atoi(argv[2]);
  }

  if (trigio_ini(sitcpIpAddr, sitcpPort)<0) exit(-1);

  trigio_read_done();

  trigio_fin();
  exit(0);
}
