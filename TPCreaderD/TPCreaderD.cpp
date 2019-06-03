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
#include "TPCreaderD.h"
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
static const char* tpcreaderd_spec[] =
{
    "implementation_id", "TPCreaderD",
    "type_name",         "TPCreaderD",
    "description",       "TPCreaderD component",
    "version",           "1.0",
    "vendor",            "Kazuo Nakayoshi, KEK",
    "category",          "example",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "2",
    "language",          "C++",
    "lang_type",         "compile",
    ""
};

TPCreaderD::TPCreaderD(RTC::Manager* manager)
    : DAQMW::DaqComponentBase(manager),
      m_OutPort("tpcreaderd_out", m_out_data),
      m_recv_byte_size(0),
      m_recv_timeout_counter(0),
      m_out_status(BUF_SUCCESS),

      m_out_timeout_counter(0),
      m_debug(false)
{
    // Registration: InPort/OutPort/Service

    // Set OutPort buffers
    registerOutPort("tpcreaderd_out", m_OutPort);

    init_command_port();
    init_state_table();
    set_comp_name("TPCREADERD");
}

TPCreaderD::~TPCreaderD()
{
}

RTC::ReturnCode_t TPCreaderD::onInitialize()
{
    if (m_debug) {
        std::cerr << "TPCreaderD::onInitialize()" << std::endl;
    }

    return RTC::RTC_OK;
}

RTC::ReturnCode_t TPCreaderD::onExecute(RTC::UniqueId ec_id)
{
    daq_do();

    return RTC::RTC_OK;
}

int TPCreaderD::daq_dummy()
{
    return 0;
}

int TPCreaderD::daq_configure()
{
    std::cerr << "*** TPCreaderD::configure" << std::endl;
#include "daq_configure.inc"
}

int TPCreaderD::parse_params(::NVList* list)
{
#include "parse_params.inc"
}

int TPCreaderD::daq_unconfigure()
{
    std::cerr << "*** TPCreaderD::unconfigure" << std::endl;

    delete [] m_data4; std::cout << "Delete data buffer" << std::endl;
    fadc_close(); std::cout << "Close Spacewire ports" << std::endl;
    trigio_fin();

    return 0;
}

int TPCreaderD::daq_start()
{
    std::cerr << "*** TPCreaderD::start" << std::endl;
#include "daq_start.inc"
}

int TPCreaderD::daq_stop()
{
    std::cerr << "*** TPCreaderD::stop" << std::endl;
#include "daq_stop.inc"
}

int TPCreaderD::daq_pause()
{
    std::cerr << "*** TPCreaderD::pause" << std::endl;
#include "daq_pause.inc"
}

int TPCreaderD::daq_resume()
{
    std::cerr << "*** TPCreaderD::resume" << std::endl;
#include "daq_resume.inc"
}

int TPCreaderD::read_data_from_detectors()
{
#include "read_data_from_detectors.inc"
}

int TPCreaderD::set_data(unsigned int data_byte_size)
{
    unsigned char header[8];
    unsigned char footer[8];

    set_header(&header[0], data_byte_size);
    set_footer(&footer[0]);

    ///set OutPort buffer length
    m_out_data.data.length(data_byte_size + HEADER_BYTE_SIZE + FOOTER_BYTE_SIZE);
    memcpy(&(m_out_data.data[0]), &header[0], HEADER_BYTE_SIZE);
    memcpy(&(m_out_data.data[HEADER_BYTE_SIZE]), &m_data1[0], data_byte_size);
    memcpy(&(m_out_data.data[HEADER_BYTE_SIZE + data_byte_size]), &footer[0],
           FOOTER_BYTE_SIZE);

    return 0;
}

int TPCreaderD::write_OutPort()
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
            return -1;
        }
    }
    else {
        m_out_status = BUF_SUCCESS; // successfully done
    }

    return 0;
}

int TPCreaderD::daq_run()
{
    if (m_debug) {
        std::cerr << "*** TPCreaderD::run" << std::endl;
    }

    if (check_trans_lock()) {  // check if stop command has come
      if (Stock_CurNum>0){
	if (m_out_status!=BUF_TIMEOUT) set_data(Stock_Offset);
	if (write_OutPort()<0){
	  ;
	}else{
	  inc_total_data_size(Stock_Offset);  // increase total data byte size
	  Stock_CurNum=0;
	  Stock_Offset=0;
	}
      }
      set_trans_unlock();    // transit to CONFIGURED state
      return 0;
    }

    if (m_out_status == BUF_SUCCESS) {   // previous OutPort.write() successfully done
        m_recv_byte_size = read_data_from_detectors();
    }

    if (m_out_status == BUF_TIMEOUT){
      if (write_OutPort()<0){
	;
      }else{
	inc_total_data_size(Stock_Offset);  // increase total data byte size
	Stock_CurNum=0;
	Stock_Offset=0;
      }
    }

    if ( (Stock_CurNum==Stock_MaxNum) || (Stock_CurNum>0 && m_recv_timeout_counter>ReadTimeout) ){
      set_data(Stock_Offset);
      if (write_OutPort()<0){
	;
      }else{
	inc_total_data_size(Stock_Offset);  // increase total data byte size
	Stock_CurNum=0;
	Stock_Offset=0;
      }
    }

    return 0;
}

extern "C"
{
    void TPCreaderDInit(RTC::Manager* manager)
    {
        RTC::Properties profile(tpcreaderd_spec);
        manager->registerFactory(profile,
                    RTC::Create<TPCreaderD>,
                    RTC::Delete<TPCreaderD>);
    }
};

void TPCreaderD::toLower(std::basic_string<char>& s)
{
    for (std::basic_string<char>::iterator p = s.begin(); p != s.end(); ++p) {
        *p = tolower(*p);
    }
}

