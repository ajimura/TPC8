// -*- C++ -*-
/*!
 * @file 
 * @brief
 * @date
 * @author
 *
 */

#ifndef TPCREADERC_H
#define TPCREADERC_H

#include "DaqComponentBase.h"

using namespace RTC;

//size: unit in byte
static const int CompHeaderSize = 80;
static const int FADCHeaderSize = 28;
static const int MaxFADCSize = 16384;

class TPCreaderC
  : public DAQMW::DaqComponentBase
{
public:
  TPCreaderC(RTC::Manager* manager);
  ~TPCreaderC();

  // The initialize action (on CREATED->ALIVE transition)
  // former rtc_init_entry()
  virtual RTC::ReturnCode_t onInitialize();

  // The execution action that is invoked periodically
  // former rtc_active_do()
  virtual RTC::ReturnCode_t onExecute(RTC::UniqueId ec_id);

private:
  TimedOctetSeq          m_out_data;
  OutPort<TimedOctetSeq> m_OutPort;

private:
  int daq_dummy();
  int daq_configure();
  int daq_unconfigure();
  int daq_start();
  int daq_run();
  int daq_stop();
  int daq_pause();
  int daq_resume();

  int parse_params(::NVList* list);
  int read_data_from_detectors();
  int set_data(int data_byte_size);
  int write_OutPort();
  void toLower(std::basic_string<char>& s){
    for(std::basic_string<char>::iterator p=s.begin();p!=s.end();++p){*p=tolower(*p);}
  }
  void switch_buffer();

  static const unsigned int ComponentType = 410;

  //data buffer
  unsigned char *m_dataA, *m_dataB;	// double buffer
  unsigned char *m_data1;		// main buffer
  unsigned int *m_data4;
  unsigned char *m_resv1;		// reserver buffer
  unsigned int *m_resv4;

  int m_recv_byte_size;
  int m_recv_timeout_counter;

  BufferStatus m_out_status;
  int m_out_timeout_counter;

  bool m_debug;

  //parameter for TPC readout from DAQ op.
  int totNumFADC;
  int NumFADC[8];
  unsigned int totNumFADCforHeader1;
  unsigned int totNumFADCforHeader2;
  bool pedestal_exist;
  std::string pedestal_file;
  int ThresMargin;
  bool trigio_exist;
  std::string trigio_file;
  bool rdychk_exist;
  std::string rdychk_file;
  int ComprType;
  int TrigEnab;
  std::string TrigIOIP;
  int TrigIOPort;
  bool TrigIO_done;
  int TrigIO_waittime;
  int ComponentID;
  int ReadTimeout;

  int Stock_MaxNum;
  int Stock_MaxSiz;
  int Stock_CurNum;
  int Stock_Offset;

  int OutCompress;
  int CompressLevel;

  int SwitchAB;
  int Resv_In, Resv_Size;

  unsigned int eventnum;

};


extern "C"
{
    void TPCreaderCInit(RTC::Manager* manager);
};

#endif // TPCREADERC_H
