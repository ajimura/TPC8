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
//	  if (check_trans_lock()) {     // Check if stop command has come.
//	    set_trans_unlock();       // Transit to CONFIGURE state.
//	  }
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

//    if (m_debug) {
//        std::cerr << "m_in_data.data.length():" << recv_byte_size
//                  << std::endl;
//    }

    return recv_byte_size;
