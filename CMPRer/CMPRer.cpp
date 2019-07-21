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
#include "CMPRer.h"
#include "daqmwlib.h"

using DAQMW::FatalType::INPORT_ERROR;
using DAQMW::FatalType::OUTPORT_ERROR;
using DAQMW::FatalType::USER_DEFINED_ERROR1;

// Module specification
// Change following items to suit your component's spec.
static const char* cmprer_spec[] =
{
    "implementation_id", "CMPRer",
    "type_name",         "CMPRer",
    "description",       "CMPRer component",
    "version",           "1.0",
    "vendor",            "Kazuo Nakayoshi, KEK",
    "category",          "example",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "10",
    "language",          "C++",
    "lang_type",         "compile",
    ""
};

CMPRer::CMPRer(RTC::Manager* manager)
    : DAQMW::DaqComponentBase(manager),
      m_InPort("cmprer_in", m_in_data),
      m_OutPort("cmprer_out", m_out_data),

      m_in_status(BUF_SUCCESS),
      m_out_status(BUF_SUCCESS),
      m_in_timeout_counter(0),
      m_out_timeout_counter(0),
      m_inport_recv_data_size(0)
{
    registerInPort("cmprer_in", m_InPort);
    registerOutPort("cmprer_out", m_OutPort);

    init_command_port();
    init_state_table();
    set_comp_name("CMPRER");
}

CMPRer::~CMPRer()
{
}


RTC::ReturnCode_t CMPRer::onInitialize()
{
    if (m_debug) {
        std::cerr << "CMPRer::onInitialize()" << std::endl;
    }

    return RTC::RTC_OK;
}

RTC::ReturnCode_t CMPRer::onExecute(RTC::UniqueId ec_id)
{
  //    std::cerr << "*** onExecute\n";
    daq_do();

    return RTC::RTC_OK;
}

int CMPRer::daq_dummy()
{
    return 0;
}

int CMPRer::daq_configure()
{
    std::cerr << "*** CMPRer::configure" << std::endl;
#include "daq_configure.inc"
}

int CMPRer::parse_params(::NVList* list)
{
#include "parse_params.inc"
}


int CMPRer::daq_unconfigure()
{
    std::cerr << "*** CMPRer::unconfigure" << std::endl;

    //    delete [] m_data1; std::cout << "Delete data buffer" << std::endl;

    return 0;
}

int CMPRer::daq_start()
{
    std::cerr << "*** CMPRer::start" << std::endl;

    m_in_status  = BUF_SUCCESS;
    m_out_status = BUF_SUCCESS;

    return 0;
}

int CMPRer::daq_stop()
{
    std::cerr << "*** CMPRer::stop" << std::endl;
    reset_InPort();
    return 0;
}

int CMPRer::daq_pause()
{
    std::cerr << "*** CMPRer::pause" << std::endl;
    return 0;
}

int CMPRer::daq_resume()
{
    std::cerr << "*** CMPRer::resume" << std::endl;
    return 0;
}

int CMPRer::reset_InPort()
{
     int ret = true;
     while (ret == true) {
         ret = m_InPort.read();
     }
     m_inport_recv_data_size=0;

    std::cerr << "*** CMPRer::InPort flushed\n";
    return 0;
}

int CMPRer::write_OutPort()
{
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
    return 0; // successfully done
}

int CMPRer::read_InPort()
{
#include "read_InPort.inc"
}

int CMPRer::set_data(int data_byte_size)
{
#include "set_data.inc"
}

int CMPRer::daq_run()
{
#include "daq_run.inc"
}

unsigned char * CMPRer::renew_buf(unsigned char * orig_buf,
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
    void CMPRerInit(RTC::Manager* manager)
    {
        RTC::Properties profile(cmprer_spec);
        manager->registerFactory(profile,
                    RTC::Create<CMPRer>,
                    RTC::Delete<CMPRer>);
    }
};
