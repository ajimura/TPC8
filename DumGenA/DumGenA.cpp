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
#include "DumGenA.h"
#include "daqmwlib.h"

using DAQMW::FatalType::DATAPATH_DISCONNECTED;
using DAQMW::FatalType::OUTPORT_ERROR;
using DAQMW::FatalType::USER_DEFINED_ERROR1; //SpaceWire
using DAQMW::FatalType::USER_DEFINED_ERROR2; //RMAP
using DAQMW::FatalType::USER_DEFINED_ERROR3; //MISC

// Module specification
// Change following items to suit your component's spec.
static const char* dumgena_spec[] =
{
    "implementation_id", "DumGenA",
    "type_name",         "DumGenA",
    "description",       "DumGenA component",
    "version",           "1.0",
    "vendor",            "Kazuo Nakayoshi, KEK",
    "category",          "example",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "2",
    "language",          "C++",
    "lang_type",         "compile",
    ""
};

DumGenA::DumGenA(RTC::Manager* manager)
    : DAQMW::DaqComponentBase(manager),
      m_OutPort("dumgena_out", m_out_data),
      m_recv_byte_size(0),
      m_recv_timeout_counter(0),
      m_out_status(BUF_SUCCESS),

      m_debug(false)
{
    // Registration: InPort/OutPort/Service

    // Set OutPort buffers
    registerOutPort("dumgena_out", m_OutPort);

    init_command_port();
    init_state_table();
    set_comp_name("DUMGENA");
}

DumGenA::~DumGenA()
{
}

RTC::ReturnCode_t DumGenA::onInitialize()
{
    if (m_debug) {
        std::cerr << "DumGenA::onInitialize()" << std::endl;
    }

    return RTC::RTC_OK;
}

RTC::ReturnCode_t DumGenA::onExecute(RTC::UniqueId ec_id)
{
    daq_do();

    return RTC::RTC_OK;
}

int DumGenA::daq_dummy()
{
    return 0;
}

int DumGenA::daq_configure()
{
    std::cerr << "*** DumGenA::configure" << std::endl;

    ::NVList* paramList;
    paramList = m_daq_service0.getCompParams();
    parse_params(paramList);

    // show obtained parameter
    std::cout << std::dec;
    std::cout << "Generate Size: " << generate_size << std::endl;
    std::cout << "Interval Time: " << interval_time << std::endl;
    //
    std::cout << std::hex;
    std::cout << "Component Type: 0x" << 
      std::setfill('0') << std::setw(8) << std::hex << ComponentType << std::endl;
    std::cout << "Component ID: 0x" << 
      std::setfill('0') << std::setw(8) << std::hex << ComponentID << std::endl;
    std::cout << std::dec;
    std::cout << "Stock Max Num: " << Stock_MaxNum << std::endl;

    // Initialization start
    std::cout << "--- Initialization starting..." << std::endl;

    // allocate data buffer
    try{
      m_data4=new unsigned int[(generate_size+1024)*Stock_MaxNum];
      m_data1=(unsigned char *)m_data4;
    }
    catch(std::bad_alloc){
      std::cerr << "Bad allocation: size=" << (generate_size+1024)*Stock_MaxNum << " bytes" << std::endl;
      fatal_error_report(USER_DEFINED_ERROR3);
    }
    catch(...){
      std::cerr << "Got exception" << std::endl;
      fatal_error_report(USER_DEFINED_ERROR3);
    }

    std::cout << "Prepare data buffer done" << std::endl;

    std::cout << "--- Now ready to start !!" << std::endl;

    return 0;
}

int DumGenA::parse_params(::NVList* list)
{
  int i;

    std::cout << "--- param list (length:" << (*list).length() << ")" << std::endl;

    //set default value
    generate_size=0;
    interval_time=1000000;
    ComponentID=0;
    ReadTimeout=10000;
    Stock_MaxNum=1;
    Stock_CurNum=0;
    Stock_TotSiz=0;
    Stock_Offset=0;

    int len = (*list).length();
    for (i = 0; i < len; i+=2) {
      std::string sname  = (std::string)(*list)[i].value;
      std::string svalue = (std::string)(*list)[i+1].value;

      //      std::cerr << "sname: " << sname << "  ";
      //      std::cerr << "value: " << svalue << std::endl;

      if (sname == "GenSize") generate_size=atoi(svalue.c_str());
      if (sname == "IntvTime") interval_time=atoi(svalue.c_str());
      if (sname == "ComponentID") ComponentID=atoi(svalue.c_str());
      if (sname == "StockNum") Stock_MaxNum=atoi(svalue.c_str());
    }

    return 0;
}

int DumGenA::daq_unconfigure()
{
    std::cerr << "*** DumGenA::unconfigure" << std::endl;

    delete [] m_data4; std::cout << "Delete data buffer" << std::endl;

    return 0;
}

int DumGenA::daq_start()
{
    std::cerr << "*** DumGenA::start" << std::endl;
    m_out_status = BUF_SUCCESS;
    eventnum = 0;

    return 0;
}

int DumGenA::daq_stop()
{
    std::cerr << "*** DumGenA::stop" << std::endl;
    return 0;
}

int DumGenA::daq_pause()
{
    std::cerr << "*** DumGenA::pause" << std::endl;
    return 0;
}

int DumGenA::daq_resume()
{
    std::cerr << "*** DumGenA::resume" << std::endl;
    return 0;
}

int DumGenA::read_data_from_detectors()
{
#include "read_data_from_detectors.inc"
}

int DumGenA::set_data(unsigned int data_byte_size)
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

int DumGenA::write_OutPort()
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

int DumGenA::daq_run()
{
    struct timespec ts;
    double t0,t1;

    if (m_debug) {
        std::cerr << "*** DumGenA::run" << std::endl;
    }

    if (Stock_CurNum==0)

      if (check_trans_lock()) {  // check if stop command has come
        set_trans_unlock();    // transit to CONFIGURED state
        return 0;
      }

    if (m_out_status == BUF_SUCCESS) {   // previous OutPort.write() successfully done
        m_recv_byte_size = read_data_from_detectors();
	//        if (m_recv_byte_size > 0) {
	//            set_data(m_recv_byte_size); // set data to OutPort Buffer
	//        }
    }

    if (m_out_status == BUF_TIMEOUT){
      clock_gettime(CLOCK_MONOTONIC,&ts);
      t0=(ts.tv_sec*1.)+(ts.tv_nsec/1000000000.);
      //      std::cout << "-w>" << std::fixed << std::setprecision(9) << t0 << std::endl;
      if (write_OutPort()<0){
	;
      }else{
	//	inc_sequence_num();                     // increase sequence num.
	inc_total_data_size(Stock_Offset);  // increase total data byte size
	Stock_CurNum=0;
	Stock_Offset=0;
      }
      clock_gettime(CLOCK_MONOTONIC,&ts);
      t1=(ts.tv_sec*1.)+(ts.tv_nsec/1000000000.);
      std::cout << "+w> " << std::fixed << std::setprecision(9) << t1-t0 << std::endl;
    }

    if ( (Stock_CurNum==Stock_MaxNum) || (Stock_CurNum>0 && m_recv_timeout_counter>ReadTimeout) ){
      clock_gettime(CLOCK_MONOTONIC,&ts);
      t0=(ts.tv_sec*1.)+(ts.tv_nsec/1000000000.);
      //      std::cout << "-w>" << std::fixed << std::setprecision(9) << t0 << std::endl;
      set_data(Stock_Offset);
      if (write_OutPort()<0){
	;
      }else{
	//	inc_sequence_num();                     // increase sequence num.
	inc_total_data_size(Stock_Offset);  // increase total data byte size
	Stock_CurNum=0;
	Stock_Offset=0;
      }
      clock_gettime(CLOCK_MONOTONIC,&ts);
      t1=(ts.tv_sec*1.)+(ts.tv_nsec/1000000000.);
      std::cout << "+w> " << std::fixed << std::setprecision(9) << t1-t0 << std::endl;
    }

    // clock_gettime(CLOCK_MONOTONIC,&ts);
    // t0=(ts.tv_sec*1.)+(ts.tv_nsec/1000000000.);
    // //    std::cout << "Pre:" << std::fixed << std::setprecision(9) << t0 << std::endl;

    // if (write_OutPort() < 0) {
    //     ;     // Timeout. do nothing.
    // }
    // else {    // OutPort write successfully done
    //     inc_sequence_num();                     // increase sequence num.
    //     inc_total_data_size(m_recv_byte_size);  // increase total data byte size
    // }

    // clock_gettime(CLOCK_MONOTONIC,&ts);
    // t1=(ts.tv_sec*1.)+(ts.tv_nsec/1000000000.);
    // std::cout << std::fixed << std::setprecision(9) << t1-t0 << std::endl;

    return 0;
}

extern "C"
{
    void DumGenAInit(RTC::Manager* manager)
    {
        RTC::Properties profile(dumgena_spec);
        manager->registerFactory(profile,
                    RTC::Create<DumGenA>,
                    RTC::Delete<DumGenA>);
    }
};

void DumGenA::toLower(std::basic_string<char>& s)
{
    for (std::basic_string<char>::iterator p = s.begin(); p != s.end(); ++p) {
        *p = tolower(*p);
    }
}

