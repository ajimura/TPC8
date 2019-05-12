// -*- C++ -*-
/*!
 * @file
 * @brief
 * @date
 * @author
 *
 */

#include "Merger2to1B.h"
#include "daqmwlib.h"

using DAQMW::FatalType::INPORT_ERROR;
using DAQMW::FatalType::OUTPORT_ERROR;
using DAQMW::FatalType::USER_DEFINED_ERROR1;

// Module specification
// Change following items to suit your component's spec.
static const char* merger2to1b_spec[] =
{
    "implementation_id", "Merger2to1B",
    "type_name",         "Merger2to1B",
    "description",       "Merger2to1B component",
    "version",           "1.0",
    "vendor",            "Kazuo Nakayoshi, KEK",
    "category",          "example",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "10",
    "language",          "C++",
    "lang_type",         "compile",
    ""
};

Merger2to1B::Merger2to1B(RTC::Manager* manager)
    : DAQMW::DaqComponentBase(manager),
      m_InPort1("merger2to1b_in1", m_in1_data),
      m_InPort2("merger2to1b_in2", m_in2_data),
      m_OutPort("merger2to1b_out", m_out_data),

      m_in1_status(BUF_SUCCESS),
      m_in2_status(BUF_SUCCESS),
      m_out_status(BUF_SUCCESS),
      m_in1_timeout_counter(0),
      m_in2_timeout_counter(0),
      m_out_timeout_counter(0),
      m_inport1_recv_data_size(0),
      m_inport2_recv_data_size(0),
      m_debug(false)
      //      m_debug(true)
{
    // Registration: InPort/OutPort/Service

    // Set InPort buffers
    registerInPort("merger2to1b_in1", m_InPort1);
    registerInPort("merger2to1b_in2", m_InPort2);
    registerOutPort("merger2to1b_out", m_OutPort);

    init_command_port();
    init_state_table();
    set_comp_name("MERGER2TO1B");
}

Merger2to1B::~Merger2to1B()
{
}


RTC::ReturnCode_t Merger2to1B::onInitialize()
{
    if (m_debug) {
        std::cerr << "Merger2to1B::onInitialize()" << std::endl;
    }

    return RTC::RTC_OK;
}

RTC::ReturnCode_t Merger2to1B::onExecute(RTC::UniqueId ec_id)
{
  //    std::cerr << "*** onExecute\n";
    daq_do();

    return RTC::RTC_OK;
}

int Merger2to1B::daq_dummy()
{
    return 0;
}

int Merger2to1B::daq_configure()
{
    std::cerr << "*** Merger2to1B::configure" << std::endl;

    ::NVList* paramList;
    paramList = m_daq_service0.getCompParams();
    parse_params(paramList);

    return 0;
}

int Merger2to1B::parse_params(::NVList* list)
{
    std::cerr << "param list length:" << (*list).length() << std::endl;

    int len = (*list).length();
    for (int i = 0; i < len; i+=2) {
        std::string sname  = (std::string)(*list)[i].value;
        std::string svalue = (std::string)(*list)[i+1].value;

	//        std::cerr << "sname: " << sname << "  ";
	//        std::cerr << "value: " << svalue << std::endl;

      if (sname == "ComponentID") ComponentID=atoi(svalue.c_str());
    }

    return 0;
}


int Merger2to1B::daq_unconfigure()
{
    std::cerr << "*** Merger2to1B::unconfigure" << std::endl;

    return 0;
}

int Merger2to1B::daq_start()
{
    std::cerr << "*** Merger2to1B::start" << std::endl;

    m_in1_status  = BUF_SUCCESS;
    m_in2_status  = BUF_SUCCESS;
    m_out_status = BUF_SUCCESS;

    return 0;
}

int Merger2to1B::daq_stop()
{
    std::cerr << "*** Merger2to1B::stop" << std::endl;
    //    reset_InPort1();
    //    reset_InPort2();
    return 0;
}

int Merger2to1B::daq_pause()
{
    std::cerr << "*** Merger2to1B::pause" << std::endl;
    return 0;
}

int Merger2to1B::daq_resume()
{
    std::cerr << "*** Merger2to1B::resume" << std::endl;
    return 0;
}

//int Merger2to1B::set_data_OutPort(unsigned int data_byte_size)
//{
//  m_out_data.data.length(data_byte_size);
//  memcpy(&(m_out_data.data[0]),&m_in1_data.data[0],dtaa_byte_size);
//  retuen 0;
//}

int Merger2to1B::reset_InPort1()
{
//     uncomment if InPort is connected other OutPort of Component
//
     int ret = BUF_SUCCESS;
     while (ret == BUF_SUCCESS) {
         ret = m_InPort1.read();
     }

    std::cerr << "*** Merger2to1B::InPort1 flushed\n";
    return 0;
}

int Merger2to1B::reset_InPort2()
{
//     uncomment if InPort is connected other OutPort of Component
//
     int ret = BUF_SUCCESS;
     while (ret == BUF_SUCCESS) {
         ret = m_InPort2.read();
     }

    std::cerr << "*** Merger2to1B::InPort2 flushed\n";
    return 0;
}

int Merger2to1B::set_data_OutPort(unsigned int data1_byte_size, unsigned int data2_byte_size)
{
#include "set_data_OutPort.inc"
}

int Merger2to1B::write_OutPort()
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
            if (check_trans_lock()) {     // Check if stop command has come.
                set_trans_unlock();       // Transit to CONFIGURE state.
            }
            m_out_timeout_counter++;
            return -1;
        }
    }
    else { // success
        m_out_timeout_counter = 0;
    }
    return 0; // successfully done
}

unsigned int Merger2to1B::read_InPort1()
{
    /////////////// read data from InPort Buffer ///////////////
    unsigned int recv_byte_size = 0;
    bool ret = m_InPort1.read();

    //////////////////// check read status /////////////////////
    if (ret == false) { // false: TIMEOUT or FATAL
        m_in1_status = check_inPort_status(m_InPort1);
        if (m_in1_status == BUF_TIMEOUT) { // Buffer empty.
            m_in1_timeout_counter++;
            if (check_trans_lock()) {     // Check if stop command has come.
                set_trans_unlock();       // Transit to CONFIGURE state.
            }
        }
        else if (m_in1_status == BUF_FATAL) { // Fatal error
            fatal_error_report(INPORT_ERROR);
        }
    }
    else { // success
        m_in1_timeout_counter = 0;
        recv_byte_size = m_in1_data.data.length();
        m_in1_status = BUF_SUCCESS;
    }
    if (m_debug) {
        std::cerr << "m_in1_data.data.length():" << recv_byte_size
                  << std::endl;
    }

    return recv_byte_size;
}

unsigned int Merger2to1B::read_InPort2()
{
    /////////////// read data from InPort Buffer ///////////////
    unsigned int recv_byte_size = 0;
    bool ret = m_InPort2.read();

    //////////////////// check read status /////////////////////
    if (ret == false) { // false: TIMEOUT or FATAL
        m_in2_status = check_inPort_status(m_InPort2);
        if (m_in2_status == BUF_TIMEOUT) { // Buffer empty.
            m_in2_timeout_counter++;
            if (check_trans_lock()) {     // Check if stop command has come.
                set_trans_unlock();       // Transit to CONFIGURE state.
            }
        }
        else if (m_in2_status == BUF_FATAL) { // Fatal error
            fatal_error_report(INPORT_ERROR);
        }
    }
    else { // success
        m_in2_timeout_counter = 0;
        recv_byte_size = m_in2_data.data.length();
        m_in2_status = BUF_SUCCESS;
    }
    if (m_debug) {
        std::cerr << "m_in2_data.data.length():" << recv_byte_size
                  << std::endl;
    }

    return recv_byte_size;
}

int Merger2to1B::daq_run()
{
  unsigned int event_data_size;
    if (m_debug) {
        std::cerr << "*** Merger2to1B::run" << std::endl;
    }

    if (m_out_status != BUF_TIMEOUT) {
      if (m_inport1_recv_data_size==0)
        m_inport1_recv_data_size = read_InPort1();
      if (m_inport2_recv_data_size==0)
        m_inport2_recv_data_size = read_InPort2();

      if ((m_inport1_recv_data_size == 0) || (m_inport2_recv_data_size == 0)){ // TIMEOUT
	return 0;
      } else {
	  //            check_header_footer(m_in1_data, m_inport1_recv_data_size);
	  //            check_header_footer(m_in2_data, m_inport2_recv_data_size);
	set_data_OutPort(m_inport1_recv_data_size,m_inport2_recv_data_size);
      }
    }

    if ((m_in1_status != BUF_TIMEOUT) && (m_in2_status != BUF_TIMEOUT) && (m_out_status != BUF_TIMEOUT)) {
        if (write_OutPort() < 0) { // TIMEOUT
            ; // do nothing
        }
        else {
            m_out_status = BUF_SUCCESS;
        }
    }

    if ((m_in1_status   != BUF_TIMEOUT) &&
	(m_in2_status   != BUF_TIMEOUT) &&
        (m_out_status != BUF_TIMEOUT)) {
        inc_sequence_num();                    // increase sequence num.
	//        unsigned int event_data_size = get_event_size(m_inport_recv_data_size);
	event_data_size=m_inport1_recv_data_size+m_inport2_recv_data_size+80;
        inc_total_data_size(event_data_size);  // increase total data byte size
	m_inport1_recv_data_size=0;
	m_inport2_recv_data_size=0;
    }

    //    sleep(1);
    //
    //    if (check_trans_lock()) {  /// got stop command
    //        set_trans_unlock();
    //        return 0;
    //    }

   return 0;
}

extern "C"
{
    void Merger2to1BInit(RTC::Manager* manager)
    {
        RTC::Properties profile(merger2to1b_spec);
        manager->registerFactory(profile,
                    RTC::Create<Merger2to1B>,
                    RTC::Delete<Merger2to1B>);
    }
};
