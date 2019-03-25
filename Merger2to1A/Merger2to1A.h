// -*- C++ -*-
/*!
 * @file
 * @brief
 * @date
 * @author
 *
 */

#ifndef MERGER2TO1A_H
#define MERGER2TO1A_H

#include "DaqComponentBase.h"

using namespace RTC;

class Merger2to1A
    : public DAQMW::DaqComponentBase
{
public:
    Merger2to1A(RTC::Manager* manager);
    ~Merger2to1A();

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
  int set_data_OutPort(unsigned int data1_byte_size,unsigned int data2_byte_size);
  unsigned int read_InPort1();
  unsigned int read_InPort2();
  int write_OutPort();

  static const unsigned int ComponentType = 420; // <- should be modified

    BufferStatus m_in1_status;
    BufferStatus m_in2_status;
    BufferStatus m_out_status;

  unsigned int m_in1_timeout_counter;
  unsigned int m_in2_timeout_counter;
  unsigned int m_out_timeout_counter;

  unsigned int m_inport1_recv_data_size;
  unsigned int m_inport2_recv_data_size;

    bool m_debug;

  int ComponentID;
};


extern "C"
{
    void Merger2to1AInit(RTC::Manager* manager);
};

#endif // MERGER2TO1A_H
