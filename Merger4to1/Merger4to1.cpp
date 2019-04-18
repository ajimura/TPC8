// -*- C++ -*-
/*!
 * @file
 * @brief
 * @date
 * @author
 *
 */

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

    return 0;
}

int Merger4to1::parse_params(::NVList* list)
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


int Merger4to1::daq_unconfigure()
{
    std::cerr << "*** Merger4to1::unconfigure" << std::endl;

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

//int Merger4to1::set_data_OutPort(unsigned int data_byte_size)
//{
//  m_out_data.data.length(data_byte_size);
//  memcpy(&(m_out_data.data[0]),&m_in1_data.data[0],dtaa_byte_size);
//  retuen 0;
//}

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

int Merger4to1::set_data_OutPort(unsigned int data1_byte_size, unsigned int data2_byte_size,
				 unsigned int data3_byte_size, unsigned int data4_byte_size)
{
  int i;
    unsigned char header[8];
    unsigned char footer[8];
    unsigned int tot_data_size;
    unsigned int tot_block_size;
    unsigned int data1_block_size, data2_block_size; // size without daq-mw header and footer
    unsigned int data3_block_size, data4_block_size; // size without daq-mw header and footer
    unsigned int block_size0001, block_size0011, block_size0111, block_size1111;
    unsigned int eventnum, eventtag, seq_num;
    //    time_t curtime;
    //    tm *ltime;
    unsigned int comp_header[19],comp_footer;

    unsigned int *datapos1, *datapos2; // top pointer for the data from in ports (just after the DAQ-MW header)
    unsigned int *datapos3, *datapos4; // top pointer for the data from in ports (just after the DAQ-MW header)

    datapos1=(unsigned int *)&(m_in1_data.data[HEADER_BYTE_SIZE]);
    datapos2=(unsigned int *)&(m_in2_data.data[HEADER_BYTE_SIZE]);
    datapos3=(unsigned int *)&(m_in3_data.data[HEADER_BYTE_SIZE]);
    datapos4=(unsigned int *)&(m_in4_data.data[HEADER_BYTE_SIZE]);

    //get&check event num
    if ((eventnum=*(datapos1+3))!=*(datapos2+3)){
      if (eventnum!=*(datapos3+3)){
	if (eventnum!=*(datapos4+3)){
	  std::cerr << "Event mismatch(num):" << eventnum << " " << *(datapos2+3) <<
	    " " << *(datapos3+3) <<
	    " " << *(datapos4+3) << std::endl;
	  fatal_error_report(USER_DEFINED_ERROR1,"Event mismatch(num) !");
	}}}
    //    std::cerr << "-----eventnum " << datapos1[3] << ":" << datapos2[3] << std::endl;

    //get&check event tag
    if ((eventtag=*(datapos1+4))!=*(datapos2+4)){
      if (eventnum!=*(datapos3+4)){
	if (eventnum!=*(datapos4+4)){
	  std::cerr << "Event mismatch(tag):" << eventtag << " " << *(datapos2+4) <<
	    " " << *(datapos3+4) <<
	    " " << *(datapos4+4) << std::endl;
	  fatal_error_report(USER_DEFINED_ERROR1,"Event mismatch(tag) !");
	}}}
    //    std::cerr << "-----eventtag " << datapos1[4] << ":" << datapos2[4] << std::endl;


    //get size info.
    data1_block_size=data1_byte_size-HEADER_BYTE_SIZE-FOOTER_BYTE_SIZE;
    data2_block_size=data2_byte_size-HEADER_BYTE_SIZE-FOOTER_BYTE_SIZE;
    data3_block_size=data3_byte_size-HEADER_BYTE_SIZE-FOOTER_BYTE_SIZE;
    data4_block_size=data4_byte_size-HEADER_BYTE_SIZE-FOOTER_BYTE_SIZE;
    block_size0001=data1_block_size;
    block_size0011=block_size0001+data2_block_size;
    block_size0111=block_size0011+data3_block_size;
    block_size1111=block_size0111+data4_block_size;
    tot_block_size=data1_block_size+data2_block_size+data3_block_size+data4_block_size;;
    tot_data_size=tot_block_size+80; //+HEADER_BYTE_SIZE+FOOTER_BYTE_SIZE; //80 = Comp header + Comp Footer
    //    std::cerr << "-----size info. " << data1_block_size << ", " << data2_block_size << ", "
    //	                            << data3_block_size << ", " << data4_block_size << " -> "
    //                                    << tot_data_size << std::endl;

    //get current time
    seq_num=get_sequence_num();
    //    std::cerr << "-----seq num " << seq_num << std::endl;

    // create component header
    Create_Header(comp_header, &comp_footer,
		  eventnum, eventtag,
		  ComponentType, ComponentID,
		  tot_data_size, (unsigned int)seq_num,
		  0,0);

//     curtime=time(0);
//     ltime=localtime(&curtime);
//     //create component hedaer&footer
//     comp_header[0]=tot_data_size;
//     comp_header[1]=(ltime->tm_year+1900)*1000000+(ltime->tm_mon+1)*10000+(ltime->tm_mday)*100+ltime->tm_hour;
//     comp_header[2]=(ltime->tm_min)*10000000 + (ltime->tm_sec)*100000;
//     comp_header[3]=eventnum;
//     comp_header[4]=eventtag;
//     comp_header[5]=ComponentType;
//     comp_header[6]=ComponentID;
//     comp_header[ 7]=0;   comp_header[ 8]=0;   comp_header[ 9]=0;   comp_header[10]=0;    comp_header[11]=0;
//     comp_header[12]=0;   comp_header[13]=0;   comp_header[14]=0;   comp_header[15]=0;
//     comp_header[16]=0; // numFADC
//     comp_header[17]=0; // numFADC
//     comp_header[18]=(unsigned int)seq_num;
//     comp_footer=0x00000011;

    //set daq-mw header&footer
    set_header(&header[0], tot_data_size);
    set_footer(&footer[0]);
    //    for(i=0;i<8;i++){
    //      std::cerr << (unsigned int)header[i] << ":";
    //    }std::cerr << std::endl;
    //    for(i=0;i<8;i++){
    //      std::cerr << (unsigned int)footer[i] << ":";
    //    }std::cerr << std::endl;
      
    // set OutPort buffer length
    m_out_data.data.length(tot_data_size+HEADER_BYTE_SIZE+FOOTER_BYTE_SIZE);
    // put all the data and header/footer
    memcpy(&(m_out_data.data[0]),                             &header[0], HEADER_BYTE_SIZE);
    memcpy(&(m_out_data.data[HEADER_BYTE_SIZE]),              &comp_header[0], 76);
    memcpy(&(m_out_data.data[HEADER_BYTE_SIZE+76]),           &m_in1_data.data[HEADER_BYTE_SIZE], data1_block_size);
    memcpy(&(m_out_data.data[HEADER_BYTE_SIZE+76+block_size0001]), 
	                                                      &m_in2_data.data[HEADER_BYTE_SIZE],data2_block_size);
    memcpy(&(m_out_data.data[HEADER_BYTE_SIZE+76+block_size0011]),           
	                                                      &m_in3_data.data[HEADER_BYTE_SIZE], data1_block_size);
    memcpy(&(m_out_data.data[HEADER_BYTE_SIZE+76+block_size0111]), 
	                                                      &m_in4_data.data[HEADER_BYTE_SIZE],data2_block_size);
    memcpy(&(m_out_data.data[HEADER_BYTE_SIZE+76+block_size1111]),
	                                                      &comp_footer, 4);
    memcpy(&(m_out_data.data[HEADER_BYTE_SIZE+80+block_size1111]),
	                                                      &footer[0], FOOTER_BYTE_SIZE);

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
    }
    return 0; // successfully done
}

unsigned int Merger4to1::read_InPort1()
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

unsigned int Merger4to1::read_InPort2()
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

unsigned int Merger4to1::read_InPort3()
{
    /////////////// read data from InPort Buffer ///////////////
    unsigned int recv_byte_size = 0;
    bool ret = m_InPort3.read();

    //////////////////// check read status /////////////////////
    if (ret == false) { // false: TIMEOUT or FATAL
        m_in3_status = check_inPort_status(m_InPort3);
        if (m_in3_status == BUF_TIMEOUT) { // Buffer empty.
            m_in3_timeout_counter++;
            if (check_trans_lock()) {     // Check if stop command has come.
                set_trans_unlock();       // Transit to CONFIGURE state.
            }
        }
        else if (m_in3_status == BUF_FATAL) { // Fatal error
            fatal_error_report(INPORT_ERROR);
        }
    }
    else { // success
        m_in3_timeout_counter = 0;
        recv_byte_size = m_in3_data.data.length();
        m_in3_status = BUF_SUCCESS;
    }
    if (m_debug) {
        std::cerr << "m_in3_data.data.length():" << recv_byte_size
                  << std::endl;
    }

    return recv_byte_size;
}

unsigned int Merger4to1::read_InPort4()
{
    /////////////// read data from InPort Buffer ///////////////
    unsigned int recv_byte_size = 0;
    bool ret = m_InPort4.read();

    //////////////////// check read status /////////////////////
    if (ret == false) { // false: TIMEOUT or FATAL
        m_in4_status = check_inPort_status(m_InPort4);
        if (m_in4_status == BUF_TIMEOUT) { // Buffer empty.
            m_in4_timeout_counter++;
            if (check_trans_lock()) {     // Check if stop command has come.
                set_trans_unlock();       // Transit to CONFIGURE state.
            }
        }
        else if (m_in4_status == BUF_FATAL) { // Fatal error
            fatal_error_report(INPORT_ERROR);
        }
    }
    else { // success
        m_in4_timeout_counter = 0;
        recv_byte_size = m_in4_data.data.length();
        m_in4_status = BUF_SUCCESS;
    }
    if (m_debug) {
        std::cerr << "m_in4_data.data.length():" << recv_byte_size
                  << std::endl;
    }

    return recv_byte_size;
}

int Merger4to1::daq_run()
{
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

      if ((m_inport1_recv_data_size == 0) ||
	  (m_inport2_recv_data_size == 0) ||
	  (m_inport3_recv_data_size == 0) ||
	  (m_inport4_recv_data_size == 0)    ){ // TIMEOUT
	return 0;
      } else {
	  //            check_header_footer(m_in1_data, m_inport1_recv_data_size);
	  //            check_header_footer(m_in2_data, m_inport2_recv_data_size);
	set_data_OutPort(m_inport1_recv_data_size,m_inport2_recv_data_size,
			 m_inport3_recv_data_size,m_inport4_recv_data_size);
      }
    }

    if ((m_in1_status != BUF_TIMEOUT) && 
	(m_in2_status != BUF_TIMEOUT) && 
	(m_in3_status != BUF_TIMEOUT) && 
	(m_in4_status != BUF_TIMEOUT) && 
	(m_out_status != BUF_TIMEOUT)    ) {
        if (write_OutPort() < 0) { // TIMEOUT
            ; // do nothing
        }
        else {
            m_out_status = BUF_SUCCESS;
        }
    }

    if ((m_in1_status != BUF_TIMEOUT) &&
	(m_in2_status != BUF_TIMEOUT) &&
	(m_in3_status != BUF_TIMEOUT) &&
	(m_in4_status != BUF_TIMEOUT) &&
        (m_out_status != BUF_TIMEOUT)    ) {
        inc_sequence_num();                    // increase sequence num.
	//        unsigned int event_data_size = get_event_size(m_inport_recv_data_size);
	//	event_data_size=m_inport1_recv_data_size+m_inport2_recv_data_size+80;
	//        inc_total_data_size(event_data_size);  // increase total data byte size
	inc_total_data_size(m_inport1_recv_data_size);  // increase total data byte size
	inc_total_data_size(m_inport2_recv_data_size);  // increase total data byte size
	inc_total_data_size(m_inport3_recv_data_size);  // increase total data byte size
	inc_total_data_size(m_inport4_recv_data_size);  // increase total data byte size
	m_inport1_recv_data_size=0;
	m_inport2_recv_data_size=0;
	m_inport3_recv_data_size=0;
	m_inport4_recv_data_size=0;
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
    void Merger4to1Init(RTC::Manager* manager)
    {
        RTC::Properties profile(merger4to1_spec);
        manager->registerFactory(profile,
                    RTC::Create<Merger4to1>,
                    RTC::Delete<Merger4to1>);
    }
};
