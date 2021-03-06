// -*- C++ -*-
/*!
 * @file 
 * @brief
 * @date
 * @author
 *
 */

#ifndef DUMGENA_H
#define DUMGENA_H

#include "DaqComponentBase.h"

using namespace RTC;

static const int CompHeaderSize = 13; // <- should be modified

class DumGenA
    : public DAQMW::DaqComponentBase
{
public:
    DumGenA(RTC::Manager* manager);
    ~DumGenA();

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
    void toLower(std::basic_string<char>& s);

  static const unsigned int ComponentType = 440;

  //    static const int SEND_BUFFER_SIZE = 4096;
  //    unsigned char m_data[SEND_BUFFER_SIZE];
  unsigned int *m_data4;
  unsigned char *m_data1;

  int m_recv_byte_size;
  int m_recv_timeout_counter;

  BufferStatus m_out_status;
  int m_out_timeout_counter;
  bool m_debug;

  //parameter
  int generate_size;
  int interval_time;
  int ComponentID;
  int ReadTimeout;
  int Stock_MaxNum;
  int Stock_MaxSiz;
  int Stock_CurNum;
  int Stock_Offset;

  int OutCompress;
  int CompressLevel;

  unsigned int eventnum;
};


extern "C"
{
    void DumGenAInit(RTC::Manager* manager);
};

#endif // DUMGENA_H
