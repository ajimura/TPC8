// -*- C++ -*-
/*!
 * @file
 * @brief
 * @date
 * @author
 *
 */

#include <iomanip>
#include <ctime>
#include <zlib.h>
#include <lz4.h>
#include <zstd.h>
#include "TXer.h"
#include "daqmwlib.h"

using DAQMW::FatalType::INPORT_ERROR;
using DAQMW::FatalType::OUTPORT_ERROR;
using DAQMW::FatalType::USER_DEFINED_ERROR1;

// Module specification
// Change following items to suit your component's spec.
static const char* txer_spec[] =
{
    "implementation_id", "TXer",
    "type_name",         "TXer",
    "description",       "TXer component",
    "version",           "1.0",
    "vendor",            "Kazuo Nakayoshi, KEK",
    "category",          "example",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "10",
    "language",          "C++",
    "lang_type",         "compile",
    ""
};

TXer::TXer(RTC::Manager* manager)
    : DAQMW::DaqComponentBase(manager),
      m_InPort("txer_in", m_in_data),
      m_OutPort("txer_out", m_out_data),

      m_in_status(BUF_SUCCESS),
      m_out_status(BUF_SUCCESS),
      m_in_timeout_counter(0),
      m_out_timeout_counter(0),
      m_inport_recv_data_size(0)
{
    registerInPort("txer_in", m_InPort);
    registerOutPort("txer_out", m_OutPort);

    init_command_port();
    init_state_table();
    set_comp_name("TXER");
}

TXer::~TXer()
{
}


RTC::ReturnCode_t TXer::onInitialize()
{
    if (m_debug) {
        std::cerr << "TXer::onInitialize()" << std::endl;
    }

    return RTC::RTC_OK;
}

RTC::ReturnCode_t TXer::onExecute(RTC::UniqueId ec_id)
{
  //    std::cerr << "*** onExecute\n";
    daq_do();

    return RTC::RTC_OK;
}

int TXer::daq_dummy()
{
    return 0;
}

int TXer::daq_configure()
{
    std::cerr << "*** TXer::configure" << std::endl;
#include "daq_configure.inc"
}

int TXer::parse_params(::NVList* list)
{
#include "parse_params.inc"
}


int TXer::daq_unconfigure()
{
    std::cerr << "*** TXer::unconfigure" << std::endl;

    delete [] DataPos1;
    delete [] m_data1; std::cout << "Delete data buffer" << std::endl;

    return 0;
}

int TXer::daq_start()
{
    std::cerr << "*** TXer::start" << std::endl;

    m_in_status  = BUF_SUCCESS;
    m_out_status = BUF_SUCCESS;

    return 0;
}

int TXer::daq_stop()
{
    std::cerr << "*** TXer::stop" << std::endl;
    reset_InPort();
    return 0;
}

int TXer::daq_pause()
{
    std::cerr << "*** TXer::pause" << std::endl;
    return 0;
}

int TXer::daq_resume()
{
    std::cerr << "*** TXer::resume" << std::endl;
    return 0;
}

int TXer::reset_InPort()
{
     int ret = true;
     while (ret == true) {
         ret = m_InPort.read();
     }
     m_inport_recv_data_size=0;

    std::cerr << "*** TXer::InPort flushed\n";
    return 0;
}

int TXer::write_OutPort()
{
  struct timespec ts;
  double t0=0.,t1;

  if (m_debug) {
    clock_gettime(CLOCK_MONOTONIC,&ts);
    t0=(ts.tv_sec*1.)+(ts.tv_nsec/1000000000.);
  }

    ////////////////// send data from OutPort  //////////////////
    bool ret = m_OutPort.write();

    //////////////////// check write status /////////////////////
    if (ret == false) {  // TIMEOUT or FATAL
        m_out_status  = check_outPort_status(m_OutPort);
        if (m_out_status == BUF_FATAL) {   // Fatal error
            fatal_error_report(OUTPORT_ERROR);
        }
        if (m_out_status == BUF_TIMEOUT) { // Timeout
            m_out_timeout_counter++;
            return -1;
        }
        if (m_out_status == BUF_NOBUF) { // No Buffer on Downstream
            return -1;
        }
    }
    else { // success
      m_out_timeout_counter = 0;
      m_out_status = BUF_SUCCESS; // successfully done
    }

  if (m_debug) {
    clock_gettime(CLOCK_MONOTONIC,&ts);
    t1=(ts.tv_sec*1.)+(ts.tv_nsec/1000000000.);
    std::cout << "write time: " << std::fixed << std::setprecision(9) << t1-t0 << std::endl;
  }

    return 0; // successfully done
}

int TXer::read_InPort()
{
#include "read_InPort.inc"
}

void TXer::Stock_data(int data_byte_size)
{
#include "Stock_data.inc"
}

int TXer::set_data(int data_byte_size)
{
#include "set_data.inc"
}

int TXer::daq_run()
{
#include "daq_run.inc"
}

unsigned char * TXer::renew_buf(unsigned char * orig_buf,
				      size_t cursize, size_t newsize)
{
  unsigned char * new_buf;

  try{
    new_buf = new unsigned char[newsize];
  }
  catch(std::bad_alloc){
    std::cerr << "Bad allocation..." << std::endl;
    fatal_error_report(USER_DEFINED_ERROR1);
  }
  catch(...){
    std::cerr << "Got exception..." << std::endl;
    fatal_error_report(USER_DEFINED_ERROR1);
  }

  memcpy(new_buf, orig_buf, cursize);
  delete [] orig_buf;

  std::cerr << "Re-new data buf: " << cursize << " -> " << newsize << std::endl;

  return new_buf;
}

extern "C"
{
    void TXerInit(RTC::Manager* manager)
    {
        RTC::Properties profile(txer_spec);
        manager->registerFactory(profile,
                    RTC::Create<TXer>,
                    RTC::Delete<TXer>);
    }
};
