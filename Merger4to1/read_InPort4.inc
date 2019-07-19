    int recv_byte_size = 0;
    bool ret;
    int GlobSiz;
    int preSiz;
    int *decompsize;
    int BufSiz;
    unsigned long compsize,origsize;
    int zret;
    unsigned int flag;

//    std::cerr << "Entering read_InPort4: Tot=" << In4_TotSiz << ", Remain=" << In4_RemainSiz << std::endl;

    if (In4_RemainSiz==0){
      ret = m_InPort4.read();
      if (ret==false){ // false: TIMEOUT or FATAL
	m_in4_status=check_inPort_status(m_InPort4);
	if (m_in4_status==BUF_TIMEOUT){ // Buffer empty
          m_in4_timeout_counter++;
        }else if (m_in4_status==BUF_FATAL){
	  fatal_error_report(INPORT_ERROR);
	}
      }else{
        if (m_debug) std::cerr << "Get Data(4) Timeout: " << m_in4_timeout_counter << " -> 0" << std::endl;
        m_in4_timeout_counter = 0;
        m_in4_status = BUF_SUCCESS;
	GlobSiz=m_in4_data.data.length();
	if (m_debug) std::cerr << "(4)Received Size=" << GlobSiz << std::endl;
	memcpy(&BufSiz,&(m_in4_data.data[HEADER_BYTE_SIZE]),4); // size of 
	if ((flag=BufSiz&0xf0000000)>0){ // get compressed data
	  decompsize=(int *)&(m_in4_data.data[HEADER_BYTE_SIZE+4]); // original size
	  if (*decompsize>Cur_MaxDataSiz4){
	    DataPos4=renew_buf(DataPos4,(size_t)Cur_MaxDataSiz4,(size_t)*decompsize);
	    Cur_MaxDataSiz4=*decompsize;
	  }
	  origsize=(unsigned long)*decompsize;
	  compsize=(unsigned long)(GlobSiz-HEADER_BYTE_SIZE-FOOTER_BYTE_SIZE-8);
	  if (m_debug) std::cerr << "(4)uncompressing: datasize=" << compsize << ", bufsize=" << origsize << std::endl;
	  if (flag==0xf0000000){
	    if ((zret=uncompress(DataPos4,&origsize,&(m_in4_data.data[HEADER_BYTE_SIZE+8]),compsize))!=Z_OK){
	      std::cerr << "(4)Failed in uncompress(ZLIB): " << zret << std::endl;
	      fatal_error_report(INPORT_ERROR);
	    }
	    In4_TotSiz=(int)origsize;
	  }else{
	    if ((zret=LZ4_decompress_safe((char *)&(m_in4_data.data[HEADER_BYTE_SIZE+8]),(char *)DataPos4,
					  (int)compsize,(int)origsize))<0){
	      std::cerr << "(4)Failed in uncompress(LZ4): " << zret << std::endl;
	      fatal_error_report(INPORT_ERROR);
	    }
	    In4_TotSiz=zret;
	  }
	  if (*decompsize!=In4_TotSiz){
	    std::cerr << "(4)Failed in uncompress, size mismatch..." << std::endl;
	    fatal_error_report(INPORT_ERROR);
	  }
	  if (m_debug) std::cerr << " uncomopress(4): " << compsize << "-> " << In4_TotSiz << std::endl;
	  In4_CurPos=(unsigned int *)DataPos4;
	}else{
	  In4_TotSiz=GlobSiz-HEADER_BYTE_SIZE-FOOTER_BYTE_SIZE;
	  In4_CurPos=(unsigned int *)&(m_in4_data.data[HEADER_BYTE_SIZE]);
	}
        recv_byte_size=(int)*In4_CurPos;
	In4_RemainSiz=In4_TotSiz-recv_byte_size;
	if (m_debug)
	  std::cerr << " reading(4): Tot=" << In4_TotSiz << ", Remain=" << In4_RemainSiz << ", Recv=" << recv_byte_size << std::endl;
      }
    }else{
      preSiz=(int)*In4_CurPos;
      In4_CurPos+=(preSiz/4);
      recv_byte_size=(int)*In4_CurPos;
      if (In4_RemainSiz<recv_byte_size){
	std::cerr << "Data(4) broken? Remain=" << In4_RemainSiz << ", Size=" << recv_byte_size << std::endl;
	fatal_error_report(USER_DEFINED_ERROR1,"Data broken...");
      }
      In4_RemainSiz-=recv_byte_size;
      if (m_debug) std::cerr << " reading(4): Remain=" << In4_RemainSiz << ", Recv=" << recv_byte_size << std::endl;
    }

    return recv_byte_size;
