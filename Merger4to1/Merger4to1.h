// -*- C++ -*-
/*!
 * @file
 * @brief
 * @date
 * @author
 *
 */

#ifndef MERGER4TO1_H
#define MERGER4TO1_H

#include "DaqComponentBase.h"

using namespace RTC;

class Merger4to1
    : public DAQMW::DaqComponentBase
{
public:
    Merger4to1(RTC::Manager* manager);
    ~Merger4to1();

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

    TimedOctetSeq          m_in3_data;
    InPort<TimedOctetSeq>  m_InPort3;

    TimedOctetSeq          m_in4_data;
    InPort<TimedOctetSeq>  m_InPort4;

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
    int reset_InPort3();
    int reset_InPort4();
  void Stock_data(int, int, int, int);
  int set_data(int data_byte_size);
  int read_InPort1();
  int read_InPort2();
  int read_InPort3();
  int read_InPort4();
  int write_OutPort();
  unsigned char * renew_buf(unsigned char *orig_buf, unsigned int cursize, unsigned int newsize);

  static const unsigned int ComponentType = 421; // <- should be modified

    BufferStatus m_in1_status;
    BufferStatus m_in2_status;
    BufferStatus m_in3_status;
    BufferStatus m_in4_status;
    BufferStatus m_out_status;

  int m_in1_timeout_counter;
  int m_in2_timeout_counter;
  int m_in3_timeout_counter;
  int m_in4_timeout_counter;
  int m_out_timeout_counter;

  int m_inport1_recv_data_size;
  int m_inport2_recv_data_size;
  int m_inport3_recv_data_size;
  int m_inport4_recv_data_size;

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

  int In3_TotSiz;
  int In3_RemainSiz;
  unsigned int *In3_CurPos;
  int In3_Done;

  int In4_TotSiz;
  int In4_RemainSiz;
  unsigned int *In4_CurPos;
  int In4_Done;

  int ReadTimeout;
  int Stock_MaxNum;
  int Stock_CurNum;
  int Stock_TotSiz;
  int Stock_Offset;
  unsigned char * m_data1;
  unsigned int * m_data4;

  int Cur_MaxDataSiz;
};

extern "C"
{
    void Merger4to1Init(RTC::Manager* manager);
};

#endif // MERGER4TO1_H
