    int recv_byte_size = 0;
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
	  //	  if (check_trans_lock()) {     // Check if stop command has come.
	  //	    set_trans_unlock();       // Transit to CONFIGURE state.
	  //	  }
        }else if (m_in4_status==BUF_FATAL){
	  fatal_error_report(INPORT_ERROR);
	}
      }else{
        if (m_debug) std::cerr << "Get Data4. Timeout: " << m_in4_timeout_counter << " -> 0" << std::endl;
        m_in4_timeout_counter = 0;
        m_in4_status = BUF_SUCCESS;
	GlobSiz=m_in4_data.data.length();
	In4_TotSiz=GlobSiz-HEADER_BYTE_SIZE-FOOTER_BYTE_SIZE;
	In4_CurPos=(unsigned int *)&(m_in4_data.data[HEADER_BYTE_SIZE]);
        recv_byte_size=(int)*In4_CurPos;
	In4_RemainSiz=In4_TotSiz-recv_byte_size;
	if (m_debug) std::cerr << " reading4: Tot=" << In4_TotSiz << ", Recv=" << recv_byte_size << std::endl;
      }
    }else{
      preSiz=(int)*In4_CurPos;
      if (In4_RemainSiz<preSiz){
	std::cerr << "Data(4) broken? Remain=" << In4_RemainSiz << ", preSiz=" << preSiz << std::endl;
	fatal_error_report(USER_DEFINED_ERROR1,"Data broken...");
      }
      In4_CurPos+=(preSiz/4);
      recv_byte_size=(int)*In4_CurPos;
      In4_RemainSiz-=recv_byte_size;
      if (m_debug) std::cerr << " reading(4): Remain=" << In4_RemainSiz << ", Recv=" << recv_byte_size << std::endl;
    }

//    if (m_debug) {
//        std::cerr << "m_in_data.data.length():" << recv_byte_size
//                  << std::endl;
//    }

    return recv_byte_size;
