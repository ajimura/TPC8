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
#include "TPCreaderA.h"
#include "daqmwlib.h"
#include "tpclib.h"
#include "trigio_lib.h"
#ifdef DE10
#include "swioreg.h"
#endif

using DAQMW::FatalType::DATAPATH_DISCONNECTED;
using DAQMW::FatalType::OUTPORT_ERROR;
using DAQMW::FatalType::USER_DEFINED_ERROR1; //SpaceWire
using DAQMW::FatalType::USER_DEFINED_ERROR2; //RMAP
using DAQMW::FatalType::USER_DEFINED_ERROR3; //MISC

// Module specification
// Change following items to suit your component's spec.
static const char* tpcreadera_spec[] =
{
    "implementation_id", "TPCreaderA",
    "type_name",         "TPCreaderA",
    "description",       "TPCreaderA component",
    "version",           "1.0",
    "vendor",            "Kazuo Nakayoshi, KEK",
    "category",          "example",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "2",
    "language",          "C++",
    "lang_type",         "compile",
    ""
};

TPCreaderA::TPCreaderA(RTC::Manager* manager)
    : DAQMW::DaqComponentBase(manager),
      m_OutPort("tpcreadera_out", m_out_data),
      m_recv_byte_size(0),
      m_recv_timeout_counter(0),
      m_out_status(BUF_SUCCESS),
      m_out_timeout_counter(0)
{
    // Registration: InPort/OutPort/Service

    // Set OutPort buffers
    registerOutPort("tpcreadera_out", m_OutPort);

    init_command_port();
    init_state_table();
    set_comp_name("TPCREADERA");
}

TPCreaderA::~TPCreaderA()
{
}

RTC::ReturnCode_t TPCreaderA::onInitialize()
{
    if (m_debug) {
        std::cerr << "TPCreaderA::onInitialize()" << std::endl;
    }

    return RTC::RTC_OK;
}

RTC::ReturnCode_t TPCreaderA::onExecute(RTC::UniqueId ec_id)
{
    daq_do();

    return RTC::RTC_OK;
}

int TPCreaderA::daq_dummy()
{
    return 0;
}

int TPCreaderA::daq_configure()
{
    std::cerr << "*** TPCreaderA::configure" << std::endl;
#include "daq_configure.inc"
}

int TPCreaderA::parse_params(::NVList* list)
{
#include "parse_params.inc"
}

int TPCreaderA::daq_unconfigure()
{
    std::cerr << "*** TPCreaderA::unconfigure" << std::endl;

    delete [] m_dataA; std::cout << "Delete data bufferA" << std::endl;
    delete [] m_dataB; std::cout << "Delete data bufferB" << std::endl;
    fadc_close(); std::cout << "Close Spacewire ports" << std::endl;
    if (GetETag==1) trigio_fin();
#ifdef DE10
    if (GetETag==2) swio_close();
#endif

    return 0;
}

int TPCreaderA::daq_start()
{
    std::cerr << "*** TPCreaderA::start" << std::endl;
#include "daq_start.inc"
}

int TPCreaderA::daq_stop()
{
    std::cerr << "*** TPCreaderA::stop" << std::endl;
#include "daq_stop.inc"
}

int TPCreaderA::daq_pause()
{
    std::cerr << "*** TPCreaderA::pause" << std::endl;
#include "daq_pause.inc"
}

int TPCreaderA::daq_resume()
{
    std::cerr << "*** TPCreaderA::resume" << std::endl;
#include "daq_resume.inc"
}

int TPCreaderA::read_data_from_detectors()
{
#include "read_data_from_detectors.inc"
}

int TPCreaderA::set_data(int data_byte_size)
{
#include "set_data.inc"
}

int TPCreaderA::write_OutPort()
{
  bool ret;
  struct timespec ts;
  double t0=0.,t1;

  if (m_debug) {
    std::cerr << "write: StockNum=" << Stock_CurNum << " SockSize=" << Stock_Offset << std::endl;
  }
 
  if (m_debug) {
    clock_gettime(CLOCK_MONOTONIC,&ts);
    t0=(ts.tv_sec*1.)+(ts.tv_nsec/1000000000.);
  }

  if ((ret=m_OutPort.write()) == false) {  // TIMEOUT or FATAL
    m_out_status  = check_outPort_status(m_OutPort);
    if (m_out_status == BUF_FATAL) fatal_error_report(OUTPORT_ERROR);    // Fatal error
    if (m_out_status == BUF_TIMEOUT) return -1;    // Timeout
    if (m_out_status == BUF_NOBUF) return -1;    // No Buffer on Downstream
  } else {
    m_out_status = BUF_SUCCESS; // successfully done
  }

  if (m_debug) {
    clock_gettime(CLOCK_MONOTONIC,&ts);
    t1=(ts.tv_sec*1.)+(ts.tv_nsec/1000000000.);
    std::cout << "write time: " << std::fixed << std::setprecision(9) << t1-t0 << std::endl;
  }

  return 0;
}

int TPCreaderA::daq_run()
{
#include "daq_run.inc"
}

extern "C"
{
    void TPCreaderAInit(RTC::Manager* manager)
    {
        RTC::Properties profile(tpcreadera_spec);
        manager->registerFactory(profile,
                    RTC::Create<TPCreaderA>,
                    RTC::Delete<TPCreaderA>);
    }
};

void TPCreaderA::switch_buffer(){
  if (SwitchAB==0){
    m_data1=m_dataB;          m_data4=(unsigned int *)m_dataB;
    m_resv1=m_dataA;          m_resv4=(unsigned int *)m_dataA;
    SwitchAB=1;
  }else{
    m_data1=m_dataA;          m_data4=(unsigned int *)m_dataA;
    m_resv1=m_dataB;          m_resv4=(unsigned int *)m_dataB;
    SwitchAB=0;
  }
}
