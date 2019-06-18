    int recv_byte_size = 0;
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
	  //	  if (check_trans_lock()) {     // Check if stop command has come.
	  //	    set_trans_unlock();       // Transit to CONFIGURE state.
	  //	  }
        }else if (m_in2_status==BUF_FATAL){
	  fatal_error_report(INPORT_ERROR);
	}
      }else{
        if (m_debug) std::cerr << "Get Data2. Timeout: " << m_in2_timeout_counter << " -> 0" << std::endl;
        m_in2_timeout_counter = 0;
        m_in2_status = BUF_SUCCESS;
	GlobSiz=m_in2_data.data.length();
	In2_TotSiz=GlobSiz-HEADER_BYTE_SIZE-FOOTER_BYTE_SIZE;
	In2_CurPos=(unsigned int *)&(m_in2_data.data[HEADER_BYTE_SIZE]);
        recv_byte_size=(int)*In2_CurPos;
	In2_RemainSiz=In2_TotSiz-recv_byte_size;
	if (m_debug) std::cerr << " reading2: Tot=" << In2_TotSiz << ", Recv=" << recv_byte_size << std::endl;
      }
    }else{
      preSiz=(int)*In2_CurPos;
      if (In2_RemainSiz<preSiz){
	std::cerr << "Data(2) broken? Remain=" << In2_RemainSiz << ", preSiz=" << preSiz << std::endl;
	fatal_error_report(USER_DEFINED_ERROR1,"Data broken...");
      }
      In2_RemainSiz-=preSiz;
      In2_CurPos+=(preSiz/4);
      recv_byte_size=(int)*In2_CurPos;
      if (m_debug) std::cerr << " reading(2): Remain=" << In2_RemainSiz << ", Recv=" << recv_byte_size << std::endl;
    }

//    if (m_debug) {
//        std::cerr << "m_in_data.data.length():" << recv_byte_size
//                  << std::endl;
//    }

    return recv_byte_size;
