/* by Shuhei Ajimura */

//#ifndef ___rmap_lib
//#include "rmap_lib.h"
//#endif

#define FADC_Zero 0x00000101
#define FADC_Mark 0x00000202
#define FADC_Dip 0x00000404
#define FADC_Peak 0x00000808
#define FADC_Sup  0x00001010

#define FADC_ExtGTrigIn 0x00000010
#define FADC_ExtRstIn 0x00000020
#define FADC_ExtBusyIn 0x00000040
#define FADC_ExtLTrigIn 0x00000080

#define FADC_TrigOut 0x00000008
#define FADC_BsyOut  0x00000004

#define FADC_ExtRst   0x00000008
#define FADC_ExtTrig 0x00000004
#define FADC_ClkTrig 0x00000002
#define FADC_IntTrig 0x00000001

#ifdef __cplusplus
extern "C" {
#endif

int sw_open_check(int *);
void fadc_close(void);
void fadc_set_adc_info(void);
int fadc_hardreset_all(void);
int fadc_softreset_all(void);
int fadc_init_all_adc(void);
int fadc_node_init(int, int);
int fadc_node_def(int, int);
int fadc_setup_all(void);
void fadc_set_thres_zero(void);
int fadc_read_thres(const char *, int);
int fadc_reset_trigcount(void);
int fadc_set_comp_all(int);
int fadc_disable_localtrig_all(void);
int fadc_enable_localtrig_all(void);
int fadc_read_set_trigio(const char *);
int fadc_read_readychk(const char *);
int fadc_trig_enable(int);
int fadc_trig_disable(void);
int fadc_wait_data_ready(void);
int fadc_wait_data_ready_all(void);
int fadc_wait_data_ready_sel(void);
int fadc_show_buf_stat(int);
int fadc_get_data_size(void);
int fadc_get_totsize(void);
int fadc_get_totsizeM(void);
int fadc_get_totsizeM2(void);
int fadc_get_event_data(unsigned int *, int);
int fadc_get_event_dataM(unsigned int *, int);
int fadc_get_event_dataM1(unsigned int *, int);
int fadc_get_event_dataM2(unsigned int *, int);
int fadc_release_buffer(void);
void fadc_prepare_next(void);
int fadc_init_temp(void);
int fadc_get_temp_each(int, int, int *);
void fadc_check_roc_trig(void);
void fadc_show_roc(void);
void fadc_check_busy(void);
void fadc_check_default_value(void);
void fadc_set_default_value(void);
void fadc_set_full_range(unsigned int);
void fadc_check_TGC(int, int);

#ifdef __cplusplus
};
#endif
