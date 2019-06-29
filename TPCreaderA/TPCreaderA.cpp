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
#include "TPCreaderA.h"
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
      m_out_status(BUF_SUCCESS),

      m_debug(false)
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

    delete [] m_data4; std::cout << "Delete data buffer" << std::endl;
    fadc_close(); std::cout << "Close Spacewire ports" << std::endl;
    trigio_fin();

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

int TPCreaderA::set_data(unsigned int data_byte_size)
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

int TPCreaderA::write_OutPort()
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

int TPCreaderA::daq_run()
{
  int seq_temp;
    if (m_debug) {
        std::cerr << "*** TPCreaderA::run" << std::endl;
    }

    if (check_trans_lock()) {  // check if stop command has come
        set_trans_unlock();    // transit to CONFIGURED state
        return 0;
    }

    if (m_out_status == BUF_SUCCESS) {   // previous OutPort.write() successfully done
        m_recv_byte_size = read_data_from_detectors();
        if (m_recv_byte_size > 0) {
            set_data(m_recv_byte_size); // set data to OutPort Buffer
        }
    }

    if (write_OutPort() < 0) {
        ;     // Timeout. do nothing.
    }
    else {    // OutPort write successfully done
      seq_temp=(int)get_sequence_num();
      std::cerr << "seq_num: " << seq_temp << " incremented " << m_out_status << std::endl;
        inc_sequence_num();                     // increase sequence num.
        inc_total_data_size(m_recv_byte_size);  // increase total data byte size
    }

    return 0;
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

void TPCreaderA::toLower(std::basic_string<char>& s)
{
    for (std::basic_string<char>::iterator p = s.begin(); p != s.end(); ++p) {
        *p = tolower(*p);
    }
}

