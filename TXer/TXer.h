// -*- C++ -*-
/*!
 * @file
 * @brief
 * @date
 * @author
 *
 */

#ifndef TXER_H
#define TXER_H

#include "DaqComponentBase.h"

using namespace RTC;

class TXer
    : public DAQMW::DaqComponentBase
{
public:
    TXer(RTC::Manager* manager);
    ~TXer();

    // The initialize action (on CREATED->ALIVE transition)
    // formaer rtc_init_entry()
    virtual RTC::ReturnCode_t onInitialize();

    // The execution action that is invoked periodically
    // former rtc_active_do()
    virtual RTC::ReturnCode_t onExecute(RTC::UniqueId ec_id);

private:
    TimedOctetSeq          m_in_data;
    InPort<TimedOctetSeq>  m_InPort;

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
    int reset_InPort();
  void Stock_data(int data_byte_size);
  int set_data(int data_byte_size);
  int read_InPort();
  int write_OutPort();
  unsigned char * renew_buf(unsigned char *orig_buf, size_t cursize, size_t newsize);

    BufferStatus m_in_status;
    BufferStatus m_out_status;

  int m_in_timeout_counter;
  int m_out_timeout_counter;

  int m_inport_recv_data_size;

    bool m_debug;

  int In_TotSiz;
  int In_RemainSiz;
  unsigned int *In_CurPos;
  int In_Done;

  int ReadTimeout;
  int Stock_MaxNum;
  int Stock_MaxSiz;
  int Stock_CurNum;
  int Stock_Offset;
  unsigned char * m_data1;
  unsigned int * m_data4;
  int Cur_MaxDataSiz;

  unsigned char *DataPos1; //Data pointer ready to use
  int Cur_MaxDataSiz1;

  int OutCompress;
  int CompressLevel;

};


extern "C"
{
    void TXerInit(RTC::Manager* manager);
};

#endif // TXER_H
