    int recv_byte_size = 0;
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
	  //	  if (check_trans_lock()) {     // Check if stop command has come.
	  //	    set_trans_unlock();       // Transit to CONFIGURE state.
	  //	  }
        }else if (m_in3_status==BUF_FATAL){
	  fatal_error_report(INPORT_ERROR);
	}
      }else{
        if (m_debug) std::cerr << "Get Data3. Timeout: " << m_in3_timeout_counter << " -> 0" << std::endl;
        m_in3_timeout_counter = 0;
        m_in3_status = BUF_SUCCESS;
	GlobSiz=m_in3_data.data.length();
	In3_TotSiz=GlobSiz-HEADER_BYTE_SIZE-FOOTER_BYTE_SIZE;
	In3_CurPos=(unsigned int *)&(m_in3_data.data[HEADER_BYTE_SIZE]);
        recv_byte_size=(int)*In3_CurPos;
	In3_RemainSiz=In3_TotSiz-recv_byte_size;
	if (m_debug) std::cerr << " reading3: Tot=" << In3_TotSiz << ", Recv=" << recv_byte_size << std::endl;
      }
    }else{
      preSiz=(int)*In3_CurPos;
      if (In3_RemainSiz<preSiz){
	std::cerr << "Data(3) broken? Remain=" << In3_RemainSiz << ", preSiz=" << preSiz << std::endl;
	fatal_error_report(USER_DEFINED_ERROR1,"Data broken...");
      }
      In3_RemainSiz-=preSiz;
      In3_CurPos+=(preSiz/4);
      recv_byte_size=(int)*In3_CurPos;
      if (m_debug) std::cerr << " reading(3): Remain=" << In3_RemainSiz << ", Recv=" << recv_byte_size << std::endl;
    }

//    if (m_debug) {
//        std::cerr << "m_in_data.data.length():" << recv_byte_size
//                  << std::endl;
//    }

    return recv_byte_size;
