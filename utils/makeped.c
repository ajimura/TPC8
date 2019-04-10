#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
  int i,j,k;
  double thres;
  FILE *outfile;
  int NumFADC[8];

  for(i=0;i<8;i++) NumFADC[i]=0;
  if (argc<2){
    printf("few argument...\n");
    exit(-1);
  }
  for(i=1;i<argc;i++){
    sscanf(argv[i],"%d",&(NumFADC[i-1]));
  }

  printf("Thres?: ");
  scanf("%lf",&thres);

  if ((outfile=fopen("pedestal.txt","w"))==NULL){
    printf("Cannot open file...\n"); exit(0);
  }

  for(i=0;i<8;i++){
    for(j=0;j<NumFADC[i];j++){
      for(k=0;k<16;k++){
	fprintf(outfile,"%d %d %d %f 3.\n",i,j,k,thres);
      }
    }
  }
  fclose(outfile);
  exit(0);
}
