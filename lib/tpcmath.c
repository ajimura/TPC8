/* by Shuhei Ajimura */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

void fadc_calc_sigma(int num, int data, double *mean, double *sigma2){
  double oldmean,oldsigma2;
  double numD, dataD;

  if (num==0){
    *mean=(double)data;
    *sigma2=0.;
  }else{
    oldmean=*mean;
    oldsigma2=*sigma2;
    numD=(double)num;
    dataD=(double)data;
    *mean=(numD*oldmean+data)/(numD+1.);
    *sigma2=(numD*(oldsigma2+oldmean*oldmean)+dataD*dataD)/(numD+1.)-(*mean)*(*mean);
  }
}

void fadc_calc_sigmaD(int num, double data, double *mean, double *sigma2){
  double oldmean,oldsigma2;
  double numD, dataD;

  if (num==0){
    *mean=(double)data;
    *sigma2=0.;
  }else{
    oldmean=*mean;
    oldsigma2=*sigma2;
    numD=(double)num;
    dataD=(double)data;
    *mean=(numD*oldmean+data)/(numD+1.);
    *sigma2=(numD*(oldsigma2+oldmean*oldmean)+dataD*dataD)/(numD+1.)-(*mean)*(*mean);
  }
}

double i2d(int i){
  return (double)i;
}
void fadc_solve_parabola(int *x, int *y, double *coef){
  double denom;
  double x1,x2,x3,y1,y2,y3;
  double a,b,c;
  double pos,height;
  x1=i2d(*x);  x2=i2d(*(x+1));  x3=i2d(*(x+2));
  y1=i2d(*y);  y2=i2d(*(y+1));  y3=i2d(*(y+2));
  denom=(x1-x2)*(x2-x3)*(x3-x1);
  a=( x1*(y2-y3) + x2*(y3-y1) +  x3*(y1-y2) ) / denom;
  b=-( x1*x1*(y2-y3) + x2*x2*(y3-y1) +  x3*x3*(y1-y2) ) / denom;
  c=y1-a*x1*x1-b*x1;
  pos=-b/2./a;
  height=c-a*pos*pos;
  *coef=pos;
  *(coef+1)=height;
}

void fadc_solve_parabolaD(double *x, double *y, double *coef){
  double denom;
  double x1,x2,x3,y1,y2,y3;
  double a,b,c;
  double pos,height;
  x1=*x;  x2=*(x+1);  x3=*(x+2);
  y1=*y;  y2=*(y+1);  y3=*(y+2);
  denom=(x1-x2)*(x2-x3)*(x3-x1);
  a=( x1*(y2-y3) + x2*(y3-y1) +  x3*(y1-y2) ) / denom;
  b=-( x1*x1*(y2-y3) + x2*x2*(y3-y1) +  x3*x3*(y1-y2) ) / denom;
  c=y1-a*x1*x1-b*x1;
  pos=-b/2./a;
  height=c-a*pos*pos;
  *coef=pos;
  *(coef+1)=height;
}

