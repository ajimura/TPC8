#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define CompTypeTPCreader 410
#define CompTypeMerger2to1 420
#define CompTypeMerger4to1 421
#define CompTypeTPClogger 430
#define CompTypeDumGen 440

#define MaxHitNum 32

struct CompHeaderInfo {
  int size;
  int year, month, day, hour, min, sec;
  int eventnum, eventtag;
  int comptype, compid;
  int NumFADC[8];
  int seq_num;
};

struct hitDataInfo {
  int hitSize;
  int hitTime;
  int hitData[512];
};

struct FadcDataInfo {
  int ReaderID;
  int PortID;
  int NodeID;
  int TGC_Count;
  int TGC_FClk;
  int TGC_CClk;
  int ChSize[16];
  int ChData[16][512];
  int numHit[16];
  struct hitDataInfo hitData[16][MaxHitNum];
};
int numFadcData=0;
struct FadcDataInfo *FadcData;

int numMerger2to1=0;
int numMerger4to1=0;
int numTPCreader=0;
int numDumGen=0;
struct CompHeaderInfo *TPCloggerHeader;
struct CompHeaderInfo *Merger2Header;
struct CompHeaderInfo *Merger4Header;
struct CompHeaderInfo *TPCreaderHeader;
struct CompHeaderInfo *DumGenHeader;

int eventcount=0;

void InitFadcHitData();
void analFadcHitData();
int get_header(int, struct CompHeaderInfo *);
void show_headerinfo(struct CompHeaderInfo *);
int store_headerInfo(struct CompHeaderInfo *);

int analTPClogger(int, struct CompHeaderInfo *);
int analMerger2to1(int, struct CompHeaderInfo *);
int analMerger4to1(int, struct CompHeaderInfo *);
int analTPCreader(int, struct CompHeaderInfo *);
int analDumGen(int, struct CompHeaderInfo *);

unsigned int *bufdata;
int curbufsize;

int verbose=0;

int main(int argc, char *argv[]) {

  int i;
  int infile;
  char filename[100]="";
  struct CompHeaderInfo next_header;
  int remain_size;

  for(i=1;i<argc;i++){
    if (argv[i][0]=='-'){
      if (argv[i][1]=='v') verbose=1;
    }else{
      strncpy(filename,argv[i],100);
    }
  }
  if (strlen(filename)==0){
    printf("Isage: %s [-v] filename\n",argv[0]);
    exit(-1);
  }
  if ((infile=open(filename,O_RDONLY))<0){
    printf("Cannot open file...\n"); exit(-1);
  }

  TPCloggerHeader=(struct CompHeaderInfo *)malloc(sizeof(struct CompHeaderInfo));

  numMerger2to1=0;
  numMerger4to1=0;
  numTPCreader=0;
  numDumGen=0;

  curbufsize=1024;
  bufdata=(unsigned int *)malloc(curbufsize);

  while (get_header(infile, &next_header)>0){

    printf("\n---\nEvent #: %d ------\n---\n\n",next_header.seq_num);
    show_headerinfo(&next_header);
    memcpy(TPCloggerHeader,&next_header,sizeof(struct CompHeaderInfo));
    remain_size=next_header.size-80;

    // init FADC hit information
    InitFadcHitData();
    
    // analyze TPClogger buffer
    remain_size-=analTPClogger(infile,TPCloggerHeader);

    // analyze FADC hit data
    analFadcHitData();

    eventcount++;
  }
  exit(0);
}

int analTPClogger(int infile, struct CompHeaderInfo *Header){
  struct CompHeaderInfo next_header;
  unsigned int footer;
  int HeaderInfoID;
  int remain_size;
  int ret;

  remain_size=Header->size-80;
  ret=Header->size;

  get_header(infile, &next_header);
  show_headerinfo(&next_header);
  HeaderInfoID=store_headerInfo(&next_header);

  switch(next_header.comptype){
  case CompTypeMerger2to1:
    remain_size-=analMerger2to1(infile,Merger2Header+HeaderInfoID);
    break;
  case CompTypeMerger4to1:
    remain_size-=analMerger4to1(infile,Merger4Header+HeaderInfoID);
    break;
  case CompTypeTPCreader:
    remain_size-=analTPCreader(infile,TPCreaderHeader+HeaderInfoID);
    break;
  case CompTypeDumGen:
    remain_size-=analDumGen(infile,DumGenHeader+HeaderInfoID);
    break;
  default:
    printf("Unknown Component type... %d\n",next_header.comptype);
    exit(-1);
  }
  if (remain_size!=0){
    printf("Something wrong in analTPClogger... remain size=%d\n",remain_size);
    exit(-1);
  }
  read(infile,&footer,4);
  if (footer!=0x00000011){
    printf("Check footer failed: %08x\n",footer);
    exit(-1);
  }
  return ret;
}

int analMerger2to1(int infile, struct CompHeaderInfo *Header){
  struct CompHeaderInfo next_header;
  unsigned int footer;
  int i;
  int HeaderInfoID;
  int remain_size;
  int ret;

  remain_size=Header->size-80;
  ret=Header->size;

  for(i=0;i<2;i++){

    get_header(infile, &next_header);
    show_headerinfo(&next_header);
    HeaderInfoID=store_headerInfo(&next_header);
      
    switch(next_header.comptype){
    case CompTypeMerger2to1:
      remain_size-=analMerger2to1(infile,Merger2Header+HeaderInfoID);
      break;
    case CompTypeMerger4to1:
      remain_size-=analMerger4to1(infile,Merger4Header+HeaderInfoID);
      break;
    case CompTypeTPCreader:
      remain_size-=analTPCreader(infile,TPCreaderHeader+HeaderInfoID);
      break;
    case CompTypeDumGen:
      remain_size-=analDumGen(infile,DumGenHeader+HeaderInfoID);
      break;
    default:
      printf("Unknown Component Type %d\n",next_header.comptype);
      exit(-1);
    }
  }
  if (remain_size!=0){
    printf("Something wrong in analMerger2to1... remain size=%d\n",remain_size);
    exit(-1);
  }
  read(infile, &footer, 4);
  if (footer!=0x00000011){
    printf("Check footer filed: %08x\n",footer);
    exit(-1);
  }

  return ret;
}

int analMerger4to1(int infile, struct CompHeaderInfo *Header){
  struct CompHeaderInfo next_header;
  unsigned int footer;
  int i;
  int HeaderInfoID;
  int remain_size;
  int ret;

  remain_size=Header->size-80;
  ret=Header->size;

  for(i=0;i<4;i++){

    get_header(infile, &next_header);
    show_headerinfo(&next_header);
    HeaderInfoID=store_headerInfo(&next_header);
      
    switch(next_header.comptype){
    case CompTypeMerger2to1:
      remain_size-=analMerger2to1(infile,Merger2Header+HeaderInfoID);
      break;
    case CompTypeMerger4to1:
      remain_size-=analMerger4to1(infile,Merger4Header+HeaderInfoID);
      break;
    case CompTypeTPCreader:
      remain_size-=analTPCreader(infile,TPCreaderHeader+HeaderInfoID);
      break;
    case CompTypeDumGen:
      remain_size-=analDumGen(infile,DumGenHeader+HeaderInfoID);
      break;
    default:
      printf("Unknown Component Type %d\n",next_header.comptype);
      exit(-1);
    }
  }
  if (remain_size!=0){
    printf("Something wrong in analMerger2to1... remain size=%d\n",remain_size);
    exit(-1);
  }
  read(infile, &footer, 4);
  if (footer!=0x00000011){
    printf("Check footer filed: %08x\n",footer);
    exit(-1);
  }

  return ret;
}

int analTPCreader(int infile, struct CompHeaderInfo *Header){
  int i,j;
  int bufsize;
  unsigned int footer;
  unsigned int *curpos;
  int remain_size;

  int port,node,trigID,fclk,cclk,size;
  int ch,eachsize;
  struct FadcDataInfo *curFadcData;
  
  bufsize=Header->size-80;
  if (bufsize>curbufsize){
    printf("bufsize=%d: allocating buffer...\n",bufsize);
    bufdata=(unsigned int *)realloc(bufdata,bufsize);
    curbufsize=bufsize;
  }

  read(infile,bufdata,bufsize);
  remain_size=bufsize;
  curpos=bufdata;

  while(remain_size>0){

    // get fadc header
    port=*(curpos)>>16;
    node=*(curpos++)&0x0000ffff; remain_size-=4;
    trigID=*(curpos++); remain_size-=4;
    fclk=*(curpos++); remain_size-=4;
    cclk=*(curpos++); remain_size-=4;
    size=*(curpos++); remain_size-=4;
    curpos++; remain_size-=4;

    // check&get FadcDataInfo buffer with Header->compid,port,node
    if (numFadcData==0){
      FadcData=(struct FadcDataInfo *)malloc(sizeof(struct FadcDataInfo));
      curFadcData=FadcData;
      numFadcData=1;
    }else{
      curFadcData=NULL;
      for(i=0;i<numFadcData;i++){
	if ( (FadcData+i)->ReaderID == Header->compid &&
	     (FadcData+i)->PortID == port && (FadcData+i)->NodeID == node){
	  curFadcData=FadcData+i;
	}
      }
      if (curFadcData==NULL){
	FadcData=(struct FadcDataInfo *)realloc(FadcData,(numFadcData+1)*sizeof(struct FadcDataInfo));
	curFadcData=FadcData+numFadcData;
	numFadcData++;
      }
    }

    //store FADC ID
    curFadcData->ReaderID=Header->compid;
    curFadcData->PortID=port;
    curFadcData->NodeID=node;

    // store trigger info
    curFadcData->TGC_Count=trigID;
    curFadcData->TGC_FClk=fclk;
    curFadcData->TGC_CClk=cclk;

    //    printf("Reader/Port/Node %d/%d/%d, Count/FClk/CClk %08x/%08x/%08x\n",
    //	   curFadcData->ReaderID,curFadcData->PortID,curFadcData->NodeID,
    //	   curFadcData->TGC_Count,curFadcData->TGC_FClk,curFadcData->TGC_CClk);
    printf("Reader/Port/Node %d/%d/%d, Count/FClk/Size %08x/%08x/%08x\n",
	   curFadcData->ReaderID,curFadcData->PortID,curFadcData->NodeID,
	   curFadcData->TGC_Count,curFadcData->TGC_FClk,size);

    // start each channel
    for(i=0;i<16;i++){
      ch=*(curpos)>>16; eachsize=*(curpos++)&0x0000ffff; remain_size-=4;
      curpos++; remain_size-=4;
      curFadcData->ChSize[ch]=eachsize;
      for(j=0;j<(eachsize+1)/2;j++){
	curFadcData->ChData[ch][j*2]=(*curpos)&0xffff;
	curFadcData->ChData[ch][j*2+1]=(*curpos++>>16)&0xffff; remain_size-=4;
      }
      if (verbose)
	printf("%04X %04X %04X %04X %04X %04X %04X %04X\n",
	       curFadcData->ChData[ch][0],curFadcData->ChData[ch][1],
	       curFadcData->ChData[ch][2],curFadcData->ChData[ch][3],
	       curFadcData->ChData[ch][4],curFadcData->ChData[ch][5],
	       curFadcData->ChData[ch][6],curFadcData->ChData[ch][7]);
    }

    // check footer
    footer=*(curpos++); remain_size-=4;
    if (footer!=0xffff0000){
      printf("Something wrong! FADC Footer=%08x\n",footer);
      exit(-1);
    }
  }
  printf("\n");

  read(infile,&footer,4);
  if (footer!=0x00000011){
    printf("Check footer failed: %08x\n",footer);
    exit(-1);
  }

  return Header->size;
}

int analDumGen(int infile, struct CompHeaderInfo *Header){
  int bufsize;
  unsigned int footer;
  //  unsigned int *curpos;
  //  int remain_size;

  bufsize=Header->size-80;
  if (bufsize>curbufsize){
    printf("bufsize=%d: allocating buffer...\n",bufsize);
    bufdata=(unsigned int *)realloc(bufdata,bufsize);
    curbufsize=bufsize;
  }

  read(infile,bufdata,bufsize);
  //  remain_size=bufsize;
  //  curpos=bufdata;

  //  while(remain_size>0){
  //    ;
  //  }
  //  printf("\n");

  read(infile,&footer,4);
  if (footer!=0x00000011){
    printf("Check footer failed: %08x\n",footer);
    exit(-1);
  }

  return Header->size;
}

int store_headerInfo(struct CompHeaderInfo *Header){
  int HeaderInfoID;
  int i;

  HeaderInfoID=-1;
  switch(Header->comptype){
  case CompTypeMerger2to1:
    for(i=0;i<numMerger2to1;i++){
      if (Header->compid==(Merger2Header+i)->compid){
	HeaderInfoID=i;
      }
    }
    if (HeaderInfoID<0){
      if (numMerger2to1==0){
	Merger2Header=(struct CompHeaderInfo *)malloc(sizeof(struct CompHeaderInfo));
      }else{
	Merger2Header=(struct CompHeaderInfo *)realloc(Merger2Header,(numMerger2to1+1)*sizeof(struct CompHeaderInfo));
      }
      HeaderInfoID=numMerger2to1;
      numMerger2to1++;
    }
    memcpy(Merger2Header+HeaderInfoID, Header,sizeof(struct CompHeaderInfo));
    break;
  case CompTypeMerger4to1:
    for(i=0;i<numMerger4to1;i++){
      if (Header->compid==(Merger4Header+i)->compid){
	HeaderInfoID=i;
      }
    }
    if (HeaderInfoID<0){
      if (numMerger4to1==0){
	Merger4Header=(struct CompHeaderInfo *)malloc(sizeof(struct CompHeaderInfo));
      }else{
	Merger4Header=(struct CompHeaderInfo *)realloc(Merger4Header,(numMerger4to1+1)*sizeof(struct CompHeaderInfo));
      }
      HeaderInfoID=numMerger4to1;
      numMerger4to1++;
    }
    memcpy(Merger4Header+HeaderInfoID, Header,sizeof(struct CompHeaderInfo));
    break;
  case CompTypeTPCreader:
    for(i=0;i<numTPCreader;i++){
      if (Header->compid==(TPCreaderHeader+i)->compid){
	HeaderInfoID=i;
      }
    }
    if (HeaderInfoID<0){
      if (numTPCreader==0){
	TPCreaderHeader=(struct CompHeaderInfo *)malloc(sizeof(struct CompHeaderInfo));
      }else{
	TPCreaderHeader=(struct CompHeaderInfo *)realloc(TPCreaderHeader,(numTPCreader+1)*sizeof(struct CompHeaderInfo));
      }
      HeaderInfoID=numTPCreader;
      numTPCreader++;
    }
    memcpy(TPCreaderHeader+HeaderInfoID, Header,sizeof(struct CompHeaderInfo));
    break;
  case CompTypeDumGen:
    for(i=0;i<numDumGen;i++){
      if (Header->compid==(DumGenHeader+i)->compid){
	HeaderInfoID=i;
      }
    }
    if (HeaderInfoID<0){
      if (numDumGen==0){
	DumGenHeader=(struct CompHeaderInfo *)malloc(sizeof(struct CompHeaderInfo));
      }else{
	DumGenHeader=(struct CompHeaderInfo *)realloc(DumGenHeader,(numDumGen+1)*sizeof(struct CompHeaderInfo));
      }
      HeaderInfoID=numDumGen;
      numDumGen++;
    }
    memcpy(DumGenHeader+HeaderInfoID, Header,sizeof(struct CompHeaderInfo));
    break;
  default:
    printf("Unknown Component Type %d\n",Header->comptype);
    exit(-1);
  }
  return HeaderInfoID;
}
  
void InitFadcHitData(){
  int i,j;
  for(i=0;i<numFadcData;i++){
    for(j=0;j<16;j++){
      (FadcData+i)->TGC_Count=-1;
      (FadcData+i)->TGC_FClk=0;
      (FadcData+i)->TGC_CClk=0;
      (FadcData+i)->ChSize[j]=0;
      (FadcData+i)->numHit[j]=0;
    }
  }
}

void analFadcHitData(){
  int i,j,k,l;
  int timebin;
  struct FadcDataInfo *curFadcData;

  if (verbose){

    printf("DataSize:\n");
    for(i=0;i<numFadcData;i++){
      curFadcData=FadcData+i;
      printf("%d-%d-%d ",curFadcData->ReaderID,curFadcData->PortID,curFadcData->NodeID);
      for(j=0;j<16;j++){
	printf("%d(%x) ",curFadcData->ChSize[j],curFadcData->ChSize[j]);
      }printf("\n");
    }

    printf("TrigID: ");
    for(i=0;i<numFadcData;i++)
      printf("%d ",(FadcData+i)->TGC_Count);
    printf("\n");

    printf("FClk: ");
    for(i=0;i<numFadcData;i++)
      printf("%d ",(FadcData+i)->TGC_FClk);
    printf("\n");

    printf("CClk: ");
    for(i=0;i<numFadcData;i++)
      printf("%d ",(FadcData+i)->TGC_CClk);
    printf("\n");
  }

  
  for(i=0;i<numFadcData;i++){
    curFadcData=FadcData+i;
    for(j=0;j<16;j++){
      timebin=0;
      for(k=0;k<curFadcData->ChSize[j];k++){
	if (curFadcData->numHit[j]<MaxHitNum){
	  if ((curFadcData->ChData[j][k]>>14)==0x1){ //cmp type of thres}
	    ;
	  }else if ((curFadcData->ChData[j][k]>>14)==0x2 ||  //time stamp
		    (curFadcData->ChData[j][k]>>14)==0x3 ){  //end
	    if (timebin>0){
	      curFadcData->hitData[j][curFadcData->numHit[j]].hitTime=
		((curFadcData->ChData[j][k])&0x3FFF)-timebin;
	      curFadcData->hitData[j][curFadcData->numHit[j]].hitSize=timebin;
	      curFadcData->numHit[j]++;
	      timebin=0;
	    }
	  }else{ // normal data
	    curFadcData->hitData[j][curFadcData->numHit[j]].hitData[timebin]=
	      curFadcData->ChData[j][k];
	    timebin++;
	  }
	}
      }
    }
  }
  if (verbose)
    for(i=0;i<numFadcData;i++){
      curFadcData=FadcData+i;
      for(j=0;j<16;j++){
	printf("%d-%d #hit=%d\n",i,j,curFadcData->numHit[j]);
	for(k=0;k<curFadcData->numHit[j];k++){
	  printf("hit#%d  Size=%d, Time=%d: ",k,curFadcData->hitData[j][k].hitSize,
		 curFadcData->hitData[j][k].hitTime);
	  for(l=0;l<curFadcData->hitData[j][k].hitSize;l++){
	    printf("%5d ",curFadcData->hitData[j][k].hitData[l]);
	  }printf("\n");
	}
      }
    }
}

int get_header(int infile, struct CompHeaderInfo *header){
  unsigned int data[19];
  int ret;
  
  if ((ret=read(infile,&(data[0]),76))>0){
    header->size=data[0];
    header->year=data[1]/1000000;
    header->month=(data[1]%1000000)/10000;
    header->day=(data[1]%10000)/100;
    header->hour=data[1]%100;
    header->min=data[2]/10000000;
    header->sec=(data[2]%10000000)/100000;
    header->eventnum=data[3];
    header->eventtag=data[4];
    header->comptype=data[5];
    header->compid=data[6];
    header->NumFADC[4]=data[16]&0xff;
    header->NumFADC[5]=(data[16]>>8)&0xff;
    header->NumFADC[6]=(data[16]>>16)&0xff;
    header->NumFADC[7]=(data[16]>>24)&0xff;
    header->NumFADC[0]=data[17]&0xff;
    header->NumFADC[1]=(data[17]>>8)&0xff;
    header->NumFADC[2]=(data[17]>>16)&0xff;
    header->NumFADC[3]=(data[17]>>24)&0xff;
    header->seq_num=data[18];
  }
  return ret;
}

void show_headerinfo(struct CompHeaderInfo *header){
  printf("CompType: %d, CompID: %d -------------------------------\n",header->comptype,header->compid);
  printf("  Size: %d\n",header->size);
  printf("  Date/Time: %d/%d/%d %d:%d:%d\n",header->year,header->month,header->day,header->hour,header->min,header->sec);
  printf("  EventNum/Tag: %d(%x)/%d(%x)\n",header->eventnum,header->eventnum,header->eventtag,header->eventtag);
  printf("  NumFADC: %d %d %d %d %d %d %d %d\n",header->NumFADC[0],header->NumFADC[1],header->NumFADC[2],header->NumFADC[3],
	 header->NumFADC[4],header->NumFADC[5],header->NumFADC[6],header->NumFADC[7]);
  printf("  Seq#: %d\n",header->seq_num);
  printf("\n");
}





