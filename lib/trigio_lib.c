/* by Shuhei Ajimura */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define RBCP_VER 0xFF
#define RBCP_CMD_WR 0x80
#define RBCP_CMD_RD 0xC0

#define TIMEOUT_SEC 0
#define TIMEOUT_USEC 3000
#define RETRY_NUM 5

#define DEF_EVTAG 255

struct sockaddr_in TrigIO_Addr;
int TrigIO_sock;

unsigned char TrigIO_ID;

int trigio_wr(unsigned int , unsigned char* , unsigned char);
int trigio_rd(unsigned int , unsigned char* , unsigned char);

int trigio_ini(const char* ipaddr, unsigned int port){

  TrigIO_Addr.sin_addr.s_addr = inet_addr(ipaddr);
  if (TrigIO_Addr.sin_addr.s_addr){ // if not 0.0.0.0
    TrigIO_sock = socket(AF_INET, SOCK_DGRAM, 0);
    TrigIO_Addr.sin_family = AF_INET;
    TrigIO_Addr.sin_port = htons(port);
    TrigIO_ID = 0;
  }
  return 0;
}

int trigio_fin(){
  if (TrigIO_Addr.sin_addr.s_addr)
    close(TrigIO_sock);
  return 0;
}

int trigio_reset_count(){
  unsigned int addr;
  unsigned char data;
  int st=0;

  if (TrigIO_Addr.sin_addr.s_addr){
    addr=0x0000000c; data=0x00; st=trigio_wr(addr,&data,1);
  }
  return st;
}

int trigio_read_eventtag(unsigned char *tag){
  unsigned int addr;
  int st=0;

  if (TrigIO_Addr.sin_addr.s_addr){
    addr=0x0000000b; st=trigio_rd(addr,tag,1);
  }else{
    tag=DEF_EVTAG;
  }
  return st;
}

int trigio_read_done(){
  unsigned int addr;
  unsigned char data;
  int st=0;

  if (TrigIO_Addr.sin_addr.s_addr){
    addr=0x0000000b; data=0x00; st=trigio_wr(addr,&data,1);
  }
  return st;
}

int trigio_wr(unsigned int address, unsigned char* data, unsigned char length){

  int cmdPckLen;
  char sndBuf[1024],rcvBuf[1024];
  struct timeval timeout;
  fd_set setSelect;
  int ret;

  /* fill header data */
  sndBuf[0]=0xff; //RBCP Version
  sndBuf[1]=0x80; //RBCP Write Command
  sndBuf[2]=TrigIO_ID;
  sndBuf[3]=length;
  sndBuf[4]=(0xff000000&address)>>24;
  sndBuf[5]=(0x00ff0000&address)>>16;
  sndBuf[6]=(0x0000ff00&address)>>8;
  sndBuf[7]=(0x000000ff&address);
  memcpy(sndBuf+8,data,length);
  cmdPckLen=length+8;

  /* send a packet*/
  sendto(TrigIO_sock, sndBuf, cmdPckLen, 0, (struct sockaddr *)&TrigIO_Addr, sizeof(TrigIO_Addr));
  /* Receive packets*/
  FD_ZERO(&setSelect);
  FD_SET(TrigIO_sock, &setSelect);
  timeout.tv_sec  = 1;
  timeout.tv_usec = 0;
  if(select(TrigIO_sock+1, &setSelect, NULL, NULL,&timeout)==0){ //TimeOut
    puts("\n***** Timeout ! *****");
    TrigIO_ID++;
    return -1;
  }else{
    if(FD_ISSET(TrigIO_sock,&setSelect)){      /* receive packet */
      ret=recvfrom(TrigIO_sock, rcvBuf, 2048, 0, NULL, NULL);
      if(ret<cmdPckLen){
	puts("ERROR: ACK packet is too short");
	TrigIO_ID++;
	return -1;
      }
      if((0x0f & rcvBuf[1])!=0x8){
	puts("ERROR: Detected bus error");
	TrigIO_ID++;
	return -1;
      }
      if(rcvBuf[2]!=TrigIO_ID){
	puts("ERROR: Packet ID is wrong");
	TrigIO_ID++;
	return -1;
      }
    }
  }
  return 0;
}

int trigio_rd(unsigned int address, unsigned char* data, unsigned char length){

  int cmdPckLen;
  char sndBuf[1024],rcvBuf[1024];
  struct timeval timeout;
  fd_set setSelect;
  int retsiz;
  int retry_count;

  /* fill header data */
  sndBuf[0]=0xff; //RBCP Version
  sndBuf[1]=0xc0; //RBCP Read Command
  sndBuf[2]=TrigIO_ID;
  sndBuf[3]=length;
  sndBuf[4]=(0xff000000&address)>>24;
  sndBuf[5]=(0x00ff0000&address)>>16;
  sndBuf[6]=(0x0000ff00&address)>>8;
  sndBuf[7]=(0x000000ff&address);
  cmdPckLen=8;

  retry_count=0;
  while(1){

  /* send a packet*/
  sendto(TrigIO_sock, sndBuf, cmdPckLen, 0, (struct sockaddr *)&TrigIO_Addr, sizeof(TrigIO_Addr));
  /* Receive packets*/
  FD_ZERO(&setSelect);
  FD_SET(TrigIO_sock, &setSelect);
  timeout.tv_sec  = TIMEOUT_SEC;
  timeout.tv_usec = TIMEOUT_USEC;
  if(select(TrigIO_sock+1, &setSelect, NULL, NULL,&timeout)==0){ //TimeOut
    if ((retry_count++)==RETRY_NUM){
      puts("\n***** Timeout ! *****");
      TrigIO_ID++;
      return -1;
    }
  }else{
    if(FD_ISSET(TrigIO_sock,&setSelect)){      /* receive packet */
      retsiz=recvfrom(TrigIO_sock, rcvBuf, 2048, 0, NULL, NULL);
      if(retsiz<cmdPckLen+length){
	puts("ERROR: ACK packet is too short");
	TrigIO_ID++;
	return -1;
      }
      if((0x0f & rcvBuf[1])!=0x8){
	puts("ERROR: Detected bus error");
	TrigIO_ID++;
	return -1;
      }
      if(rcvBuf[2]!=TrigIO_ID){
	puts("ERROR: Packet ID is wrong");
	TrigIO_ID++;
	return -1;
      }
    }
    memcpy(data, &(rcvBuf[8]), retsiz-8);
    return retsiz-8;
  }

  } //while
}
