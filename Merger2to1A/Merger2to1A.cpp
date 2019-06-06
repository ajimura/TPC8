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
#include "Merger2to1A.h"
#include "daqmwlib.h"

using DAQMW::FatalType::INPORT_ERROR;
using DAQMW::FatalType::OUTPORT_ERROR;
using DAQMW::FatalType::USER_DEFINED_ERROR1;

// Module specification
// Change following items to suit your component's spec.
static const char* merger2to1a_spec[] =
{
    "implementation_id", "Merger2to1A",
    "type_name",         "Merger2to1A",
    "description",       "Merger2to1A component",
    "version",           "1.0",
    "vendor",            "Kazuo Nakayoshi, KEK",
    "category",          "example",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "10",
    "language",          "C++",
    "lang_type",         "compile",
    ""
};

Merger2to1A::Merger2to1A(RTC::Manager* manager)
    : DAQMW::DaqComponentBase(manager),
      m_InPort1("merger2to1a_in1", m_in1_data),
      m_InPort2("merger2to1a_in2", m_in2_data),
      m_OutPort("merger2to1a_out", m_out_data),

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
    registerInPort("merger2to1a_in1", m_InPort1);
    registerInPort("merger2to1a_in2", m_InPort2);
    registerOutPort("merger2to1a_out", m_OutPort);

    init_command_port();
    init_state_table();
    set_comp_name("MERGER2TO1A");
}

Merger2to1A::~Merger2to1A()
{
}


RTC::ReturnCode_t Merger2to1A::onInitialize()
{
    if (m_debug) {
        std::cerr << "Merger2to1A::onInitialize()" << std::endl;
    }

    return RTC::RTC_OK;
}

RTC::ReturnCode_t Merger2to1A::onExecute(RTC::UniqueId ec_id)
{
  //    std::cerr << "*** onExecute\n";
    daq_do();

    return RTC::RTC_OK;
}

int Merger2to1A::daq_dummy()
{
    return 0;
}

int Merger2to1A::daq_configure()
{
    std::cerr << "*** Merger2to1A::configure" << std::endl;

    ::NVList* paramList;
    paramList = m_daq_service0.getCompParams();
    parse_params(paramList);

    In1_TotSiz=0;
    In1_RemainSiz=0;
    In1_CurPos=NULL;
    In1_Done=0;

    In2_TotSiz=0;
    In2_RemainSiz=0;
    In2_CurPos=NULL;
    In2_Done=0;

    Stock_CurNum=0;
    Stock_TotSiz=0;
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

    return 0;
}

int Merger2to1A::parse_params(::NVList* list)
{
    std::cerr << "param list length:" << (*list).length() << std::endl;

    //set default value
    ComponentID=0;
    ReadTimeout=10000;
    Stock_MaxNum=1;

    int len = (*list).length();
    for (int i = 0; i < len; i+=2) {
        std::string sname  = (std::string)(*list)[i].value;
        std::string svalue = (std::string)(*list)[i+1].value;

	//        std::cerr << "sname: " << sname << "  ";
	//        std::cerr << "value: " << svalue << std::endl;

      if (sname == "ComponentID") ComponentID=atoi(svalue.c_str());
      if (sname == "StockNum") Stock_MaxNum=atoi(svalue.c_str());
    }

    return 0;
}


int Merger2to1A::daq_unconfigure()
{
    std::cerr << "*** Merger2to1A::unconfigure" << std::endl;

    delete [] m_data1; std::cout << "Delete data buffer" << std::endl;

    return 0;
}

int Merger2to1A::daq_start()
{
    std::cerr << "*** Merger2to1A::start" << std::endl;

    m_in1_status  = BUF_SUCCESS;
    m_in2_status  = BUF_SUCCESS;
    m_out_status = BUF_SUCCESS;

    return 0;
}

int Merger2to1A::daq_stop()
{
    std::cerr << "*** Merger2to1A::stop" << std::endl;
    //    reset_InPort1();
    //    reset_InPort2();
    return 0;
}

int Merger2to1A::daq_pause()
{
    std::cerr << "*** Merger2to1A::pause" << std::endl;
    return 0;
}

int Merger2to1A::daq_resume()
{
    std::cerr << "*** Merger2to1A::resume" << std::endl;
    return 0;
}

int Merger2to1A::reset_InPort1()
{
//     uncomment if InPort is connected other OutPort of Component
//
     int ret = BUF_SUCCESS;
     while (ret == BUF_SUCCESS) {
         ret = m_InPort1.read();
     }

    std::cerr << "*** Merger2to1A::InPort1 flushed\n";
    return 0;
}

int Merger2to1A::reset_InPort2()
{
//     uncomment if InPort is connected other OutPort of Component
//
     int ret = BUF_SUCCESS;
     while (ret == BUF_SUCCESS) {
         ret = m_InPort2.read();
     }

    std::cerr << "*** Merger2to1A::InPort2 flushed\n";
    return 0;
}

int Merger2to1A::write_OutPort()
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
	  //            if (check_trans_lock()) {     // Check if stop command has come.
	  //                set_trans_unlock();       // Transit to CONFIGURE state.
	  //            }
            m_out_timeout_counter++;
            return -1;
        }
    }
    else { // success
      m_out_timeout_counter = 0;
      m_out_status = BUF_SUCCESS; // successfully done
    }
    return 0; // successfully done
}

unsigned int Merger2to1A::read_InPort1()
{
#include "read_InPort1.inc"
}

unsigned int Merger2to1A::read_InPort2()
{
#include "read_InPort2.inc"
}

unsigned int Merger2to1A::Stock_data(unsigned int data1_byte_size, unsigned int data2_byte_size)
{
#include "Stock_data.inc"
}

int Merger2to1A::set_data(unsigned int data_byte_size)
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

int Merger2to1A::daq_run()
{
#include "daq_run.inc"
}

unsigned char * Merger2to1A::renew_buf(unsigned char * orig_buf,
				      unsigned int cursize, unsigned int newsize)
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
    void Merger2to1AInit(RTC::Manager* manager)
    {
        RTC::Properties profile(merger2to1a_spec);
        manager->registerFactory(profile,
                    RTC::Create<Merger2to1A>,
                    RTC::Delete<Merger2to1A>);
    }
};
