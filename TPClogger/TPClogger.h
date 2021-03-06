// -*- C++ -*-
/*!
 * @file TPClogger.h
 * @brief Event data logging component.
 * @date
 * @author Kazuo Nakayoshi <kazuo.nakayoshi@kek.jp>
 *
 * Copyright (C) 2011
 *     Kazuo Nakayoshi
 *     Electronics System Group,
 *     KEK, Japan.
 *     All rights reserved.
 *
 */

#ifndef TPCLOGGER_H
#define TPCLOGGER_H

#include "DaqComponentBase.h"
#include "FileUtils.h"

using namespace RTC;

class TPClogger
    : public DAQMW::DaqComponentBase
{
public:
    TPClogger(RTC::Manager* manager);
    virtual ~TPClogger();

    // The initialize action (on CREATED->ALIVE transition)
    virtual RTC::ReturnCode_t onInitialize();

    // The execution action that is invoked periodically
    virtual RTC::ReturnCode_t onExecute(RTC::UniqueId ec_id);

private:
    RTC::TimedOctetSeq m_in_data;
    ///InPort<TimedOctetSeq, MyRingBuffer> m_InPort;
    RTC::InPort< RTC::TimedOctetSeq > m_InPort;

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
    unsigned int read_InPort();
    void toLower(std::basic_string<char>& s);
    unsigned char * renew_buf(unsigned char *orig_buf, size_t cursize, size_t newsize);

    FileUtils* fileUtils;
    bool m_isDataLogging;
    bool m_filesOpened;
    std::string m_dirName;
    unsigned int m_maxFileSizeInMByte;
    BufferStatus m_in_status;
    int m_in_timeout_counter;
    int m_update_rate;
    bool m_debug;

  static const unsigned int ComponentType = 430;
  int ComponentID;
  int In_TotSiz;
  int In_RemainSiz;
  unsigned int *In_CurPos; //Data pointer before decompressed in case
  int In_Done;

  unsigned char *DataPos1; //Data pointer ready to use
  int Cur_MaxDataSiz;
};

extern "C"
{
    void TPCloggerInit(RTC::Manager* manager);
};

#endif // TPCLOGGER_H
