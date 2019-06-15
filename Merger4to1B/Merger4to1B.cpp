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
#include "Merger4to1B.h"
#include "daqmwlib.h"

using DAQMW::FatalType::INPORT_ERROR;
using DAQMW::FatalType::OUTPORT_ERROR;
using DAQMW::FatalType::USER_DEFINED_ERROR1;

// Module specification
// Change following items to suit your component's spec.
static const char* merger4to1b_spec[] =
{
    "implementation_id", "Merger4to1B",
    "type_name",         "Merger4to1B",
    "description",       "Merger4to1B component",
    "version",           "1.0",
    "vendor",            "Kazuo Nakayoshi, KEK",
    "category",          "example",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "10",
    "language",          "C++",
    "lang_type",         "compile",
    ""
};

Merger4to1B::Merger4to1B(RTC::Manager* manager)
    : DAQMW::DaqComponentBase(manager),
      m_InPort1("merger4to1b_in1", m_in1_data),
      m_InPort2("merger4to1b_in2", m_in2_data),
      m_InPort3("merger4to1b_in3", m_in3_data),
      m_InPort4("merger4to1b_in4", m_in4_data),
      m_OutPort("merger4to1b_out", m_out_data),

      m_in1_status(BUF_SUCCESS),
      m_in2_status(BUF_SUCCESS),
      m_in3_status(BUF_SUCCESS),
      m_in4_status(BUF_SUCCESS),
      m_out_status(BUF_SUCCESS),
      m_in1_timeout_counter(0),
      m_in2_timeout_counter(0),
      m_in3_timeout_counter(0),
      m_in4_timeout_counter(0),
      m_out_timeout_counter(0),
      m_inport1_recv_data_size(0),
      m_inport2_recv_data_size(0),
      m_inport3_recv_data_size(0),
      m_inport4_recv_data_size(0),
      m_debug(false)
      //      m_debug(true)
{
    // Registration: InPort/OutPort/Service

    // Set InPort buffers
    registerInPort("merger4to1b_in1", m_InPort1);
    registerInPort("merger4to1b_in2", m_InPort2);
    registerInPort("merger4to1b_in3", m_InPort3);
    registerInPort("merger4to1b_in4", m_InPort4);
    registerOutPort("merger4to1b_out", m_OutPort);

    init_command_port();
    init_state_table();
    set_comp_name("MERGER4TO1B");
}

Merger4to1B::~Merger4to1B()
{
}


RTC::ReturnCode_t Merger4to1B::onInitialize()
{
    if (m_debug) {
        std::cerr << "Merger4to1B::onInitialize()" << std::endl;
    }

    return RTC::RTC_OK;
}

RTC::ReturnCode_t Merger4to1B::onExecute(RTC::UniqueId ec_id)
{
  //    std::cerr << "*** onExecute\n";
    daq_do();

    return RTC::RTC_OK;
}

int Merger4to1B::daq_dummy()
{
    return 0;
}

int Merger4to1B::daq_configure()
{
    std::cerr << "*** Merger4to1B::configure" << std::endl;

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

    In3_TotSiz=0;
    In3_RemainSiz=0;
    In3_CurPos=NULL;
    In3_Done=0;

    In4_TotSiz=0;
    In4_RemainSiz=0;
    In4_CurPos=NULL;
    In4_Done=0;

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

int Merger4to1B::parse_params(::NVList* list)
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


int Merger4to1B::daq_unconfigure()
{
    std::cerr << "*** Merger4to1B::unconfigure" << std::endl;

    delete [] m_data1; std::cout << "Delete data buffer" << std::endl;

    return 0;
}

int Merger4to1B::daq_start()
{
    std::cerr << "*** Merger4to1B::start" << std::endl;

    m_in1_status  = BUF_SUCCESS;
    m_in2_status  = BUF_SUCCESS;
    m_in3_status  = BUF_SUCCESS;
    m_in4_status  = BUF_SUCCESS;
    m_out_status = BUF_SUCCESS;

    return 0;
}

int Merger4to1B::daq_stop()
{
    std::cerr << "*** Merger4to1B::stop" << std::endl;
    //    reset_InPort1();
    //    reset_InPort2();
    //    reset_InPort3();
    //    reset_InPort4();
    return 0;
}

int Merger4to1B::daq_pause()
{
    std::cerr << "*** Merger4to1B::pause" << std::endl;
    return 0;
}

int Merger4to1B::daq_resume()
{
    std::cerr << "*** Merger4to1B::resume" << std::endl;
    return 0;
}

int Merger4to1B::reset_InPort1()
{
//     uncomment if InPort is connected other OutPort of Component
//
     int ret = BUF_SUCCESS;
     while (ret == BUF_SUCCESS) {
         ret = m_InPort1.read();
     }

    std::cerr << "*** Merger4to1B::InPort1 flushed\n";
    return 0;
}

int Merger4to1B::reset_InPort2()
{
//     uncomment if InPort is connected other OutPort of Component
//
     int ret = BUF_SUCCESS;
     while (ret == BUF_SUCCESS) {
         ret = m_InPort2.read();
     }

    std::cerr << "*** Merger4to1B::InPort2 flushed\n";
    return 0;
}

int Merger4to1B::reset_InPort3()
{
//     uncomment if InPort is connected other OutPort of Component
//
     int ret = BUF_SUCCESS;
     while (ret == BUF_SUCCESS) {
         ret = m_InPort3.read();
     }

    std::cerr << "*** Merger4to1B::InPort3 flushed\n";
    return 0;
}

int Merger4to1B::reset_InPort4()
{
//     uncomment if InPort is connected other OutPort of Component
//
     int ret = BUF_SUCCESS;
     while (ret == BUF_SUCCESS) {
         ret = m_InPort4.read();
     }

    std::cerr << "*** Merger4to1B::InPort4 flushed\n";
    return 0;
}

int Merger4to1B::write_OutPort()
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

int Merger4to1B::read_InPort1()
{
#include "read_InPort1.inc"
}

int Merger4to1B::read_InPort2()
{
#include "read_InPort2.inc"
}

int Merger4to1B::read_InPort3()
{
#include "read_InPort3.inc"
}

int Merger4to1B::read_InPort4()
{
#include "read_InPort4.inc"
}

void Merger4to1B::Stock_data(int data1_byte_size, int data2_byte_size,
				    int data3_byte_size, int data4_byte_size)
{
#include "Stock_data.inc"
}

int Merger4to1B::set_data(int data_byte_size)
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

int Merger4to1B::daq_run()
{
#include "daq_run.inc"
}

unsigned char * Merger4to1B::renew_buf(unsigned char * orig_buf,
				      unsigned int cursize, unsigned int newsize)
{
  unsigned char * new_buf;

  try{
    new_buf = new unsigned char[newsize];
  }
  catch(std::bad_alloc){
    std::cerr << "Bad allocation..." << std::endl;
    return NULL;
  }
  catch(...){
    std::cerr << "Got exception..." << std::endl;
    return NULL;
  }

  memcpy(new_buf, orig_buf, cursize);
  delete [] orig_buf;

  std::cerr << "Re-new data buf: " << cursize << " -> " << newsize << std::endl;

  return new_buf;
}

extern "C"
{
    void Merger4to1BInit(RTC::Manager* manager)
    {
        RTC::Properties profile(merger4to1b_spec);
        manager->registerFactory(profile,
                    RTC::Create<Merger4to1B>,
                    RTC::Delete<Merger4to1B>);
    }
};
