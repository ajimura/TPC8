  int recv_byte_size = 0;
  bool ret;
  int GlobSiz;

//    std::cerr << "Entering read_InPort: Tot=" << In_TotSiz << ", Remain=" << In_RemainSiz << std::endl;

  ret = m_InPort.read();
  if (ret==false){ // false: TIMEOUT or FATAL
    m_in_status=check_inPort_status(m_InPort);
    if (m_in_status==BUF_TIMEOUT){ // Buffer empty
      m_in_timeout_counter++;
    }else if (m_in_status==BUF_FATAL){
      fatal_error_report(INPORT_ERROR);
    }
  }else{
    if (m_debug) std::cerr << "Get Data. Timeout: " << m_in_timeout_counter << " -> 0" << std::endl;
    m_in_timeout_counter = 0;
    m_in_status = BUF_SUCCESS;
    In_CurPos=(unsigned int *)&(m_in_data.data[HEADER_BYTE_SIZE]);
    GlobSiz=m_in_data.data.length();
    if (m_debug) std::cerr << "Received Size=" << GlobSiz << std::endl;
    recv_byte_size=GlobSiz-HEADER_BYTE_SIZE-FOOTER_BYTE_SIZE;
//    In_TotSiz=(int)*In_CurPos;
    In_TotSiz=recv_byte_size;
    if (m_debug) std::cerr << " reading: Recv=" << recv_byte_size << std::endl;
  }

  return recv_byte_size;
