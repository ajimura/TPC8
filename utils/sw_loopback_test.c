#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#ifdef PCIE
#include "swpci_lib.h"
#else
#include "swsoc_lib.h"
#endif

#define MAXSIZ 16384
#define VERIFY 1

int main(int argc, char *argv[]) {
  int i,j;
  int fd0;
  int fd1;
  unsigned int data[4096];
  unsigned int ret[4096];

  int port0, port1;
  int loop;
  int size;

  struct swio_mem swio;

  printf("from ? "); scanf("%d",&port0);
  printf("to   ? "); scanf("%d",&port1);
  printf("loop ? "); scanf("%d",&loop);
  printf("size ? "); scanf("%d",&size);

  fd0=sw_open(port0);
#ifdef PCIE
  fd1=fd0;
#else
  fd1=sw_open(port1);
#endif

  ioctl(fd0,SW_TIME_MARK,&swio);

  //  for(j=0;j<size/4;j++) data[j]=j;

  for(i=0;i<loop;i++){

    //    printf("loop#%d ---------\n",i);

    for(j=0;j<size/4;j++) data[j]=j+i;
    sw_put_data(fd0,port0,data,size);
    if (sw_wait_data(fd1,port1)<0){ printf("no data \n"); exit(-1);}
    sw_get_data(fd1,port1,ret,size);
    //    printf("put data ----\n");
    //    for(j=0;j<size/4;j++){
    //      printf("%08X ",data[j]);
    //      if (j%16==15) printf("\n");
    //    }
    //    printf("get data ----\n");
    //    for(j=0;j<size/4;j++){
    //      printf("%08X ",ret[j]);
    //      if (j%16==15) printf("\n");
    //    }
#if VERIFY
    for(j=0;j<size/4;j++)
      if (data[j]!=ret[j]){
	printf("Verification error !!\n");
	exit(-1);
      }
#endif

  }
  ioctl(fd0,SW_TIME_MARK,&swio);
  close(fd0);
  close(fd1);
  exit(0);
}
