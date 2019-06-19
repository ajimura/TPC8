// -*- C++ -*-
/*!
 * @file TPClogger.cpp
 * @brief Event data logging component.
 * @date
 * @author Kazuo Nakayoshi <kazuo.nakayoshi@kek.jp>
 *
 * Copyright (C) 2011
 *     Kazuo Nakayoshi
 *     Electronics System Group,
 *     KEK, Japan.
 *     All rights reserved.
 *
 */

#include "TPClogger.h"
#include "daqmwlib.h"

// Possible fatal errors for this component
using DAQMW::FatalType::BAD_DIR;
using DAQMW::FatalType::INPORT_ERROR;
using DAQMW::FatalType::CANNOT_OPEN_FILE;
using DAQMW::FatalType::CANNOT_WRITE_DATA;
using DAQMW::FatalType::USER_DEFINED_ERROR1;

// Module specification
static const char* mylogger_spec[] = {
    "implementation_id", "TPClogger",
    "type_name",         "TPClogger",
    "description",       "Event data Logging component",
    "version",           "1.0",
    "vendor",            "Kazuo Nakayoshi, KEK",
    "category",          "example",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    ""
};

TPClogger::TPClogger(RTC::Manager* manager)
    : DAQMW::DaqComponentBase(manager),
      m_InPort("tpclogger_in", m_in_data),
      m_isDataLogging(false),
      m_filesOpened(false),
      m_in_status(BUF_SUCCESS),
      m_update_rate(100),
      m_debug(false)
{
    // Registration: InPort/OutPort/Service
    registerInPort("tpclogger_in", m_InPort);

    init_command_port();
    init_state_table();
    set_comp_name("TPClogger");
}

TPClogger::~TPClogger()
{
}

RTC::ReturnCode_t TPClogger::onInitialize()
{
    if (m_debug) {
        std::cerr << "TPClogger::onInitialize()" << std::endl;
    }

    return RTC::RTC_OK;
}

RTC::ReturnCode_t TPClogger::onExecute(RTC::UniqueId ec_id)
{
    daq_do();

    return RTC::RTC_OK;
}

int TPClogger::daq_dummy()
{
    return 0;
}

int TPClogger::daq_configure()
{
    std::cerr << "*** TPClogger::configure" << std::endl;
    int ret = 0;
    m_isDataLogging = false;

    ::NVList* list = m_daq_service0.getCompParams();
    parse_params(list);

    In_TotSiz=0;
    In_RemainSiz=0;
    In_CurPos=NULL;
    In_Done=0;

    return ret;
}

int TPClogger::parse_params(::NVList* list)
{
    int ret = 0;

    bool isExistParamLogging = false;
    bool isExistParamDirName = false;

    int length = (*list).length();
    for (int i = 0; i < length; i += 2) {
        if (m_debug) {
            std::cerr << "NVList[" << (*list)[i].name
                      << "," << (*list)[i].value << "]" << std::endl;
        }
        std::string sname  = (std::string)(*list)[i].value;
        std::string svalue = (std::string)(*list)[i + 1].value;
        if (sname == "eventByteSize") {
            unsigned int eventByteSize = atoi(svalue.c_str());
            set_event_byte_size(eventByteSize);
            std::cerr << "ventByteSize:"
                      << eventByteSize << std::endl;
        }
        if (sname == "isLogging") {
            toLower(svalue); // all characters of cvale are converted to lower.
            isExistParamLogging = true;
            if (svalue == "yes") {
                m_isDataLogging = true;
                fileUtils = new FileUtils();
                isExistParamLogging = true;
                std::cerr << "TPClogger: Save to file: true\n";
            }
            else if (svalue == "no") {
                m_isDataLogging = false;
                isExistParamLogging = true;
                std::cerr << "TPClogger: Save to file: false\n";
            }
        }

        if (sname == "monRate") {
            m_update_rate = atoi(svalue.c_str());
            if (m_debug) {
                std::cerr << "update rate:" << m_update_rate << std::endl;
            }
        }

	if (sname == "ComponentID") ComponentID=atoi(svalue.c_str());
    }

    if (m_isDataLogging) {
        for (int i = 0; i < length ; i += 2) {
            if (m_debug) {
                std::cerr << "NVList[" << (*list)[i].name
                          << "," << (*list)[i].value << "]" << std::endl;
            }
            std::string sname  = (std::string)(*list)[i].value;
            std::string svalue = (std::string)(*list)[i + 1].value;

            if (sname == "dirName") {
                isExistParamDirName = true;
                m_dirName = svalue;
                if (m_isDataLogging) {
                    std::cerr << "Dir name for data saving:"
                              << m_dirName << std::endl;
                    if (fileUtils->check_dir(m_dirName) != true) {
                        delete fileUtils;
                        fileUtils = 0;
                        std::cerr << "Can not open directory:"
                                  << m_dirName << std::endl;
                        fatal_error_report(BAD_DIR);
                    }
                }
            }

            if (sname == "runNumber") {
                unsigned int runNumber = atoi(svalue.c_str());
                set_run_number(runNumber);
                std::cerr << "Run Number:"
                          << runNumber << std::endl;
            }

            if (sname == "maxFileSizeInMegaByte") {
                m_maxFileSizeInMByte = strtoul(svalue.c_str(), NULL, 0);
                std::cerr << "Max File size(MByte):"
                          << m_maxFileSizeInMByte << std::endl;
            }
        }
    }

    return ret;
}

int TPClogger::daq_unconfigure()
{
    std::cerr << "*** TPClogger::unconfigure" << std::endl;
    if (m_isDataLogging) {
        delete fileUtils;
        if (m_debug) {
            std::cerr << "fileUtils deleted\n";
        }
        fileUtils = 0;
    }
    return 0;
}

int TPClogger::daq_start()
{
    std::cerr << "*** TPClogger::start" << std::endl;

    m_in_status = BUF_SUCCESS;
    m_filesOpened = false;
    unsigned int runNumber = m_daq_service0.getRunNo();
    if (m_debug) {
        std::cerr << "runNumber:" << runNumber << std::endl;
    }
    if (m_isDataLogging) {
        int ret = 0;
        fileUtils->set_run_no(runNumber);
        if (m_debug) {
            std::cerr << "m_maxFileSizeInMByte:"
                      << m_maxFileSizeInMByte << std::endl;
        }
        fileUtils->set_max_size_in_megaBytes(m_maxFileSizeInMByte);
        ret = fileUtils->open_file(m_dirName);
        if (ret < 0) {
            std::cerr << "### ERROR: TPClogger: open file failed\n";
            fatal_error_report(CANNOT_OPEN_FILE);
        }
        else {
            std::cerr << "*** TPClogger: file open succeed\n";
            m_filesOpened = true;
        }
    }
    return 0;
}

int TPClogger::daq_stop()
{
    std::cerr << "*** TPClogger::stop" << std::endl;

    if (m_isDataLogging && m_filesOpened) {
        if (m_debug) {
            std::cerr << "TPClogger::stop: close files \n";
        }
        fileUtils->close_file();
    }

    reset_InPort();
    return 0;
}

int TPClogger::daq_pause()
{
    std::cerr << "*** TPClogger::pause" << std::endl;
    return 0;
}

int TPClogger::daq_resume()
{
    std::cerr << "*** TPClogger::resume" << std::endl;
    return 0;
}

int TPClogger::reset_InPort()
{
    bool ret = true;
    while (ret) {
        ret = m_InPort.read();
        if (ret == true) {
            std::cerr << "m_in_data.data.length(): "
                      << m_in_data.data.length() << std::endl;
        }
    }
    if (m_debug) {
        std::cerr << "*** TPClogger::InPort flushed\n";
    }
    return 0;
}

void TPClogger::toLower(std::basic_string<char>& s)
{
    for (std::basic_string<char>::iterator p = s.begin(); p != s.end(); ++p) {
        *p = tolower(*p);
    }
}

unsigned int TPClogger::read_InPort()
{
    unsigned int recv_byte_size = 0;
    bool ret;
    int GlobSiz;
    int preSiz;

    //    std::cerr << "Entering read_InPort: Tot=" << In_TotSiz << ", Remain=" << In_RemainSiz << std::endl;

    if (In_RemainSiz==0){
      ret = m_InPort.read();
      //      std::cerr << " m_InPort.read(): ret=" << ret << std::endl;
      if (ret==false){ // false: TIMEOUT or FATAL
	m_in_status=check_inPort_status(m_InPort);
	//	std::cerr << " check_inPort_status(): ret=" << m_in_status << std::endl;
	if (m_in_status==BUF_TIMEOUT){ // Buffer empty
	  //	  m_in_timeout_counter++;
	  if (check_trans_lock()) {     // Check if stop command has come.
	    set_trans_unlock();       // Transit to CONFIGURE state.
	  }
        }else if (m_in_status==BUF_FATAL){
	  fatal_error_report(INPORT_ERROR);
	}
      }else{
	//        m_in_timeout_counter = 0;
        m_in_status = BUF_SUCCESS;
	GlobSiz=m_in_data.data.length();
	In_TotSiz=GlobSiz-HEADER_BYTE_SIZE-FOOTER_BYTE_SIZE;
	In_CurPos=(unsigned int *)&(m_in_data.data[HEADER_BYTE_SIZE]);
        recv_byte_size=*In_CurPos;
	In_RemainSiz=In_TotSiz-recv_byte_size;
	std::cerr << " reading: Tot=" << In_TotSiz << ", Recv=" << recv_byte_size << std::endl;
      }
    }else{
      preSiz=*In_CurPos;
      if (In_RemainSiz<preSiz){
	std::cerr << "Data broken? Remain=" << In_RemainSiz << ", preSiz=" << preSiz << std::endl;
	fatal_error_report(USER_DEFINED_ERROR1,"Data broken...");
      }
      In_RemainSiz-=preSiz;
      In_CurPos+=(preSiz/4);
      recv_byte_size=*In_CurPos;
      std::cerr << " reading: Remain=" << In_RemainSiz << ", Recv=" << recv_byte_size << std::endl;
    }

    if (m_debug) {
        std::cerr << "m_in_data.data.length():" << recv_byte_size
                  << std::endl;
    }

    return recv_byte_size;
}

int TPClogger::daq_run()
{

    unsigned int comp_header[19],comp_footer;
    unsigned int *top_ptr;
    unsigned int eventnum, eventtag, seq_num;

    int event_byte_size = 0;

    event_byte_size=read_InPort();

    if (event_byte_size==0){ // no input
      if (check_trans_lock()) {
	if (In_Done==1){
	  set_trans_unlock();
	}else{
	  In_Done=1;
	}
      }
      return 0;
    }

    In_Done=0;

    if (m_isDataLogging) {

      eventnum=*(In_CurPos+3);
      eventtag=*(In_CurPos+4);
      seq_num=(unsigned int)get_sequence_num();
      // create component header
      Create_Header(comp_header, &comp_footer,
		    eventnum, eventtag,
		    ComponentType, ComponentID,
		    event_byte_size+80, seq_num,
		    0,0);

        int ret0 = fileUtils->write_data((char *)comp_header,76);

        int ret1 = fileUtils->write_data((char *)In_CurPos,
                                        event_byte_size);
        int ret2 = fileUtils->write_data((char *)&comp_footer,4);
	
        if (ret0 < 0 || ret1 < 0 || ret2 < 0) {
            std::cerr << "### TPClogger: ERROR occured at data saving\n";
            fatal_error_report(CANNOT_WRITE_DATA);
        }
    }

    inc_total_data_size(event_byte_size+80);
    inc_sequence_num();

    if (m_debug) {
        seq_num = (unsigned int)get_sequence_num();
        if (seq_num % m_update_rate == 0) {
            std::cerr << "TPClogger: loop = " << seq_num << std::endl;
            std::cerr << "\033[A\r";
        }
    }

    return 0;
}

extern "C"
{
    void TPCloggerInit(RTC::Manager* manager)
    {
        RTC::Properties profile(mylogger_spec);
        manager->registerFactory(profile,
                                 RTC::Create<TPClogger>,
                                 RTC::Delete<TPClogger>);
    }
};
