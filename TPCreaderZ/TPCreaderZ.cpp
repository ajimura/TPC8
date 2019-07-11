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
#include <zlib.h>
#include "TPCreaderZ.h"
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
static const char* tpcreaderz_spec[] =
{
    "implementation_id", "TPCreaderZ",
    "type_name",         "TPCreaderZ",
    "description",       "TPCreaderZ component",
    "version",           "1.0",
    "vendor",            "Kazuo Nakayoshi, KEK",
    "category",          "example",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "2",
    "language",          "C++",
    "lang_type",         "compile",
    ""
};

TPCreaderZ::TPCreaderZ(RTC::Manager* manager)
    : DAQMW::DaqComponentBase(manager),
      m_OutPort("tpcreaderz_out", m_out_data),
      m_recv_byte_size(0),
      m_recv_timeout_counter(0),
      m_out_status(BUF_SUCCESS),
      m_out_timeout_counter(0)
{
    // Registration: InPort/OutPort/Service

    // Set OutPort buffers
    registerOutPort("tpcreaderz_out", m_OutPort);

    init_command_port();
    init_state_table();
    set_comp_name("TPCREADERZ");
}

TPCreaderZ::~TPCreaderZ()
{
}

RTC::ReturnCode_t TPCreaderZ::onInitialize()
{
    if (m_debug) {
        std::cerr << "TPCreaderZ::onInitialize()" << std::endl;
    }

    return RTC::RTC_OK;
}

RTC::ReturnCode_t TPCreaderZ::onExecute(RTC::UniqueId ec_id)
{
    daq_do();

    return RTC::RTC_OK;
}

int TPCreaderZ::daq_dummy()
{
    return 0;
}

int TPCreaderZ::daq_configure()
{
    std::cerr << "*** TPCreaderZ::configure" << std::endl;
#include "daq_configure.inc"
}

int TPCreaderZ::parse_params(::NVList* list)
{
#include "parse_params.inc"
}

int TPCreaderZ::daq_unconfigure()
{
    std::cerr << "*** TPCreaderZ::unconfigure" << std::endl;

    delete [] m_dataA; std::cout << "Delete data bufferA" << std::endl;
    delete [] m_dataB; std::cout << "Delete data bufferB" << std::endl;
    fadc_close(); std::cout << "Close Spacewire ports" << std::endl;
    trigio_fin();

    return 0;
}

int TPCreaderZ::daq_start()
{
    std::cerr << "*** TPCreaderZ::start" << std::endl;
#include "daq_start.inc"
}

int TPCreaderZ::daq_stop()
{
    std::cerr << "*** TPCreaderZ::stop" << std::endl;
#include "daq_stop.inc"
}

int TPCreaderZ::daq_pause()
{
    std::cerr << "*** TPCreaderZ::pause" << std::endl;
#include "daq_pause.inc"
}

int TPCreaderZ::daq_resume()
{
    std::cerr << "*** TPCreaderZ::resume" << std::endl;
#include "daq_resume.inc"
}

int TPCreaderZ::read_data_from_detectors()
{
#include "read_data_from_detectors.inc"
}

int TPCreaderZ::set_data(int data_byte_size)
{
    unsigned char header[8];
    unsigned char footer[8];

  unsigned char comp_buf[Stock_MaxSiz];
  unsigned char deco_buf[Stock_MaxSiz];
  unsigned int *comp_buf4=(unsigned int *)&(comp_buf[0]);
  unsigned int *deco_buf4=(unsigned int *)&(deco_buf[0]);
  unsigned long origsize,compsize,decompsize;
  unsigned int *toppos=m_data4;
  unsigned int outsize, bufsize;

  if (m_debug)
    std::cerr << "in set_data: data_byte_size=" << data_byte_size << std::endl;

  m_out_data.data.length((unsigned int)Stock_MaxSiz);
  if (m_debug)
    std::cerr << "Outdata address=" << reinterpret_cast<void *>(&(m_out_data.data[0])) << std::endl;

  if (OutCompress){
    compsize=Stock_MaxSiz;
    if (compress(&(m_out_data.data[HEADER_BYTE_SIZE+4]),&compsize,m_data1,data_byte_size)!=Z_OK){
      printf("error in compress()\n");
    }
    if (m_debug) printf("Compressed: size=%d -> %lu\n",data_byte_size,compsize);
    outsize=(unsigned int)(compsize+8);
    bufsize=outsize|0xf0000000;
    memcpy(&(m_out_data.data[HEADER_BYTE_SIZE]), &bufsize, 4); // 4+4+compressed size
    memcpy(&(m_out_data.data[HEADER_BYTE_SIZE+4]), &data_byte_size, 4); //original size
  }else{
    memcpy(&(m_out_data.data[HEADER_BYTE_SIZE]), &m_data1[0], (size_t)data_byte_size);
    outsize=(unsigned int)data_byte_size;
  }

  set_header(&header[0], outsize);
  set_footer(&footer[0]);

  ///set OutPort buffer length
  memcpy(&(m_out_data.data[0]), &header[0], HEADER_BYTE_SIZE);
  //    memcpy(&(m_out_data.data[HEADER_BYTE_SIZE]), &m_data1[0], (size_t)data_byte_size);
  memcpy(&(m_out_data.data[HEADER_BYTE_SIZE + outsize]), &footer[0],
	 FOOTER_BYTE_SIZE);

  m_out_data.data.length((unsigned int)outsize + HEADER_BYTE_SIZE + FOOTER_BYTE_SIZE);

// check compression by zlib
//    printf("Orig: %08x %08x %08x %08x %08x %08x %08x %08x\n",
//	   *toppos,*(toppos+1),*(toppos+2),*(toppos+3),
//	   *(toppos+4),*(toppos+5),*(toppos+6),*(toppos+7));
//    compsize=Stock_MaxSiz;
//    if (compress(comp_buf,&compsize,(unsigned char *)toppos,data_byte_size)!=Z_OK){
//      printf("error in compress()\n");
//    }
//    printf("Compressed: size=%ld -> %ld\n",data_byte_size,compsize);
//    printf("Orig: %08x %08x %08x %08x %08x %08x %08x %08x\n",
//	   *comp_buf4,*(comp_buf4+1),*(comp_buf4+2),*(comp_buf4+3),
//	   *(comp_buf4+4),*(comp_buf4+5),*(comp_buf4+6),*(comp_buf4+7));
//    decompsize=Stock_MaxSiz;
//    if (uncompress(deco_buf,&decompsize,comp_buf,compsize)!=Z_OK){
//      printf("error in uncompress()\n");
//    }
//    printf("Uncompressed: size=%ld -> %ld\n",compsize,decompsize);
//    printf("Orig: %08x %08x %08x %08x %08x %08x %08x %08x\n",
//	   *deco_buf4,*(deco_buf4+1),*(deco_buf4+2),*(deco_buf4+3),
//	   *(deco_buf4+4),*(deco_buf4+5),*(deco_buf4+6),*(deco_buf4+7));

    return 0;
}

int TPCreaderZ::write_OutPort()
{
  struct timespec ts;
  double t0,t1;

  if (m_debug) {
    std::cerr << "write: StockNum=" << Stock_CurNum << " SockSize=" << Stock_Offset << std::endl;
  }

  if (m_debug) {
    clock_gettime(CLOCK_MONOTONIC,&ts);
    t0=(ts.tv_sec*1.)+(ts.tv_nsec/1000000000.);
  }

  ////////////////// send data from OutPort  //////////////////
  bool ret = m_OutPort.write();

  //////////////////// check write status /////////////////////
  if (ret == false) {  // TIMEOUT or FATAL
    m_out_status  = check_outPort_status(m_OutPort);
    // Fatal error
    if (m_out_status == BUF_FATAL) fatal_error_report(OUTPORT_ERROR);
    // Timeout
    if (m_out_status == BUF_TIMEOUT) return -1;
    // No Buffer on Downstream
    if (m_out_status == BUF_NOBUF) return -1;
  } else {
    m_out_status = BUF_SUCCESS; // successfully done
  }

  if (m_debug) {
    clock_gettime(CLOCK_MONOTONIC,&ts);
    t1=(ts.tv_sec*1.)+(ts.tv_nsec/1000000000.);
    std::cout << "write time: " << std::fixed << std::setprecision(9) << t1-t0 << std::endl;
  }

  return 0;
}

int TPCreaderZ::daq_run()
{
#include "daq_run.inc"
}

extern "C"
{
    void TPCreaderZInit(RTC::Manager* manager)
    {
        RTC::Properties profile(tpcreaderz_spec);
        manager->registerFactory(profile,
                    RTC::Create<TPCreaderZ>,
                    RTC::Delete<TPCreaderZ>);
    }
};

void TPCreaderZ::toLower(std::basic_string<char>& s)
{
    for (std::basic_string<char>::iterator p = s.begin(); p != s.end(); ++p) {
        *p = tolower(*p);
    }
}

