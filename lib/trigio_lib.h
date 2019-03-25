/* by Shuhei Ajimura */

#ifdef __cplusplus
extern "C" {
#endif

int trigio_ini(const char *, unsigned int);
int trigio_fin();
int trigio_reset_count();
int trigio_read_eventtag(unsigned char *);
int trigio_read_done();
int trigio_wr(unsigned int , unsigned char* , unsigned char);
int trigio_rd(unsigned int , unsigned char* , unsigned char);

#ifdef __cplusplus
};
#endif
