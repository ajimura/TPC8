/* by Shuhei Ajimura */

#include <stdio.h>
#include <string.h>
#ifdef PCIE
#include "swpci_lib.h"
#else
#include "swsoc_lib.h"
#endif

#define RM_LINK_ERROR -1
#define RM_DATA_ERROR -2
#define RM_BUF_OVERFLOW -3
#define RM_BUF_NONE -4

struct rmap_node_info {
  int out_size; unsigned char out_path[12];
  int  in_size; unsigned char  in_path[12];
  unsigned char dest_addr;
  unsigned char src_addr;
  unsigned char key;;
};

#define X_BUFFER_SIZE 16000

#define RM_PCKT_CMD  0x40
#define RM_PCKT_WRT  0x20
#define RM_PCKT_VER  0x10
#define RM_PCKT_ACK  0x08
#define RM_PCKT_INC  0x04

#define RM_ProtoID 0x01

#define WaitLoop 2000

unsigned char rmap_calc_crc(void *,unsigned int );
int rmap_create_buffer(unsigned int, unsigned int, struct rmap_node_info *, unsigned int, unsigned int);

unsigned char rx_buffer[X_BUFFER_SIZE+1024];
unsigned char tx_buffer[X_BUFFER_SIZE+1024];

unsigned char RM_CRCTbl [] = {
  0x00,0x91,0xe3,0x72,0x07,0x96,0xe4,0x75,  0x0e,0x9f,0xed,0x7c,0x09,0x98,0xea,0x7b,
  0x1c,0x8d,0xff,0x6e,0x1b,0x8a,0xf8,0x69,  0x12,0x83,0xf1,0x60,0x15,0x84,0xf6,0x67,
  0x38,0xa9,0xdb,0x4a,0x3f,0xae,0xdc,0x4d,  0x36,0xa7,0xd5,0x44,0x31,0xa0,0xd2,0x43,
  0x24,0xb5,0xc7,0x56,0x23,0xb2,0xc0,0x51,  0x2a,0xbb,0xc9,0x58,0x2d,0xbc,0xce,0x5f,
  0x70,0xe1,0x93,0x02,0x77,0xe6,0x94,0x05,  0x7e,0xef,0x9d,0x0c,0x79,0xe8,0x9a,0x0b,
  0x6c,0xfd,0x8f,0x1e,0x6b,0xfa,0x88,0x19,  0x62,0xf3,0x81,0x10,0x65,0xf4,0x86,0x17,
  0x48,0xd9,0xab,0x3a,0x4f,0xde,0xac,0x3d,  0x46,0xd7,0xa5,0x34,0x41,0xd0,0xa2,0x33,
  0x54,0xc5,0xb7,0x26,0x53,0xc2,0xb0,0x21,  0x5a,0xcb,0xb9,0x28,0x5d,0xcc,0xbe,0x2f,
  0xe0,0x71,0x03,0x92,0xe7,0x76,0x04,0x95,  0xee,0x7f,0x0d,0x9c,0xe9,0x78,0x0a,0x9b,
  0xfc,0x6d,0x1f,0x8e,0xfb,0x6a,0x18,0x89,  0xf2,0x63,0x11,0x80,0xf5,0x64,0x16,0x87,
  0xd8,0x49,0x3b,0xaa,0xdf,0x4e,0x3c,0xad,  0xd6,0x47,0x35,0xa4,0xd1,0x40,0x32,0xa3,
  0xc4,0x55,0x27,0xb6,0xc3,0x52,0x20,0xb1,  0xca,0x5b,0x29,0xb8,0xcd,0x5c,0x2e,0xbf,
  0x90,0x01,0x73,0xe2,0x97,0x06,0x74,0xe5,  0x9e,0x0f,0x7d,0xec,0x99,0x08,0x7a,0xeb,
  0x8c,0x1d,0x6f,0xfe,0x8b,0x1a,0x68,0xf9,  0x82,0x13,0x61,0xf0,0x85,0x14,0x66,0xf7,
  0xa8,0x39,0x4b,0xda,0xaf,0x3e,0x4c,0xdd,  0xa6,0x37,0x45,0xd4,0xa1,0x30,0x42,0xd3,
  0xb4,0x25,0x57,0xc6,0xb3,0x22,0x50,0xc1,  0xba,0x2b,0x59,0xc8,0xbd,0x2c,0x5e,0xcf
};

int rmap_get_data0(int sw_fd, int port,
			   struct rmap_node_info *n,
			   unsigned int rx_address,
			   unsigned int *rx_data,
			   unsigned int rx_size)
{
  int i,j;
  int retval;
  unsigned int n_data;
  int packet_size;

  //  if (rx_size>2048) n_data=2048;
  if (rx_size>X_BUFFER_SIZE) n_data=X_BUFFER_SIZE;
  else n_data=rx_size;

  packet_size=rmap_create_buffer(RM_PCKT_CMD|RM_PCKT_ACK|RM_PCKT_INC,0x0000,n,rx_address,n_data);
  retval = sw_put_data0(sw_fd, port, (unsigned int *)tx_buffer, packet_size);

  if (retval<0){
    printf("rmap_get_data0: Error in sw_put_data0 ret=%d\n", retval);
    sw_print_status(sw_fd,port);
    return -1;
  }

  for(i=0;i<WaitLoop;i++) {
    if ((j=sw_rx_status(sw_fd,port))>0) break;
  }
  if (i==WaitLoop){
    printf("Timeout in rmap_get_data0 port=%d\n",port);
    sw_print_status(sw_fd,port);
    return -1;
  }

  retval = sw_get_data0(sw_fd, port, (unsigned int *)rx_buffer,X_BUFFER_SIZE);
  if (retval<=0){
    printf("rmap_get_data0: Error in sw_get_data0 ret=%d\n", retval);
    sw_print_status(sw_fd,port);
    return -1;
  }

  if ( (rx_buffer[3]&0xff) != 0x00){ // NOT SUCCESS
    printf("rmap_get_data0: RMAP error code=%08x\n",rx_buffer[3]);
    sw_print_status(sw_fd,port);
    printf("tx buffer ---\n");
    for(i=0;i<packet_size;i++){
      printf("%02x ",tx_buffer[i]);
      if ((i+1)%8==0) printf("\n");
    }printf("\n");
    printf("rx buffer ---\n");
    for(i=0;i<retval;i++){
      printf("%02x ",rx_buffer[i]);
      if ((i+1)%8==0) printf("\n");
    }printf("\n");
    return RM_DATA_ERROR;
  }
  memcpy(rx_data,&(rx_buffer[12]),n_data);

  return 0;
}

int rmap_get_data(int sw_fd, int port,
			   struct rmap_node_info *n,
			   unsigned int rx_address,
			   unsigned int *rx_data,
			   unsigned int rx_size)
{
  int i,j;
  int retval;
  unsigned int n_data;
  int packet_size;

  //  if (rx_size>2048) n_data=2048;
  if (rx_size>X_BUFFER_SIZE) n_data=X_BUFFER_SIZE;
  else n_data=rx_size;

  packet_size=rmap_create_buffer(RM_PCKT_CMD|RM_PCKT_ACK|RM_PCKT_INC,0x0000,n,rx_address,n_data);
  retval = sw_put_data(sw_fd, port, (unsigned int *)tx_buffer, packet_size);

  if (retval<0){
    printf("rmap_get_data: Error in sw_put_data ret=%d\n", retval);
    sw_print_status(sw_fd,port);
    return -1;
  }

  for(i=0;i<WaitLoop;i++) {
    if ((j=sw_rx_status(sw_fd,port))>0) break;
  }
  if (i==WaitLoop){
    printf("Timeout in rmap_get_data port=%d\n",port);
    sw_print_status(sw_fd,port);
    return -1;
  }

  retval = sw_get_data(sw_fd, port, (unsigned int *)rx_buffer,X_BUFFER_SIZE);
  if (retval<=0){
    printf("rmap_get_data: Error in sw_get_data ret=%d\n", retval);
    sw_print_status(sw_fd,port);
    return -1;
  }

  if ( (rx_buffer[3]&0xff) != 0x00){ // NOT SUCCESS
    printf("rmap_get_data: RMAP error code=%08x\n",rx_buffer[3]);
    sw_print_status(sw_fd,port);
    printf("tx buffer ---\n");
    for(i=0;i<packet_size;i++){
      printf("%02x ",tx_buffer[i]);
      if ((i+1)%8==0) printf("\n");
    }printf("\n");
    printf("rx buffer ---\n");
    for(i=0;i<retval;i++){
      printf("%02x ",rx_buffer[i]);
      if ((i+1)%8==0) printf("\n");
    }printf("\n");
    return RM_DATA_ERROR;
  }
  memcpy(rx_data,&(rx_buffer[12]),n_data);

  return 0;
}


int rmap_get_data2(int sw_fd, int port,
			   struct rmap_node_info *n,
			   unsigned int rx_address,
			   unsigned int *rx_data,
			   unsigned int rx_size)
{
  int i,j;
  int retval;
  unsigned int n_data;
  int ret_status;

  if (rx_size>X_BUFFER_SIZE) n_data=X_BUFFER_SIZE;
  else n_data=rx_size;

  retval=sw_req(sw_fd,port,
		RM_PCKT_CMD|RM_PCKT_ACK|RM_PCKT_INC,
		n->src_addr,n->dest_addr,n->key,0x1234,rx_address,n_data);

  for(i=0;i<WaitLoop;i++) {
    if ((j=sw_rx_status(sw_fd,port))>0) break;
  }
  if (i==WaitLoop){
    printf("Timeout in rmap_get_data2\n");
    sw_print_status(sw_fd,port);
    return -1;
  }

  retval = sw_rcv(sw_fd, port, (unsigned int *)rx_data, &ret_status, 0,  X_BUFFER_SIZE);
  if (retval<=0){
    printf("rmap_get_data: Error in sw_rcv ret=%d\n", retval);
    sw_print_status(sw_fd,port);
    return -1;
  }

  if (ret_status<0){
    printf("%d-%d %x %x\n",port,n->dest_addr-160,ret_status,rx_address);
    return RM_DATA_ERROR;
  }

  return 0;
}

int rmap_get_data_verbose(int sw_fd, int port,
			   struct rmap_node_info *n,
			   unsigned int rx_address,
			   unsigned int *rx_data,
			   unsigned int rx_size)
{
  int i,j;
  int retval;
  unsigned int n_data;
  int packet_size;

  //  if (rx_size>2048) n_data=2048;
  if (rx_size>X_BUFFER_SIZE) n_data=X_BUFFER_SIZE;
  else n_data=rx_size;

  packet_size=rmap_create_buffer(RM_PCKT_CMD|RM_PCKT_ACK|RM_PCKT_INC,0x0000,n,rx_address,n_data);
  retval = sw_put_data(sw_fd, port, (unsigned int *)tx_buffer, packet_size);

  printf("tx buffer ---\n");
  for(i=0;i<packet_size;i++){
    printf("%02x ",tx_buffer[i]);
    if ((i+1)%8==0) printf("\n");
  }printf("\n");

  if (retval<0){
    printf("rmap_get_data_v: Error in sw_put_data ret=%d\n", retval);
    sw_print_status(sw_fd,port);
    return -1;
  }

  for(i=0;i<WaitLoop;i++) {
    if ((j=sw_rx_status(sw_fd,port))>0) break;
  }
  if (i==WaitLoop){
    printf("Timeout in rmap_get_data_v port=%d\n",port);
    sw_print_status(sw_fd,port);
    return -1;
  }

  retval = sw_get_data(sw_fd, port, (unsigned int *)rx_buffer,X_BUFFER_SIZE);
  if (retval<=0){
    printf("rmap_get_data: Error in sw_get_data ret=%d\n", retval);
    sw_print_status(sw_fd,port);
    return -1;
  }

  printf("rx buffer ---\n");
  for(i=0;i<retval;i++){
    printf("%02x ",rx_buffer[i]);
    if ((i+1)%8==0) printf("\n");
  }printf("\n");

  if ( (rx_buffer[3]&0xff) != 0x00){ // NOT SUCCESS
    printf("rmap_get_data_v: RMAP error code=%08x\n",rx_buffer[3]);
    return RM_DATA_ERROR;
  }
  memcpy(rx_data,&(rx_buffer[12]),n_data);

  return 0;
}

int rmap_req_data0(int sw_fd, int port, struct rmap_node_info *n, 
			   unsigned int tid,
			   unsigned int rx_address,
			   unsigned int rx_size)
{
  unsigned int n_data;
  unsigned int retval;
  int packet_size;

  //  printf("TID(req)=%X(%d)\n",tid,tid);

  //  if (rx_size>2048) n_data=2048;
  //  else n_data=rx_size;
  if (rx_size>X_BUFFER_SIZE) n_data=X_BUFFER_SIZE; else n_data=rx_size;

  packet_size=rmap_create_buffer(RM_PCKT_CMD|RM_PCKT_ACK|RM_PCKT_INC,tid,n,rx_address,n_data);
  retval = sw_put_data(sw_fd, port, (unsigned int *)tx_buffer,
  		       packet_size);
  if (retval<0){
    printf("rmap_req_data0: Error in sw_put_data ret=%d\n", retval);
    sw_print_status(sw_fd,port);
    return -1;
  }

  return retval;
}

int rmap_req_data(int sw_fd, int port, struct rmap_node_info *n, 
			   unsigned int tid,
			   unsigned int rx_address,
			   unsigned int rx_size)
{
  unsigned int retval;

  retval=sw_req(sw_fd,port,RM_PCKT_CMD|RM_PCKT_ACK|RM_PCKT_INC,
		n->src_addr,n->dest_addr,n->key,tid,rx_address,rx_size);
  if (retval<0){
    printf("rmap_req_data: Error in sw_req ret=%d\n", retval);
    sw_print_status(sw_fd,port);
    return -1;
  }
  
  return retval;
}

int rmap_rcv0(int sw_fd, int port, unsigned int tid, unsigned int *rx_size, unsigned int *rx_data){
  unsigned int retval;
  int i,j;
  unsigned int rx_tid;

  for(i=0;i<WaitLoop;i++) {
    if ((j=sw_rx_status(sw_fd,port))>0) break;
  }
  if (i==WaitLoop){
    printf("Timeout in rmap_rcv0 port=%d\n",port);
    sw_print_status(sw_fd,port);
    return -1;
  }
  retval=sw_get_data(sw_fd, port, (unsigned int *)rx_buffer,X_BUFFER_SIZE+100);
  if (retval<=0){
    printf("rmap_rcv0: Error in sw_get_data ret=%d\n", retval);
    sw_print_status(sw_fd,port);
    return -1;
  }
  if (rx_buffer[3]!=0){
    printf("rmap_rcv0: RMAP error code=%08x\n",rx_buffer[3]);
    sw_print_status(sw_fd,port);
    printf("rx buffer ---\n");
    for(i=0;i<retval;i++){
      printf("%02x ",rx_buffer[i]);
      if ((i+1)%8==0) printf("\n");
    }printf("\n");
    return RM_DATA_ERROR;
  }
  rx_tid =rx_buffer[5]*0x100+rx_buffer[6];
  *rx_size=rx_buffer[8]*0x10000+rx_buffer[9]*0x100+rx_buffer[10];
  if (tid!=0 && tid!=rx_tid){
    printf("rmap_rcv0: TID mismatch %04x shoud be %04x\n",rx_tid,tid);
    sw_print_status(sw_fd,port);
    return RM_LINK_ERROR;
  }
  if (*rx_size==0){
    printf("rmap_rcv0: receive size=0\n");
    sw_print_status(sw_fd,port);
    return RM_BUF_NONE;
  }
  memcpy(rx_data,&(rx_buffer[12]),*rx_size);
  return 0;
}

int rmap_rcv_all(int sw_fd, int port, unsigned int tid, unsigned int *rx_size, unsigned int *rx_data){
  unsigned int retval;
  int status;
  int i,j;
  
  for(i=0;i<WaitLoop;i++) {
    if ((j=sw_rx_status(sw_fd,port))>0) break;
  }
  if (i==WaitLoop){
    printf("Timeout in rmap_rcv_header\n"); return -1;
  }
#ifdef DMA
  retval=sw_drcv(sw_fd, port, rx_data, &status, tid, X_BUFFER_SIZE+100);
#else
  retval=sw_rcv(sw_fd, port, rx_data, &status, tid, X_BUFFER_SIZE+100);
#endif
  if (status>0) return RM_LINK_ERROR;
  if (retval==0) return RM_DATA_ERROR;
  *rx_size=retval;
  return 0;
}

int rmap_put_word0(int sw_fd, int port,
			   struct rmap_node_info *n, 
			   unsigned int tx_address, 
			   unsigned int tx_data)
{
  int i,j;
  unsigned int retval;
  int packet_size;
  
  packet_size=rmap_create_buffer(RM_PCKT_CMD|RM_PCKT_WRT|RM_PCKT_ACK,0x0000,n,tx_address,tx_data);
  retval = sw_put_data0(sw_fd, port, (unsigned int *)tx_buffer, packet_size);

  if (retval<0){
    printf("rmap_put_word0: Error in sw_put_data0 ret=%d\n", retval);
    sw_print_status(sw_fd,port);
    return -1;
  }

  for(i=0;i<WaitLoop;i++) {
    if ((j=sw_rx_status(sw_fd,port))>0) break;
  }
  if (i==WaitLoop){
    printf("Timeout in rmap_put_word0\n");
    sw_print_status(sw_fd,port);
    return -1;
  }

  retval = sw_get_data0(sw_fd, port, (unsigned int *)rx_buffer,X_BUFFER_SIZE);
  if (retval<=0){
    printf("rmap_put_word0: Error in sw_get_data0 ret=%d\n", retval);
    sw_print_status(sw_fd,port);
    return -1;
  }

  if ( (rx_buffer[3]&0xff) != 0x00){ // NOT SUCCESS
    printf("rmap_get_data0: RMAP error code=%08x\n",rx_buffer[3]);
    sw_print_status(sw_fd,port);
    printf("tx buffer ---\n");
    for(i=0;i<packet_size;i++){
      printf("%02x ",tx_buffer[i]);
      if ((i+1)%8==0) printf("\n");
    }printf("\n");
    printf("rx buffer ---\n");
    for(i=0;i<retval;i++){
      printf("%02x ",rx_buffer[i]);
      if ((i+1)%8==0) printf("\n");
    }printf("\n");
    return RM_DATA_ERROR;
  }

  return 0;
  
}

int rmap_put_word(int sw_fd, int port,
			   struct rmap_node_info *n, 
			   unsigned int tx_address, 
			   unsigned int tx_data)
{
  int i,j;
  unsigned int retval;
  int packet_size;
  
  packet_size=rmap_create_buffer(RM_PCKT_CMD|RM_PCKT_WRT|RM_PCKT_ACK,0x0000,n,tx_address,tx_data);
  retval = sw_put_data(sw_fd, port, (unsigned int *)tx_buffer, packet_size);

  if (retval<0){
    printf("rmap_put_word: Error in sw_put_data ret=%d\n", retval);
    sw_print_status(sw_fd,port);
    return -1;
  }

  for(i=0;i<WaitLoop;i++) {
    if ((j=sw_rx_status(sw_fd,port))>0) break;
  }
  if (i==WaitLoop){
    printf("Timeout in rmap_put_word\n");
    sw_print_status(sw_fd,port);
    return -1;
  }

  retval = sw_get_data(sw_fd, port, (unsigned int *)rx_buffer,X_BUFFER_SIZE);
  if (retval<=0){
    printf("rmap_put_word: Error in sw_get_data ret=%d\n", retval);
    sw_print_status(sw_fd,port);
    return -1;
  }

  if ( (rx_buffer[3]&0xff) != 0x00){ // NOT SUCCESS
    printf("rmap_get_data: RMAP error code=%08x\n",rx_buffer[3]);
    sw_print_status(sw_fd,port);
    printf("tx buffer ---\n");
    for(i=0;i<packet_size;i++){
      printf("%02x ",tx_buffer[i]);
      if ((i+1)%8==0) printf("\n");
    }printf("\n");
    printf("rx buffer ---\n");
    for(i=0;i<retval;i++){
      printf("%02x ",rx_buffer[i]);
      if ((i+1)%8==0) printf("\n");
    }printf("\n");
    return RM_DATA_ERROR;
  }

  return 0;
  
}

int rmap_put_word2(int sw_fd, int port,
			   struct rmap_node_info *n, 
			   unsigned int tx_address, 
			   unsigned int tx_data)
{
  int i,j;
  unsigned int retval;
  
  retval=sw_req(sw_fd,port,RM_PCKT_CMD|RM_PCKT_WRT|RM_PCKT_ACK,
		n->src_addr,n->dest_addr,n->key,0x1234,tx_address,tx_data);

  for(i=0;i<WaitLoop;i++) {
    if ((j=sw_rx_status(sw_fd,port))>0) break;
  }
  if (i==WaitLoop){
    printf("Timeout in rmap_put_word\n"); return -1;
  }

  retval = sw_get_data(sw_fd, port, (unsigned int *)rx_buffer,X_BUFFER_SIZE);

  if (retval==0) return RM_LINK_ERROR;
  
  // GET STATUS
  if ( (rx_buffer[3]&0xff) != 0x00){ // NOT SUCCESS
    printf("Error Ack'ed: code -- 0x%02X\n",rx_buffer[3]);
    return RM_DATA_ERROR;
  }

  return 0;
  
}

int rmap_put_word_verbose(int sw_fd, int port,
			   struct rmap_node_info *n, 
			   unsigned int tx_address, 
			   unsigned int tx_data)
{
  int i,j;
  unsigned int retval;
  int packet_size;
  
  packet_size=rmap_create_buffer(RM_PCKT_CMD|RM_PCKT_WRT|RM_PCKT_ACK,0x0000,n,tx_address,tx_data);
  retval = sw_put_data(sw_fd, port, (unsigned int *)tx_buffer, packet_size);

  printf("tx buffer ---\n");
  for(i=0;i<packet_size;i++){
    printf("%02x ",tx_buffer[i]);
    if ((i+1)%8==0) printf("\n");
  }printf("\n");

  for(i=0;i<8000;i++) {
    if ((j=sw_rx_status(sw_fd,port))>0) break;
  }
  if (i==8000){
    printf("Timeout in rmap_put_data\n"); return -1;
  }

  retval = sw_get_data(sw_fd, port, (unsigned int *)rx_buffer,X_BUFFER_SIZE);

  printf("rx buffer ---\n");
  for(i=0;i<retval;i++){
    printf("%02x ",rx_buffer[i]);
    if ((i+1)%8==0) printf("\n");
  }printf("\n");

  if (retval==0) return RM_LINK_ERROR;

  // GET STATUS
  if ( (rx_buffer[3]&0xff) != 0x00){ // NOT SUCCESS
    printf("Error Ack'ed: code -- 0x%02X\n",rx_buffer[3]);
    return RM_DATA_ERROR;
  }

  return 0;
  
}

int rmap_throw_word(int sw_fd, int port,
			   struct rmap_node_info *n,
			   unsigned int tx_address,
			   unsigned int tx_data)
{
  unsigned int retval;
  retval=sw_req(sw_fd,port,RM_PCKT_CMD|RM_PCKT_WRT,
		n->src_addr,n->dest_addr,n->key,0x1234,tx_address,tx_data);
  return retval;
}

unsigned char rmap_calc_crc(void *buf,unsigned int len){
  unsigned int i;
  unsigned char crc;
  unsigned char *ptr = (unsigned char *)buf;

  /* initial CRC */
  crc=0;

  /* for each bute */
  for(i=0;i<len;i++){
    crc=RM_CRCTbl[crc ^ *ptr++];
  }
  return crc;
}

int rmap_create_buffer(unsigned int command, unsigned int tid,
		       struct rmap_node_info *n,
		       unsigned int address, unsigned int size){ // for "put" size is put_data
  int i;
  int in_length;
  unsigned int header_size;
  unsigned char *ptr, *dptr, *crc_start;

  in_length=(n->in_size+3)/4;

  ptr = tx_buffer;
  header_size=15 + in_length*4;
  for(i=0;i<n->out_size;i++)		//Destination Path Address
    *ptr++ = n->out_path[i];
  crc_start = ptr;
  *ptr++ = n->dest_addr;			//Destination Logical Address
  *ptr++ = RM_ProtoID;			//Protocol ID
  //  *ptr++ = RM_PCKT_CMD|RM_PCKT_ACK|RM_PCKT_INC|in_length;	//Packet Type
  *ptr++ = command|in_length;	//Packet Type
  *ptr++ = n->key;				//Destination Key
  for(i=0;i<(12-n->in_size)%4;i++)	//Source Path Address
    *ptr++ = 0x00;
  for(i=0;i<n->in_size;i++)
    *ptr++ = n->in_path[i];
  *ptr++ = n->src_addr;			//Source Logical Address
  *ptr++ = (tid>>8)&0xff;		//Transaction ID(MS)
  *ptr++ = (tid   )&0xff;		//Transaction ID(LS)
  *ptr++ = 0x00;			//Extended Address
  *ptr++ = (address>>24)&0xff;	//Address
  *ptr++ = (address>>16)&0xff;	//Address
  *ptr++ = (address>> 8)&0xff;	//Address
  *ptr++ = (address    )&0xff;	//Address
  if ((command & RM_PCKT_WRT)>0){
    *ptr++=0x00;		//Data Size
    *ptr++=0x00;		//Data Size
    *ptr++=0x04;		//Data Size
    *ptr++ = rmap_calc_crc(crc_start,header_size);	//Header CRC
    dptr=ptr;
    *ptr++ = (size    )&0xff;		//Data
    *ptr++ = (size>> 8)&0xff;		//Data
    *ptr++ = (size>>16)&0xff;		//Data
    *ptr++ = (size>>24)&0xff;		//Data
    *ptr++ = rmap_calc_crc(dptr,4);	//Data CRC
    return header_size + n->out_size + 6;
  }else{
    *ptr++ = (size>>16)&0xff;		//Data Size
    *ptr++ = (size>> 8)&0xff;		//Data Size
    *ptr++ = (size    )&0xff;		//Data Size
    *ptr++ = rmap_calc_crc(crc_start,header_size);	//Header CRC
    return header_size + n->out_size + 1;
  }
}
