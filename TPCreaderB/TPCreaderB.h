// -*- C++ -*-
/*!
 * @file 
 * @brief
 * @date
 * @author
 *
 */

#ifndef TPCREADERB_H
#define TPCREADERB_H

#include "DaqComponentBase.h"

using namespace RTC;

static const int CompHeaderSize = 13; // <- should be modified

class TPCreaderB
    : public DAQMW::DaqComponentBase
{
public:
    TPCreaderB(RTC::Manager* manager);
    ~TPCreaderB();

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
    int set_data(unsigned int data_byte_size);
    int write_OutPort();
    void toLower(std::basic_string<char>& s);

  static const unsigned int ComponentType = 410; // <- should be modified

  //    static const int SEND_BUFFER_SIZE = 4096;
  //    unsigned char m_data[SEND_BUFFER_SIZE];
  unsigned int *m_data4;
  unsigned char *m_data1;

    unsigned int m_recv_byte_size;

    BufferStatus m_out_status;
    bool m_debug;

  //size: unit in 4bytes
  static const int ChHeaderSize = 2;
  static const int FADCHeaderSize = 7;
  static const int EventHeaderSize = 4;

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

  unsigned int eventnum;
};


extern "C"
{
    void TPCreaderBInit(RTC::Manager* manager);
};

#endif // TPCREADERB_H
