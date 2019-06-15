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
#include "TPCreaderC.h"
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
static const char* tpcreaderc_spec[] =
{
    "implementation_id", "TPCreaderC",
    "type_name",         "TPCreaderC",
    "description",       "TPCreaderC component",
    "version",           "1.0",
    "vendor",            "Kazuo Nakayoshi, KEK",
    "category",          "example",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "2",
    "language",          "C++",
    "lang_type",         "compile",
    ""
};

TPCreaderC::TPCreaderC(RTC::Manager* manager)
    : DAQMW::DaqComponentBase(manager),
      m_OutPort("tpcreaderc_out", m_out_data),
      m_recv_byte_size(0),
      m_recv_timeout_counter(0),
      m_out_status(BUF_SUCCESS),

      m_out_timeout_counter(0),
      m_debug(false)
{
    // Registration: InPort/OutPort/Service

    // Set OutPort buffers
    registerOutPort("tpcreaderc_out", m_OutPort);

    init_command_port();
    init_state_table();
    set_comp_name("TPCREADERC");
}

TPCreaderC::~TPCreaderC()
{
}

RTC::ReturnCode_t TPCreaderC::onInitialize()
{
    if (m_debug) {
        std::cerr << "TPCreaderC::onInitialize()" << std::endl;
    }

    return RTC::RTC_OK;
}

RTC::ReturnCode_t TPCreaderC::onExecute(RTC::UniqueId ec_id)
{
    daq_do();

    return RTC::RTC_OK;
}

int TPCreaderC::daq_dummy()
{
    return 0;
}

int TPCreaderC::daq_configure()
{
    std::cerr << "*** TPCreaderC::configure" << std::endl;
#include "daq_configure.inc"
}

int TPCreaderC::parse_params(::NVList* list)
{
#include "parse_params.inc"
}

int TPCreaderC::daq_unconfigure()
{
    std::cerr << "*** TPCreaderC::unconfigure" << std::endl;

    delete [] m_dataA; std::cout << "Delete data bufferA" << std::endl;
    delete [] m_dataB; std::cout << "Delete data bufferB" << std::endl;
    fadc_close(); std::cout << "Close Spacewire ports" << std::endl;
    trigio_fin();

    return 0;
}

int TPCreaderC::daq_start()
{
    std::cerr << "*** TPCreaderC::start" << std::endl;
#include "daq_start.inc"
}

int TPCreaderC::daq_stop()
{
    std::cerr << "*** TPCreaderC::stop" << std::endl;
#include "daq_stop.inc"
}

int TPCreaderC::daq_pause()
{
    std::cerr << "*** TPCreaderC::pause" << std::endl;
#include "daq_pause.inc"
}

int TPCreaderC::daq_resume()
{
    std::cerr << "*** TPCreaderC::resume" << std::endl;
#include "daq_resume.inc"
}

int TPCreaderC::read_data_from_detectors()
{
#include "read_data_from_detectors.inc"
}

int TPCreaderC::set_data(int data_byte_size)
{
    unsigned char header[8];
    unsigned char footer[8];

    set_header(&header[0], (unsigned int)data_byte_size);
    set_footer(&footer[0]);

    ///set OutPort buffer length
    m_out_data.data.length((unsigned int)data_byte_size + HEADER_BYTE_SIZE + FOOTER_BYTE_SIZE);
    memcpy(&(m_out_data.data[0]), &header[0], HEADER_BYTE_SIZE);
    memcpy(&(m_out_data.data[HEADER_BYTE_SIZE]), &m_data1[0], (size_t)data_byte_size);
    memcpy(&(m_out_data.data[HEADER_BYTE_SIZE + (unsigned int)data_byte_size]), &footer[0],
           FOOTER_BYTE_SIZE);

    return 0;
}

int TPCreaderC::write_OutPort()
{
  if (m_debug) {
    std::cerr << "write: StockNum=" << Stock_CurNum << " SockSize=" << Stock_Offset << std::endl;
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
            return -1;
        }
        if (m_out_status == BUF_NOBUF) { // No Buffer on Downstream
            return -1;
        }
    }
    else {
        m_out_status = BUF_SUCCESS; // successfully done
    }

    return 0;
}

int TPCreaderC::daq_run()
{
#include "daq_run.inc"
}

extern "C"
{
    void TPCreaderCInit(RTC::Manager* manager)
    {
        RTC::Properties profile(tpcreaderc_spec);
        manager->registerFactory(profile,
                    RTC::Create<TPCreaderC>,
                    RTC::Delete<TPCreaderC>);
    }
};

void TPCreaderC::toLower(std::basic_string<char>& s)
{
    for (std::basic_string<char>::iterator p = s.begin(); p != s.end(); ++p) {
        *p = tolower(*p);
    }
}

