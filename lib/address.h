#define ChBase 0x1000000
#define BufBase 0x100000
#define MemBase 0x48000

// Register base address
#define L1D 0x00000
#define LTG 0x10000
#define TGC 0x20000
#define ROC 0x30000
#define EBM 0x40000
#define ASC 0x50000

#define L1D_Delay 0x00000
#define L1D_Delayed 0x00004
//#define L1D_ChEnable 0x00004
//#define L1D_Prompt 0x00008
//#define L1D_Delayed 0x0000c

//#define LTG_ThresHigh 0x10000
//#define LTG_ThresLow 0x10004
//#define LTG_EventCount  0x10008
//#define LTG_Width 0x1000C
//#define LTG_TrigSelect 0x10010

#define TGC_FreeBuf 0x20000
#define TGC_NextBuf 0x20004
#define TGC_Count 0x20008
#define TGC_CountEach 0x2000C
#define TGC_TrigEnab 0x20010
#define TGC_TrigInOut 0x20014
#define TGC_BufEnab 0x20018
#define TGC_ClkTrig 0x2001C
#define TGC_CntRst 0x20020
#define TGC_TrigID 0x20024
#define TGC_FClk 0x20028
#define TGC_CClk 0x2002C
#define TGC_IOstat 0x20030
#define TGC_NoEmpty 0x20034
#define TGC_Busy 0x20038

#define ROC_Ready 0x30000
#define ROC_Done 0x30004

#define EBM_Count 0x40000
#define EBM_DataSize 0x40004
#define EBM_TotSize 0x40008
#define EBM_CntRst 0x4000C
#define EBM_Status 0x40010
#define EBM_Range 0x40080
#define EBM_Thres 0x40084
#define EBM_CmpType 0x40088
#define EBM_PExcess 0x4008C
#define EBM_DExcess 0x40090

#define TMP_Tmp 0x60000
#define TMP_Conf 0x60004

#define CMN_Version 0xf0000
#define CMN_HardRst 0xf0000
#define CMN_Reset 0xf0004
#define CMN_LogAddr 0xf0008
#define CMN_SpWStatus0 0xf0010
#define CMN_SpWStatus1 0xf0014
#define CMN_SpWStatus2 0xf0018

