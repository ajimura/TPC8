// -*- C++ -*-
/*!
 * @file
 * @brief
 * @date
 * @author
 *
 */

#ifndef MERGER2TO1_H
#define MERGER2TO1_H

#include "DaqComponentBase.h"

using namespace RTC;

class Merger2to1
    : public DAQMW::DaqComponentBase
{
public:
    Merger2to1(RTC::Manager* manager);
    ~Merger2to1();

    // The initialize action (on CREATED->ALIVE transition)
    // formaer rtc_init_entry()
    virtual RTC::ReturnCode_t onInitialize();

    // The execution action that is invoked periodically
    // former rtc_active_do()
    virtual RTC::ReturnCode_t onExecute(RTC::UniqueId ec_id);

private:
    TimedOctetSeq          m_in1_data;
    InPort<TimedOctetSeq>  m_InPort1;

    TimedOctetSeq          m_in2_data;
    InPort<TimedOctetSeq>  m_InPort2;

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
    int reset_InPort1();
    int reset_InPort2();
  void Stock_data(int data1_byte_size, int data2_byte_size);
  int set_data(int data_byte_size);
  int read_InPort1();
  int read_InPort2();
  int write_OutPort();
  unsigned char * renew_buf(unsigned char *orig_buf, size_t cursize, size_t newsize);

  static const unsigned int ComponentType = 420;

    BufferStatus m_in1_status;
    BufferStatus m_in2_status;
    BufferStatus m_out_status;

  int m_in1_timeout_counter;
  int m_in2_timeout_counter;
  int m_out_timeout_counter;

  int m_inport1_recv_data_size;
  int m_inport2_recv_data_size;

    bool m_debug;

  int ComponentID;

  int In1_TotSiz;
  int In1_RemainSiz;
  unsigned int *In1_CurPos;
  int In1_Done;

  int In2_TotSiz;
  int In2_RemainSiz;
  unsigned int *In2_CurPos;
  int In2_Done;

  int ReadTimeout;
  int Stock_MaxNum;
  int Stock_MaxSiz;
  int Stock_CurNum;
  int Stock_Offset;
  unsigned char * m_data1;
  unsigned int * m_data4;

  unsigned char *DataPos1; //Data pointer ready to use
  unsigned char *DataPos2; //Data pointer ready to use
  int Cur_MaxDataSiz1;
  int Cur_MaxDataSiz2;

  int Cur_MaxDataSiz;

  int OutCompress;
  int CompressLevel;

};


extern "C"
{
    void Merger2to1Init(RTC::Manager* manager);
};

#endif // MERGER2TO1_H
