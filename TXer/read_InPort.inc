    int recv_byte_size = 0;
    bool ret;
    int GlobSiz;
    int preSiz;

//    std::cerr << "Entering read_InPort: Tot=" << In_TotSiz << ", Remain=" << In_RemainSiz << std::endl;

    if (In_RemainSiz==0){
      ret = m_InPort.read();
//      std::cerr << " m_InPort.read(): ret=" << ret << std::endl;
      if (ret==false){ // false: TIMEOUT or FATAL
	m_in_status=check_inPort_status(m_InPort);
//	std::cerr << " check_inPort_status: ret=" << m_in_status << std::endl;
	if (m_in_status==BUF_TIMEOUT){ // Buffer empty
          m_in_timeout_counter++;
//	  if (check_trans_lock()) {     // Check if stop command has come.
//	    set_trans_unlock();       // Transit to CONFIGURE state.
//	  }
        }else if (m_in_status==BUF_FATAL){
	  fatal_error_report(INPORT_ERROR);
	}
      }else{
        if (m_debug) std::cerr << "Get Data. Timeout: " << m_in_timeout_counter << " -> 0" << std::endl;
        m_in_timeout_counter = 0;
        m_in_status = BUF_SUCCESS;
	GlobSiz=m_in_data.data.length();
	In_TotSiz=GlobSiz-HEADER_BYTE_SIZE-FOOTER_BYTE_SIZE;
	In_CurPos=(unsigned int *)&(m_in_data.data[HEADER_BYTE_SIZE]);
        recv_byte_size=(int)*In_CurPos;
	In_RemainSiz=In_TotSiz-recv_byte_size;
	if (m_debug) std::cerr << " reading: Tot=" << In_TotSiz << ", Recv=" << recv_byte_size << std::endl;
      }
    }else{
      preSiz=(int)*In_CurPos;
      In_CurPos+=(preSiz/4);
      recv_byte_size=(int)*In_CurPos;
      if (In_RemainSiz<recv_byte_size){
	std::cerr << "Data broken? Remain=" << In_RemainSiz << ", Size=" << recv_byte_size << std::endl;
	fatal_error_report(USER_DEFINED_ERROR1,"Data broken...");
      }
      In_RemainSiz-=recv_byte_size;
      if (m_debug) std::cerr << " reading: Remain=" << In_RemainSiz << ", Recv=" << recv_byte_size << std::endl;
    }

//    if (m_debug) {
//        std::cerr << "m_in_data.data.length():" << recv_byte_size
//                  << std::endl;
//    }

    return recv_byte_size;
