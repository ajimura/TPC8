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
#include "TPCreader.h"
#include "daqmwlib.h"
#include "tpclib.h"
#include "trigio_lib.h"

using DAQMW::FatalType::DATAPATH_DISCONNECTED;
using DAQMW::FatalType::OUTPORT_ERROR;
using DAQMW::FatalType::USER_DEFINED_ERROR1; //SpaceWire
using DAQMW::FatalType::USER_DEFINED_ERROR2; //RMAP
using DAQMW::FatalType::USER_DEFINED_ERROR3; //MISC

// Module specification
// Change following items to suit your component's spec.
static const char* tpcreader_spec[] =
{
    "implementation_id", "TPCreader",
    "type_name",         "TPCreader",
    "description",       "TPCreader component",
    "version",           "1.0",
    "vendor",            "Kazuo Nakayoshi, KEK",
    "category",          "example",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "2",
    "language",          "C++",
    "lang_type",         "compile",
    ""
};

TPCreader::TPCreader(RTC::Manager* manager)
    : DAQMW::DaqComponentBase(manager),
      m_OutPort("tpcreader_out", m_out_data),
      m_recv_byte_size(0),
      m_recv_timeout_counter(0),
      m_out_status(BUF_SUCCESS),
      m_out_timeout_counter(0)
{
    // Registration: InPort/OutPort/Service

    // Set OutPort buffers
    registerOutPort("tpcreader_out", m_OutPort);

    init_command_port();
    init_state_table();
    set_comp_name("TPCREADER");
}

TPCreader::~TPCreader()
{
}

RTC::ReturnCode_t TPCreader::onInitialize()
{
    if (m_debug) {
        std::cerr << "TPCreader::onInitialize()" << std::endl;
    }

    return RTC::RTC_OK;
}

RTC::ReturnCode_t TPCreader::onExecute(RTC::UniqueId ec_id)
{
    daq_do();

    return RTC::RTC_OK;
}

int TPCreader::daq_dummy()
{
    return 0;
}

int TPCreader::daq_configure()
{
    std::cerr << "*** TPCreader::configure" << std::endl;
#include "daq_configure.inc"
}

int TPCreader::parse_params(::NVList* list)
{
#include "parse_params.inc"
}

int TPCreader::daq_unconfigure()
{
    std::cerr << "*** TPCreader::unconfigure" << std::endl;

    delete [] m_dataA; std::cout << "Delete data bufferA" << std::endl;
    delete [] m_dataB; std::cout << "Delete data bufferB" << std::endl;
    fadc_close(); std::cout << "Close Spacewire ports" << std::endl;
    if (GetETag==1) trigio_fin();
    if (GetETag==2) swio_close();

    return 0;
}

int TPCreader::daq_start()
{
    std::cerr << "*** TPCreader::start" << std::endl;
#include "daq_start.inc"
}

int TPCreader::daq_stop()
{
    std::cerr << "*** TPCreader::stop" << std::endl;
#include "daq_stop.inc"
}

int TPCreader::daq_pause()
{
    std::cerr << "*** TPCreader::pause" << std::endl;
#include "daq_pause.inc"
}

int TPCreader::daq_resume()
{
    std::cerr << "*** TPCreader::resume" << std::endl;
#include "daq_resume.inc"
}

int TPCreader::read_data_from_detectors()
{
#include "read_data_from_detectors.inc"
}

int TPCreader::set_data(int data_byte_size)
{
#include "set_data.inc"
}

int TPCreader::write_OutPort()
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

int TPCreader::daq_run()
{
#include "daq_run.inc"
}

extern "C"
{
    void TPCreaderInit(RTC::Manager* manager)
    {
        RTC::Properties profile(tpcreader_spec);
        manager->registerFactory(profile,
                    RTC::Create<TPCreader>,
                    RTC::Delete<TPCreader>);
    }
};

void TPCreader::switch_buffer(){
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
