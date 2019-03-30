#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include "tpclib.h"
#include "tpcmath.h"

#define MaxNumFADC 120

struct FadcData {
  int Port;
  int NodeID;
  int TGC_Count;
  int TGC_FClk;
  int TGC_CClk;
  int DataSize[16];
  int ChData[16][512];
  double mean[16];
  double sigma[16];
  double sigma2[16];
  int num[16];
};

int main(int argc, char *argv[]) {

  int infile;

  unsigned int *indata, *curptr;
  int i,j,k;
  unsigned int totsize;
  int ch;
  int numfadc;
  int readsize;
  unsigned int footer;
  FILE *outfile;

  struct FadcData Data[MaxNumFADC];

  /* for event data */
  indata=(unsigned int *)malloc((4+(7+258*16)*MaxNumFADC)*4);

  if ((infile=open("output.dat",O_RDONLY))<0){
    printf("Cannot open file...\n"); exit(0);
  }

  if ((outfile=fopen("pedestal.txt","w"))==NULL){
    printf("Cannot open file...\n"); exit(0);
  }

  /* start analysis event data */
  for(i=0;i<MaxNumFADC;i++){
    for(j=0;j<16;j++){
      Data[i].num[j]=0;
    }
  }

  /* start event data */
  while(read(infile,&totsize,4)>0){
    printf("totsize=%d(%x)\n",totsize,totsize);
    read(infile,indata,totsize-4);
    curptr=indata; readsize=totsize-4;
    numfadc=0;

    curptr++; readsize-=4; // reserved
    curptr++; readsize-=4; // reserved

    /* start each fadc data */
    while(readsize>0){
      Data[numfadc].Port=(*curptr>>16)&0xffff;
      Data[numfadc].NodeID=(*curptr++)&0xffff; readsize-=4;
      Data[numfadc].TGC_Count=*curptr++; readsize-=4;
      Data[numfadc].TGC_FClk=*curptr++; readsize-=4;
      Data[numfadc].TGC_CClk=*curptr++; readsize-=4;
      curptr++; readsize-=4; // reserved
      curptr++; readsize-=4; // reserved
      printf("Port/NodeID %d/%d, Count/FClk/CClk %08x/%08x/%08x\n",
	     Data[numfadc].Port,Data[numfadc].NodeID,
	     Data[numfadc].TGC_Count,Data[numfadc].TGC_FClk,Data[numfadc].TGC_CClk);

      /* start each ch data */
      for(i=0;i<16;i++){
	ch=(*curptr>>16)&0xffff;
	//	printf("analyzing ch#%02d\n",ch);
	Data[numfadc].DataSize[i]=(*curptr++)&0xffff; readsize-=4;
	curptr++; readsize-=4; // reserved
	for(j=0;j<Data[numfadc].DataSize[i]/2;j++){
	  Data[numfadc].ChData[ch][j*2]=(*curptr)&0xffff;
	  Data[numfadc].ChData[ch][j*2+1]=(*curptr++>>16)&0xffff; readsize-=4;
	}
      }
      numfadc++;
      footer=*curptr++; readsize-=4;
      if (footer!=0xffff0000) printf("Something wrong! FADC Footer=%08x\n",footer);

      footer=*curptr;
      if (footer==0xf0f0f0f0){
	//	printf("Event Data ends\n");
	curptr++; readsize-=4;
      }
    }
    

    for(i=0;i<numfadc;i++){
      for(j=0;j<16;j++){
	for(k=0;k<Data[i].DataSize[j];k++){
	  if ((Data[i].ChData[j][k]&0xc000)==0){
	    fadc_calc_sigma(Data[i].num[j],Data[i].ChData[j][k],
		      &(Data[i].mean[j]),&(Data[i].sigma2[j]));
	    Data[i].num[j]++;
	  }
	}
	Data[i].sigma[j]=sqrt(Data[i].sigma2[j]);
	//	printf("%d-%d-%d %f %f\n",
	//	       Data[i].Port,Data[i].NodeID,j,Data[i].mean[j],Data[i].sigma[j]);
      }
    }
  }
  for(i=0;i<numfadc;i++){
    for(j=0;j<16;j++){
      fprintf(outfile,"%d %d %d %f %f\n",Data[i].Port,Data[i].NodeID,j,
	      Data[i].mean[j],Data[i].sigma[j]);
    }
  }
  fclose(outfile);
  close(infile);
  exit(0);
}
