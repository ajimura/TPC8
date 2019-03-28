/* by Shuhei Ajimura */

#define ___rmap_lib

#define RM_LINK_ERROR -1
#define RM_DATA_ERROR -2
#define RM_BUF_OVERFLOW -3
#define RM_BUF_NONE -4

struct rmap_node_info {
  int out_size; unsigned char out_path[12];
  int  in_size; unsigned char  in_path[12];
  unsigned char dest_addr;
  unsigned char src_addr;
  unsigned char key;
};

int rmap_get_data0(int, int, struct rmap_node_info *, unsigned int, unsigned int *, unsigned int);
int rmap_get_data(int, int, struct rmap_node_info *, unsigned int, unsigned int *, unsigned int);
int rmap_get_data_verbose(int, int, struct rmap_node_info *, unsigned int, unsigned int *, unsigned int);
int rmap_get_data_W(int, int, struct rmap_node_info *, unsigned int, unsigned int *, unsigned int);
int rmap_put_word(int, int, struct rmap_node_info *, unsigned int, unsigned int);
int rmap_put_word0(int, int, struct rmap_node_info *, unsigned int, unsigned int);
int rmap_put_word_verbose(int, int, struct rmap_node_info *, unsigned int, unsigned int);
int rmap_req_data0(int, int, struct rmap_node_info *, unsigned int, unsigned int, unsigned int);
int rmap_req_data(int, int, struct rmap_node_info *, unsigned int, unsigned int, unsigned int);
int rmap_rcv_all(int, int, unsigned int, unsigned int *, unsigned int *);
int rmap_rcv_header(int, int, unsigned int *, unsigned int *);
int rmap_rcv_data(unsigned int, unsigned int *);
int rmap_clr_data();
