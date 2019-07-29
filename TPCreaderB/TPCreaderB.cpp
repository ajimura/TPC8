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
#include "TPCreaderB.h"
#include "daqmwlib.h"
#include "tpclib.h"
#include "trigio_lib.h"
#include "swioreg.h"

using DAQMW::FatalType::DATAPATH_DISCONNECTED;
using DAQMW::FatalType::OUTPORT_ERROR;
using DAQMW::FatalType::USER_DEFINED_ERROR1; //SpaceWire
using DAQMW::FatalType::USER_DEFINED_ERROR2; //RMAP
using DAQMW::FatalType::USER_DEFINED_ERROR3; //MISC

// Module specification
// Change following items to suit your component's spec.
static const char* tpcreaderb_spec[] =
{
    "implementation_id", "TPCreaderB",
    "type_name",         "TPCreaderB",
    "description",       "TPCreaderB component",
    "version",           "1.0",
    "vendor",            "Kazuo Nakayoshi, KEK",
    "category",          "example",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "2",
    "language",          "C++",
    "lang_type",         "compile",
    ""
};

TPCreaderB::TPCreaderB(RTC::Manager* manager)
    : DAQMW::DaqComponentBase(manager),
      m_OutPort("tpcreaderb_out", m_out_data),
      m_recv_byte_size(0),
      m_recv_timeout_counter(0),
      m_out_status(BUF_SUCCESS),
      m_out_timeout_counter(0)
{
    // Registration: InPort/OutPort/Service

    // Set OutPort buffers
    registerOutPort("tpcreaderb_out", m_OutPort);

    init_command_port();
    init_state_table();
    set_comp_name("TPCREADERB");
}

TPCreaderB::~TPCreaderB()
{
}

RTC::ReturnCode_t TPCreaderB::onInitialize()
{
    if (m_debug) {
        std::cerr << "TPCreaderB::onInitialize()" << std::endl;
    }

    return RTC::RTC_OK;
}

RTC::ReturnCode_t TPCreaderB::onExecute(RTC::UniqueId ec_id)
{
    daq_do();

    return RTC::RTC_OK;
}

int TPCreaderB::daq_dummy()
{
    return 0;
}

int TPCreaderB::daq_configure()
{
    std::cerr << "*** TPCreaderB::configure" << std::endl;
#include "daq_configure.inc"
}

int TPCreaderB::parse_params(::NVList* list)
{
#include "parse_params.inc"
}

int TPCreaderB::daq_unconfigure()
{
    std::cerr << "*** TPCreaderB::unconfigure" << std::endl;

    delete [] m_dataA; std::cout << "Delete data bufferA" << std::endl;
    delete [] m_dataB; std::cout << "Delete data bufferB" << std::endl;
    fadc_close(); std::cout << "Close Spacewire ports" << std::endl;
    if (GetETag==1) trigio_fin();
    if (GetETag==2) swio_close();

    return 0;
}

int TPCreaderB::daq_start()
{
    std::cerr << "*** TPCreaderB::start" << std::endl;
#include "daq_start.inc"
}

int TPCreaderB::daq_stop()
{
    std::cerr << "*** TPCreaderB::stop" << std::endl;
#include "daq_stop.inc"
}

int TPCreaderB::daq_pause()
{
    std::cerr << "*** TPCreaderB::pause" << std::endl;
#include "daq_pause.inc"
}

int TPCreaderB::daq_resume()
{
    std::cerr << "*** TPCreaderB::resume" << std::endl;
#include "daq_resume.inc"
}

int TPCreaderB::read_data_from_detectors()
{
#include "read_data_from_detectors.inc"
}

int TPCreaderB::set_data(int data_byte_size)
{
#include "set_data.inc"
}

int TPCreaderB::write_OutPort()
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

int TPCreaderB::daq_run()
{
#include "daq_run.inc"
}

extern "C"
{
    void TPCreaderBInit(RTC::Manager* manager)
    {
        RTC::Properties profile(tpcreaderb_spec);
        manager->registerFactory(profile,
                    RTC::Create<TPCreaderB>,
                    RTC::Delete<TPCreaderB>);
    }
};

void TPCreaderB::switch_buffer(){
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
