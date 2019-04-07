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
#include "tpcmath.h"

#define MaxHit 20

struct hitDataInfo {
  int hitSize;
  int hitTime;
  int hitData[512];
};

struct FadcDataInfo {
  int Port;
  int NodeID;
  int TGC_Count;
  int TGC_FClk;
  int TGC_CClk;
  int ChSize[16];
  int ChData[16][512];
  int numHit[16];
  struct hitDataInfo hitData[16][MaxHit];
};

//struct HeaderInfo {
//  int ComponentType;
//  int ComponentID;
//  int totNumFADC;
//  int seq_num;
//  int year,mon,day,hour,min,sec;
//  int EventTag;
//};

struct PeakInfo {
  double mean[16];
  double sigma[16];
  double sigma2[16];
  int num[16];
};

int main(int argc, char *argv[]) {

  int infile;

  unsigned int *indata, *curptr;
  int i,j,k,l;
  unsigned int totsize, eachsize;
  int ch;
  int NumFADC[8];
  int numevent=0;
  int readsize, tpcremain;
  unsigned int footer;
  int timebin;
  unsigned int NumFADCHeader0;
  unsigned int NumFADCHeader1;
  int totFADCnum;
  int malloced=0;
  int port, nodeid;

  int inputx[3]={1,2,3};
  double coef[2];

  struct FadcDataInfo *Data[8], *DataPtr;
  //  struct HeaderInfo Header;
  struct PeakInfo *Peak[8], *PeakPtr;

  if ((infile=open(argv[1],O_RDONLY))<0){
    printf("Cannot open file...\n"); exit(-1);
  }

  /* start event data */
  while(read(infile,&totsize,4)>0){

    //    printf("\n--- event #%d ---\n\n",numevent);

    //    printf("totsize=%d(%x)\n",totsize,totsize);

    read(infile,&NumFADCHeader0,4);
    read(infile,&NumFADCHeader1,4);
    if (malloced==0){
      NumFADC[0]=NumFADCHeader0&0x000000ff;
      NumFADC[1]=(NumFADCHeader0&0x0000ff00)>>8;
      NumFADC[2]=(NumFADCHeader0&0x00ff0000)>>16;
      NumFADC[3]=(NumFADCHeader0&0xff000000)>>24;
      NumFADC[4]=NumFADCHeader1&0x000000ff;
      NumFADC[5]=(NumFADCHeader1&0x0000ff00)>>8;
      NumFADC[6]=(NumFADCHeader1&0x00ff0000)>>16;
      NumFADC[7]=(NumFADCHeader1&0xff000000)>>24;
      for(i=0;i<8;i++)
	if (NumFADC[i]>0){
	  Data[i]=malloc(sizeof(struct FadcDataInfo)*NumFADC[i]);
	  Peak[i]=malloc(sizeof(struct PeakInfo)*NumFADC[i]);
	  for(j=0;j<NumFADC[i];j++)
	    for(k=0;k<16;k++)
	      (Peak[i]+j)->num[k]=0;
	}
      //      if (NumFADC[1]>0) Data[1]=malloc(sizeof(struct FadcDataInfo)*NumFADC[1]);
      //      if (NumFADC[2]>0) Data[2]=malloc(sizeof(struct FadcDataInfo)*NumFADC[2]);
      //      if (NumFADC[3]>0) Data[3]=malloc(sizeof(struct FadcDataInfo)*NumFADC[3]);
      //      if (NumFADC[4]>0) Data[4]=malloc(sizeof(struct FadcDataInfo)*NumFADC[4]);
      //      if (NumFADC[5]>0) Data[5]=malloc(sizeof(struct FadcDataInfo)*NumFADC[5]);
      //      if (NumFADC[6]>0) Data[6]=malloc(sizeof(struct FadcDataInfo)*NumFADC[6]);
      //      if (NumFADC[7]>0) Data[7]=malloc(sizeof(struct FadcDataInfo)*NumFADC[7]);
      malloced=1;
      totFADCnum=NumFADC[0]+NumFADC[1]+NumFADC[2]+NumFADC[3]+NumFADC[4]+NumFADC[5]+NumFADC[6]+NumFADC[7];
      indata=(unsigned int *)malloc((4+(7+258*16)*totFADCnum)*4);
    }
    read(infile,indata,totsize-12);

    /* start TPC data */
    //    curptr=indata; readsize=totsize-4;tpcremain=totsize-4;
    //    //    tpcsize=*(curptr++);readsize-=4;tpcremain=tpcsize-4;
    //    //    NumFADCHeader=*(curptr++);readsize-=4;tpcremain-=4;
    //    curptr++;readsize-=8;tpcremain-=8;
    curptr=indata; readsize=totsize-12;tpcremain=totsize-12;

    for(i=0;i<8;i++)
      for(j=0;j<NumFADC[i];j++)
	for(k=0;k<16;k++){
	  (Data[i]+j)->ChSize[k]=0;
	}

    while(tpcremain>4){

	port=(*curptr>>16)&0xffff;
	nodeid=(*curptr++)&0xffff; readsize-=4;tpcremain-=4;
	DataPtr=Data[port]+nodeid;
	DataPtr->TGC_Count=*curptr++; readsize-=4;tpcremain-=4;
	DataPtr->TGC_FClk=*curptr++; readsize-=4;tpcremain-=4;
	DataPtr->TGC_CClk=*curptr++; readsize-=4;tpcremain-=4;
	curptr++;readsize-=4;tpcremain-=4;
	curptr++;readsize-=4;tpcremain-=4;
	/*
	printf("Port/NodeID %d/%d, Count/FClk/CClk %08x/%08x/%08x\n",
	       port, nodeid,
	       DataPtr->TGC_Count,
	       DataPtr->TGC_FClk,
	       DataPtr->TGC_CClk);
	*/
	/* start each ch data */
	for(i=0;i<16;i++){
	  ch=(*curptr>>16)&0xffff;
	  eachsize=(*curptr++)&0xffff; readsize-=4;tpcremain-=4;
	  curptr++;readsize-=4;tpcremain-=4;
	  DataPtr->ChSize[ch]=eachsize;
	  for(j=0;j<(eachsize+1)/2;j++){
	    DataPtr->ChData[ch][j*2]=(*curptr)&0xffff;
	    DataPtr->ChData[ch][j*2+1]=(*curptr++>>16)&0xffff; readsize-=4;tpcremain-=4;
	  }

	  /*
	  printf("%04X %04X %04X %04X %04X %04X %04X %04X\n",
		 DataPtr->ChData[ch][0],DataPtr->ChData[ch][1],
		 DataPtr->ChData[ch][2],DataPtr->ChData[ch][3],
		 DataPtr->ChData[ch][4],DataPtr->ChData[ch][5],
		 DataPtr->ChData[ch][6],DataPtr->ChData[ch][7]);
	  */
	//	footer=*curptr++; readsize-=4;;tpcremain-=4;
	//	if (footer!=0xffff0000) printf("Something wrong! tpc footer=%08x\n",footer);
	}
	footer=*curptr++; readsize-=4; tpcremain-=4;
	if (footer!=0xffff0000) printf("Something wrong! FADC Footer=%08x\n",footer);

      footer=*curptr;
      if (footer==0xf0f0f0f0){
	curptr++; readsize-=4; tpcremain-=4;
      }
    }
    //    if (readsize!=4) printf("Something wrong! readsize=%d\n",readsize);

    /* start analysis TPC data */
/*
    for(i=0;i<8;i++){
      for(j=0;j<NumFADC[i];j++){
	printf("%d-%d ",i,j);
	for(k=0;k<16;k++){
	  printf("%d(%x) ",(Data[i]+j)->ChSize[k],(Data[i]+j)->ChSize[k]);
	}printf("\n");
      }
    }
*/
/*
    printf("TGC_Count: ");
    for(i=0;i<8;i++)
      for(j=0;j<NumFADC[i];j++){
	DataPtr=Data[i]+j;
	printf("%d ",DataPtr->TGC_Count);
      }printf("\n");

    printf("TGC_FClk: ");
    for(i=0;i<8;i++)
      for(j=0;j<NumFADC[i];j++){
	DataPtr=Data[i]+j;
	printf("%d ",(DataPtr)->TGC_FClk);
      }printf("\n");

    printf("TGC_CClk: ");
    for(i=0;i<8;i++)
      for(j=0;j<NumFADC[i];j++){
	DataPtr=Data[i]+j;
	printf("%d ",(DataPtr)->TGC_CClk);
      }printf("\n");
*/
    for(i=0;i<8;i++){
      for(j=0;j<NumFADC[i];j++){
	DataPtr=Data[i]+j;
	for(k=0;k<16;k++){
	  DataPtr->numHit[k]=0; timebin=0;
	  for(l=0;l<DataPtr->ChSize[k];l++){
	    if (DataPtr->numHit[k]<MaxHit){
	      if ((DataPtr->ChData[k][l]>>14)==0x1){ /*cmp type or thres*/
		;
	      }else if ((DataPtr->ChData[k][l]>>14)==0x2 || /* TimeStamp */
			(DataPtr->ChData[k][l]>>14)==0x3){ /* end */
		if (timebin>0){
		  DataPtr->hitData[k][DataPtr->numHit[k]].hitTime=
		    ((DataPtr->ChData[k][l])&0x3FFF)-timebin;
		  DataPtr->hitData[k][DataPtr->numHit[k]].hitSize=timebin;
		  DataPtr->numHit[k]++;
		  timebin=0;
		}
	      }else{ /* normal data */
		DataPtr->hitData[k][DataPtr->numHit[k]].hitData[timebin]=
		  DataPtr->ChData[k][l];
		timebin++;
	      }
	    }
	  }
	}
      }
    }

    for(i=0;i<8;i++){
      for(j=0;j<NumFADC[i];j++){
	DataPtr=Data[i]+j;
	PeakPtr=Peak[i]+j;
	for(k=0;k<16;k++){
	  if (DataPtr->numHit[k]>0){
	    //	    printf("%d-%d-%d #hit=%d\n",i,j,k,DataPtr->numHit[k]);
	    for(l=0;l<DataPtr->numHit[k];l++){
	      //	      printf("hit#%d  Size=%d, Time=%d: ",l,DataPtr->hitData[k][l].hitSize,
	      //		     DataPtr->hitData[k][l].hitTime);
	      //	      for(m=0;m<DataPtr->hitData[k][l].hitSize;m++){
	      //		printf("%5d ",DataPtr->hitData[k][l].hitData[m]);
	      //	      }printf(",");
	      if (DataPtr->hitData[k][l].hitSize==3){
		inputx[0]=DataPtr->hitData[k][l].hitTime;
		inputx[1]=inputx[0]+1; inputx[2]=inputx[1]+1;
		fadc_solve_parabola(inputx,&(DataPtr->hitData[k][l].hitData[0]),coef);
		//		printf("solve: %lf %lf",coef[0],coef[1]); //pos and peak
		fadc_calc_sigmaD(PeakPtr->num[k],coef[1],&(PeakPtr->mean[k]),&(PeakPtr->sigma2[k]));
		PeakPtr->num[k]++;
	      }//printf("\n");
	    }
	  }
	}
      }
    }

    
    numevent++;
  }

  // printout the result of the analysis
  for(i=0;i<8;i++){
    for(j=0;j<NumFADC[i];j++){
      PeakPtr=Peak[i]+j;
      for(k=0;k<16;k++){
	PeakPtr->sigma[k]=sqrt(PeakPtr->sigma2[k]);
	printf("%d %d %d %lf %lf %d\n",i,j,k,PeakPtr->mean[k],PeakPtr->sigma[k],PeakPtr->num[k]);
      }
    }
  }

  close(infile);
  exit(0);
}
