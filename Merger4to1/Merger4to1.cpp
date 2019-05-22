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
#include "Merger4to1.h"
#include "daqmwlib.h"

using DAQMW::FatalType::INPORT_ERROR;
using DAQMW::FatalType::OUTPORT_ERROR;
using DAQMW::FatalType::USER_DEFINED_ERROR1;

// Module specification
// Change following items to suit your component's spec.
static const char* merger4to1_spec[] =
{
    "implementation_id", "Merger4to1",
    "type_name",         "Merger4to1",
    "description",       "Merger4to1 component",
    "version",           "1.0",
    "vendor",            "Kazuo Nakayoshi, KEK",
    "category",          "example",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "10",
    "language",          "C++",
    "lang_type",         "compile",
    ""
};

Merger4to1::Merger4to1(RTC::Manager* manager)
    : DAQMW::DaqComponentBase(manager),
      m_InPort1("merger4to1_in1", m_in1_data),
      m_InPort2("merger4to1_in2", m_in2_data),
      m_InPort3("merger4to1_in3", m_in3_data),
      m_InPort4("merger4to1_in4", m_in4_data),
      m_OutPort("merger4to1_out", m_out_data),

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
    registerInPort("merger4to1_in1", m_InPort1);
    registerInPort("merger4to1_in2", m_InPort2);
    registerInPort("merger4to1_in3", m_InPort3);
    registerInPort("merger4to1_in4", m_InPort4);
    registerOutPort("merger4to1_out", m_OutPort);

    init_command_port();
    init_state_table();
    set_comp_name("MERGER4TO1");
}

Merger4to1::~Merger4to1()
{
}


RTC::ReturnCode_t Merger4to1::onInitialize()
{
    if (m_debug) {
        std::cerr << "Merger4to1::onInitialize()" << std::endl;
    }

    return RTC::RTC_OK;
}

RTC::ReturnCode_t Merger4to1::onExecute(RTC::UniqueId ec_id)
{
  //    std::cerr << "*** onExecute\n";
    daq_do();

    return RTC::RTC_OK;
}

int Merger4to1::daq_dummy()
{
    return 0;
}

int Merger4to1::daq_configure()
{
    std::cerr << "*** Merger4to1::configure" << std::endl;

    ::NVList* paramList;
    paramList = m_daq_service0.getCompParams();
    parse_params(paramList);

    In1_TotSiz=0;
    In1_RemainSiz=0;
    In1_CurPos=NULL;

    In2_TotSiz=0;
    In2_RemainSiz=0;
    In2_CurPos=NULL;

    In3_TotSiz=0;
    In3_RemainSiz=0;
    In3_CurPos=NULL;

    In4_TotSiz=0;
    In4_RemainSiz=0;
    In4_CurPos=NULL;

    Stock_CurNum=0;
    Stock_TotSiz=0;
    Stock_Offset=0;
    //    Cur_MaxDataSiz=67108864; // 64M (tempolary)
    Cur_MaxDataSiz=10240; // 10k (tempolary)

    m_data1=new unsigned char[Cur_MaxDataSiz];
    m_data4=(unsigned int *)m_data1;

    return 0;
}

int Merger4to1::parse_params(::NVList* list)
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


int Merger4to1::daq_unconfigure()
{
    std::cerr << "*** Merger4to1::unconfigure" << std::endl;

    delete [] m_data1; std::cout << "Delete data buffer" << std::endl;

    return 0;
}

int Merger4to1::daq_start()
{
    std::cerr << "*** Merger4to1::start" << std::endl;

    m_in1_status  = BUF_SUCCESS;
    m_in2_status  = BUF_SUCCESS;
    m_in3_status  = BUF_SUCCESS;
    m_in4_status  = BUF_SUCCESS;
    m_out_status = BUF_SUCCESS;

    return 0;
}

int Merger4to1::daq_stop()
{
    std::cerr << "*** Merger4to1::stop" << std::endl;
    //    reset_InPort1();
    //    reset_InPort2();
    //    reset_InPort3();
    //    reset_InPort4();
    return 0;
}

int Merger4to1::daq_pause()
{
    std::cerr << "*** Merger4to1::pause" << std::endl;
    return 0;
}

int Merger4to1::daq_resume()
{
    std::cerr << "*** Merger4to1::resume" << std::endl;
    return 0;
}

int Merger4to1::reset_InPort1()
{
//     uncomment if InPort is connected other OutPort of Component
//
     int ret = BUF_SUCCESS;
     while (ret == BUF_SUCCESS) {
         ret = m_InPort1.read();
     }

    std::cerr << "*** Merger4to1::InPort1 flushed\n";
    return 0;
}

int Merger4to1::reset_InPort2()
{
//     uncomment if InPort is connected other OutPort of Component
//
     int ret = BUF_SUCCESS;
     while (ret == BUF_SUCCESS) {
         ret = m_InPort2.read();
     }

    std::cerr << "*** Merger4to1::InPort2 flushed\n";
    return 0;
}

int Merger4to1::reset_InPort3()
{
//     uncomment if InPort is connected other OutPort of Component
//
     int ret = BUF_SUCCESS;
     while (ret == BUF_SUCCESS) {
         ret = m_InPort3.read();
     }

    std::cerr << "*** Merger4to1::InPort3 flushed\n";
    return 0;
}

int Merger4to1::reset_InPort4()
{
//     uncomment if InPort is connected other OutPort of Component
//
     int ret = BUF_SUCCESS;
     while (ret == BUF_SUCCESS) {
         ret = m_InPort4.read();
     }

    std::cerr << "*** Merger4to1::InPort4 flushed\n";
    return 0;
}

int Merger4to1::write_OutPort()
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
      m_out_status = BUF_SUCCESS; // successfully done
    }
    return 0; // successfully done
}

unsigned int Merger4to1::read_InPort1()
{
    unsigned int recv_byte_size = 0;
    bool ret;
    int GlobSiz;
    int preSiz;

//    std::cerr << "Entering read_InPort1: Tot=" << In1_TotSiz << ", Remain=" << In1_RemainSiz << std::endl;

    if (In1_RemainSiz==0){
      ret = m_InPort1.read();
//      std::cerr << " m_InPort1.read(): ret=" << ret << std::endl;
      if (ret==false){ // false: TIMEOUT or FATAL
	m_in1_status=check_inPort_status(m_InPort1);
//	std::cerr << " check_inPort_status(1): ret=" << m_in1_status << std::endl;
	if (m_in1_status==BUF_TIMEOUT){ // Buffer empty
          m_in1_timeout_counter++;
	  if (check_trans_lock()) {     // Check if stop command has come.
	    set_trans_unlock();       // Transit to CONFIGURE state.
	  }
        }else if (m_in1_status==BUF_FATAL){
	  fatal_error_report(INPORT_ERROR);
	}
      }else{
        std::cerr << "Get Data1. Timeout: " << m_in1_timeout_counter << " -> 0" << std::endl;
        m_in1_timeout_counter = 0;
        m_in1_status = BUF_SUCCESS;
	GlobSiz=m_in1_data.data.length();
	In1_TotSiz=GlobSiz-HEADER_BYTE_SIZE-FOOTER_BYTE_SIZE;
	In1_CurPos=(unsigned int *)&(m_in1_data.data[HEADER_BYTE_SIZE]);
        recv_byte_size=*In1_CurPos;
	In1_RemainSiz=In1_TotSiz-recv_byte_size;
	std::cerr << " reading1: Tot=" << In1_TotSiz << ", Recv=" << recv_byte_size << std::endl;
      }
    }else{
      preSiz=*In1_CurPos;
      if (In1_RemainSiz<preSiz){
	std::cerr << "Data(1) broken? Remain=" << In1_RemainSiz << ", preSiz=" << preSiz << std::endl;
	fatal_error_report(USER_DEFINED_ERROR1,"Data broken...");
      }
      In1_RemainSiz-=preSiz;
      In1_CurPos+=(preSiz/4);
      recv_byte_size=*In1_CurPos;
      std::cerr << " reading(1): Remain=" << In1_RemainSiz << ", Recv=" << recv_byte_size << std::endl;
    }

    if (m_debug) {
        std::cerr << "m_in_data.data.length():" << recv_byte_size
                  << std::endl;
    }

    return recv_byte_size;
}

unsigned int Merger4to1::read_InPort2()
{
    unsigned int recv_byte_size = 0;
    bool ret;
    int GlobSiz;
    int preSiz;

//    std::cerr << "Entering read_InPort2: Tot=" << In2_TotSiz << ", Remain=" << In2_RemainSiz << std::endl;

    if (In2_RemainSiz==0){
      ret = m_InPort2.read();
//      std::cerr << " m_InPort2.read(): ret=" << ret << std::endl;
      if (ret==false){ // false: TIMEOUT or FATAL
	m_in2_status=check_inPort_status(m_InPort2);
//	std::cerr << " check_inPort_status(2): ret=" << m_in2_status << std::endl;
	if (m_in2_status==BUF_TIMEOUT){ // Buffer empty
          m_in2_timeout_counter++;
	  if (check_trans_lock()) {     // Check if stop command has come.
	    set_trans_unlock();       // Transit to CONFIGURE state.
	  }
        }else if (m_in2_status==BUF_FATAL){
	  fatal_error_report(INPORT_ERROR);
	}
      }else{
        std::cerr << "Get Data2. Timeout: " << m_in2_timeout_counter << " -> 0" << std::endl;
        m_in2_timeout_counter = 0;
        m_in2_status = BUF_SUCCESS;
	GlobSiz=m_in2_data.data.length();
	In2_TotSiz=GlobSiz-HEADER_BYTE_SIZE-FOOTER_BYTE_SIZE;
	In2_CurPos=(unsigned int *)&(m_in2_data.data[HEADER_BYTE_SIZE]);
        recv_byte_size=*In2_CurPos;
	In2_RemainSiz=In2_TotSiz-recv_byte_size;
	std::cerr << " reading2: Tot=" << In2_TotSiz << ", Recv=" << recv_byte_size << std::endl;
      }
    }else{
      preSiz=*In2_CurPos;
      if (In2_RemainSiz<preSiz){
	std::cerr << "Data(2) broken? Remain=" << In2_RemainSiz << ", preSiz=" << preSiz << std::endl;
	fatal_error_report(USER_DEFINED_ERROR1,"Data broken...");
      }
      In2_RemainSiz-=preSiz;
      In2_CurPos+=(preSiz/4);
      recv_byte_size=*In2_CurPos;
      std::cerr << " reading(2): Remain=" << In2_RemainSiz << ", Recv=" << recv_byte_size << std::endl;
    }

    if (m_debug) {
        std::cerr << "m_in_data.data.length():" << recv_byte_size
                  << std::endl;
    }

    return recv_byte_size;
}

unsigned int Merger4to1::read_InPort3()
{
    unsigned int recv_byte_size = 0;
    bool ret;
    int GlobSiz;
    int preSiz;

//    std::cerr << "Entering read_InPort3: Tot=" << In3_TotSiz << ", Remain=" << In3_RemainSiz << std::endl;

    if (In3_RemainSiz==0){
      ret = m_InPort3.read();
//      std::cerr << " m_InPort3.read(): ret=" << ret << std::endl;
      if (ret==false){ // false: TIMEOUT or FATAL
	m_in3_status=check_inPort_status(m_InPort3);
//	std::cerr << " check_inPort_status(3): ret=" << m_in3_status << std::endl;
	if (m_in3_status==BUF_TIMEOUT){ // Buffer empty
          m_in3_timeout_counter++;
	  if (check_trans_lock()) {     // Check if stop command has come.
	    set_trans_unlock();       // Transit to CONFIGURE state.
	  }
        }else if (m_in3_status==BUF_FATAL){
	  fatal_error_report(INPORT_ERROR);
	}
      }else{
        std::cerr << "Get Data3. Timeout: " << m_in3_timeout_counter << " -> 0" << std::endl;
        m_in3_timeout_counter = 0;
        m_in3_status = BUF_SUCCESS;
	GlobSiz=m_in3_data.data.length();
	In3_TotSiz=GlobSiz-HEADER_BYTE_SIZE-FOOTER_BYTE_SIZE;
	In3_CurPos=(unsigned int *)&(m_in3_data.data[HEADER_BYTE_SIZE]);
        recv_byte_size=*In3_CurPos;
	In3_RemainSiz=In3_TotSiz-recv_byte_size;
	std::cerr << " reading3: Tot=" << In3_TotSiz << ", Recv=" << recv_byte_size << std::endl;
      }
    }else{
      preSiz=*In3_CurPos;
      if (In3_RemainSiz<preSiz){
	std::cerr << "Data(3) broken? Remain=" << In3_RemainSiz << ", preSiz=" << preSiz << std::endl;
	fatal_error_report(USER_DEFINED_ERROR1,"Data broken...");
      }
      In3_RemainSiz-=preSiz;
      In3_CurPos+=(preSiz/4);
      recv_byte_size=*In3_CurPos;
      std::cerr << " reading(3): Remain=" << In3_RemainSiz << ", Recv=" << recv_byte_size << std::endl;
    }

    if (m_debug) {
        std::cerr << "m_in_data.data.length():" << recv_byte_size
                  << std::endl;
    }

    return recv_byte_size;
}

unsigned int Merger4to1::read_InPort4()
{
    unsigned int recv_byte_size = 0;
    bool ret;
    int GlobSiz;
    int preSiz;

//    std::cerr << "Entering read_InPort4: Tot=" << In4_TotSiz << ", Remain=" << In4_RemainSiz << std::endl;

    if (In4_RemainSiz==0){
      ret = m_InPort4.read();
//      std::cerr << " m_InPort4.read(): ret=" << ret << std::endl;
      if (ret==false){ // false: TIMEOUT or FATAL
	m_in4_status=check_inPort_status(m_InPort4);
//	std::cerr << " check_inPort_status(4): ret=" << m_in4_status << std::endl;
	if (m_in4_status==BUF_TIMEOUT){ // Buffer empty
          m_in4_timeout_counter++;
	  if (check_trans_lock()) {     // Check if stop command has come.
	    set_trans_unlock();       // Transit to CONFIGURE state.
	  }
        }else if (m_in4_status==BUF_FATAL){
	  fatal_error_report(INPORT_ERROR);
	}
      }else{
        std::cerr << "Get Data4. Timeout: " << m_in4_timeout_counter << " -> 0" << std::endl;
        m_in4_timeout_counter = 0;
        m_in4_status = BUF_SUCCESS;
	GlobSiz=m_in4_data.data.length();
	In4_TotSiz=GlobSiz-HEADER_BYTE_SIZE-FOOTER_BYTE_SIZE;
	In4_CurPos=(unsigned int *)&(m_in4_data.data[HEADER_BYTE_SIZE]);
        recv_byte_size=*In4_CurPos;
	In4_RemainSiz=In4_TotSiz-recv_byte_size;
	std::cerr << " reading4: Tot=" << In4_TotSiz << ", Recv=" << recv_byte_size << std::endl;
      }
    }else{
      preSiz=*In4_CurPos;
      if (In4_RemainSiz<preSiz){
	std::cerr << "Data(4) broken? Remain=" << In4_RemainSiz << ", preSiz=" << preSiz << std::endl;
	fatal_error_report(USER_DEFINED_ERROR1,"Data broken...");
      }
      In4_RemainSiz-=preSiz;
      In4_CurPos+=(preSiz/4);
      recv_byte_size=*In4_CurPos;
      std::cerr << " reading(4): Remain=" << In4_RemainSiz << ", Recv=" << recv_byte_size << std::endl;
    }

    if (m_debug) {
        std::cerr << "m_in_data.data.length():" << recv_byte_size
                  << std::endl;
    }

    return recv_byte_size;
}

unsigned int Merger4to1::Stock_data(unsigned int data1_byte_size, unsigned int data2_byte_size,
				    unsigned int data3_byte_size, unsigned int data4_byte_size)
{
    int i;
    unsigned int tot_data_size;
    unsigned int tot_block_size;
    unsigned int tot_next_size;
    unsigned int eventnum, eventtag, seq_num;
    unsigned int comp_footer;

    unsigned int *toppos;
    unsigned int *curpos;
    int stock_size;

    unsigned int *datapos[4]; // top pointer for the data from in ports (just after the DAQ-MW header)

    datapos[0]=In1_CurPos;
    datapos[1]=In2_CurPos;
    datapos[2]=In3_CurPos;
    datapos[3]=In4_CurPos;

    //get&check event num
    std::cerr << "-----eventnum " << datapos[0][3] << ":" << datapos[1][3] << ":" << datapos[3][3] << ":" << datapos[3][3] << std::endl;
    eventnum=*(datapos[0]+3);
    if ((eventnum!=*(datapos[1]+3)) ||
	(eventnum!=*(datapos[2]+3)) ||
	(eventnum!=*(datapos[3]+3))){
      std::cerr << "Event mismatch(num):" << eventnum << " " << *(datapos[1]+3) <<
	" " << *(datapos[2]+3) <<
	" " << *(datapos[3]+3) << std::endl;
      fatal_error_report(USER_DEFINED_ERROR1,"Event mismatch(num)!");
    }
    //get&check event tag
    std::cerr << "-----eventtag " << datapos[0][4] << ":" << datapos[1][4] << ":" << datapos[2][4] << ":" << datapos[3][4] << std::endl;
    eventtag=255;
    for(i=0;i<4;i++){
      if (*(datapos[i]+4)!=255){
	if (eventtag==255) eventtag=*(datapos[i]+4);
	else
	  if (eventtag!=*(datapos[i]+4)){
	    std::cerr << "Event mismatch(tag):" << *(datapos[0]+4) << " " << *(datapos[1]+4) <<
	      " " << *(datapos[2]+4) << " " << *(datapos[3]+4) << std::endl;
	    fatal_error_report(USER_DEFINED_ERROR1,"Event mismatch(tag) !");
	  }
      }
    }

    //get size info.
    tot_block_size=data1_byte_size+data2_byte_size+data3_byte_size+data4_byte_size;
    tot_data_size=tot_block_size+80; //80 = Comp header + Comp Footer
    std::cerr << "-----size info. " << data1_byte_size << ", " << data2_byte_size << ", "
	      << data3_byte_size << ", " << data4_byte_size << ", " << tot_data_size << std::endl;

    //get current time
    seq_num=get_sequence_num();
        std::cerr << "-----seq num " << seq_num << std::endl;

    // check size of buf
    tot_next_size=Stock_Offset+tot_data_size;
    if (tot_next_size>Cur_MaxDataSiz){
       m_data1=renew_buf(m_data1,Stock_Offset,tot_next_size);
       Cur_MaxDataSiz=tot_next_size;
    }

    // create component header
    toppos=(unsigned int *)(m_data1+Stock_Offset);
    curpos=Create_Header(toppos, &comp_footer,
		  eventnum, eventtag,
		  ComponentType, ComponentID,
		  tot_data_size, (unsigned int)seq_num,
		  0,0);

    memcpy(curpos,In1_CurPos,data1_byte_size); curpos+=(data1_byte_size/4);
    memcpy(curpos,In2_CurPos,data2_byte_size); curpos+=(data2_byte_size/4);
    memcpy(curpos,In3_CurPos,data3_byte_size); curpos+=(data3_byte_size/4);
    memcpy(curpos,In4_CurPos,data4_byte_size); curpos+=(data4_byte_size/4);
    memcpy(curpos,&comp_footer,4); curpos++;
    stock_size=(curpos-toppos)*4;
    Stock_TotSiz+=stock_size;
    Stock_Offset+=stock_size;
    Stock_CurNum++;
    inc_sequence_num();

    return 0;
}

int Merger4to1::set_data(unsigned int data_byte_size)
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

int Merger4to1::daq_run()
{
  bool flag_timeout;
  struct timespec ts;
  double t0;

  //  unsigned int event_data_size;
    if (m_debug) {
        std::cerr << "*** Merger4to1::run" << std::endl;
    }

    if (m_out_status != BUF_TIMEOUT) {
      if (m_inport1_recv_data_size==0)
	m_inport1_recv_data_size = read_InPort1();
      if (m_inport2_recv_data_size==0)
        m_inport2_recv_data_size = read_InPort2();
      if (m_inport3_recv_data_size==0)
        m_inport3_recv_data_size = read_InPort3();
      if (m_inport4_recv_data_size==0)
        m_inport4_recv_data_size = read_InPort4();

      if ((m_inport1_recv_data_size > 0) &&
	  (m_inport2_recv_data_size > 0) &&
	  (m_inport3_recv_data_size > 0) &&
	  (m_inport4_recv_data_size > 0)    ){
	Stock_data(m_inport1_recv_data_size,m_inport2_recv_data_size,
		   m_inport3_recv_data_size,m_inport4_recv_data_size);
	m_inport1_recv_data_size=0;
	m_inport2_recv_data_size=0;
	m_inport3_recv_data_size=0;
	m_inport4_recv_data_size=0;
      }
    }

    if (m_out_status == BUF_TIMEOUT){
      clock_gettime(CLOCK_MONOTONIC,&ts);
      t0=(ts.tv_sec*1.)+(ts.tv_nsec/1000000000.);
      std::cout << "-w>" << std::fixed << std::setprecision(9) << t0 << std::endl;
      if (write_OutPort()<0){
	;
      }else{
	inc_total_data_size(Stock_Offset);  // increase total data byte size
	Stock_CurNum=0;
	Stock_Offset=0;
      }
      clock_gettime(CLOCK_MONOTONIC,&ts);
      t0=(ts.tv_sec*1.)+(ts.tv_nsec/1000000000.);
      std::cout << "+w>" << std::fixed << std::setprecision(9) << t0 << std::endl;
    }

    flag_timeout=( m_in1_timeout_counter>ReadTimeout ||
		   m_in2_timeout_counter>ReadTimeout ||
		   m_in3_timeout_counter>ReadTimeout ||
		   m_in4_timeout_counter>ReadTimeout ); 
    if ( (Stock_CurNum==Stock_MaxNum) || (Stock_CurNum>0 && flag_timeout) ){
      clock_gettime(CLOCK_MONOTONIC,&ts);
      t0=(ts.tv_sec*1.)+(ts.tv_nsec/1000000000.);
      std::cout << "-w>" << std::fixed << std::setprecision(9) << t0 << std::endl;
      set_data(Stock_Offset);
      if (write_OutPort()<0){
	;
      }else{
	inc_total_data_size(Stock_Offset);  // increase total data byte size
	Stock_CurNum=0;
	Stock_Offset=0;
      }
      clock_gettime(CLOCK_MONOTONIC,&ts);
      t0=(ts.tv_sec*1.)+(ts.tv_nsec/1000000000.);
      std::cout << "+w>" << std::fixed << std::setprecision(9) << t0 << std::endl;
    }

    //    sleep(1);
    //
    //    if (check_trans_lock()) {  /// got stop command
    //        set_trans_unlock();
    //        return 0;
    //    }

   return 0;
}

unsigned char * Merger4to1::renew_buf(unsigned char * orig_buf,
				      unsigned int cursize, unsigned int newsize)
{
  unsigned char * new_buf;

  new_buf = new unsigned char[newsize];
  memcpy(new_buf, orig_buf, cursize);
  delete [] orig_buf;

  std::cerr << "Re-new data buf: " << cursize << " -> " << newsize << std::endl;

  return new_buf;
}

extern "C"
{
    void Merger4to1Init(RTC::Manager* manager)
    {
        RTC::Properties profile(merger4to1_spec);
        manager->registerFactory(profile,
                    RTC::Create<Merger4to1>,
                    RTC::Delete<Merger4to1>);
    }
};
