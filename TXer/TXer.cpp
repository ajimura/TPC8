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
      m_inport_recv_data_size(0),
{
    // Registration: InPort/OutPort/Service

    // Set InPort buffers
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

    ::NVList* paramList;
    paramList = m_daq_service0.getCompParams();
    parse_params(paramList);

    In_TotSiz=0;
    In_RemainSiz=0;
    In_CurPos=NULL;
    In_Done=0;

    Stock_CurNum=0;
    Stock_Offset=0;
    //    Cur_MaxDataSiz=67108864; // 64M (tempolary)
    Cur_MaxDataSiz=10240; // 10k (tempolary)

    try{
      m_data1=new unsigned char[Cur_MaxDataSiz];
      m_data4=(unsigned int *)m_data1;
    }
    catch(std::bad_alloc){
      std::cerr << "Bad allocation..." << std::endl;
      fatal_error_report(USER_DEFINED_ERROR1);
    }
    catch(...){
      std::cerr << "Got exception..." << std::endl;
      fatal_error_report(USER_DEFINED_ERROR1);
    }

    // show obtained parameter
    std::cout << "Stock Max Num: " << Stock_MaxNum << std::endl;
    std::cout << "Stock Max Size: " << Stock_MaxSiz << std::endl;
    //
    std::cout << "--- Now ready to start !!" << std::endl;

    return 0;
}

int TXer::parse_params(::NVList* list)
{
    std::cerr << "param list length:" << (*list).length() << std::endl;

    //set default value
    m_debug=false;
    ReadTimeout=10000;
    Stock_MaxNum=1;
    Stock_MaxSiz=2097044;

    int len = (*list).length();
    for (int i = 0; i < len; i+=2) {
        std::string sname  = (std::string)(*list)[i].value;
        std::string svalue = (std::string)(*list)[i+1].value;

	//        std::cerr << "sname: " << sname << "  ";
	//        std::cerr << "value: " << svalue << std::endl;

      if (sname == "DEBUG"){
      	if (svalue == "yes") m_debug=true;
      }

      if (sname == "StockNum") Stock_MaxNum=atoi(svalue.c_str());
      if (sname == "StockMaxSize") Stock_MaxSiz=atoi(svalue.c_str());
    }

    return 0;
}


int TXer::daq_unconfigure()
{
    std::cerr << "*** TXer::unconfigure" << std::endl;

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
