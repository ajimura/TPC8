    int recv_byte_size = 0;
    bool ret;
    int GlobSiz;
    int preSiz;
    int *decompsize;
    int BufSiz;
    unsigned long compsize,origsize;
    int zret;
    unsigned int flag;

//    std::cerr << "Entering read_InPort1: Tot=" << In1_TotSiz << ", Remain=" << In1_RemainSiz << std::endl;

    if (In1_RemainSiz==0){
      ret = m_InPort1.read();
      if (ret==false){ // false: TIMEOUT or FATAL
	m_in1_status=check_inPort_status(m_InPort1);
	if (m_in1_status==BUF_TIMEOUT){ // Buffer empty
          m_in1_timeout_counter++;
        }else if (m_in1_status==BUF_FATAL){
	  fatal_error_report(INPORT_ERROR);
	}
      }else{
        if (m_debug) std::cerr << "Get Data(1) Timeout: " << m_in1_timeout_counter << " -> 0" << std::endl;
        m_in1_timeout_counter = 0;
        m_in1_status = BUF_SUCCESS;
	GlobSiz=m_in1_data.data.length();
	if (m_debug) std::cerr << "(1)Received Size=" << GlobSiz << std::endl;
	memcpy(&BufSiz,&(m_in1_data.data[HEADER_BYTE_SIZE]),4); // size of 
	if ((flag=BufSiz&0xf0000000)>0){ // get compressed data
	  decompsize=(int *)&(m_in1_data.data[HEADER_BYTE_SIZE+4]); // original size
	  if (*decompsize>Cur_MaxDataSiz1){
	    DataPos1=renew_buf(DataPos1,(size_t)Cur_MaxDataSiz1,(size_t)*decompsize);
	    Cur_MaxDataSiz1=*decompsize;
	  }
	  origsize=(unsigned long)*decompsize;
	  compsize=(unsigned long)(GlobSiz-HEADER_BYTE_SIZE-FOOTER_BYTE_SIZE-8);
	  if (m_debug) std::cerr << "(1)uncompressing: datasize=" << compsize << ", bufsize=" << origsize << std::endl;
	  if (flag==0xf0000000){
	    if ((zret=uncompress(DataPos1,&origsize,&(m_in1_data.data[HEADER_BYTE_SIZE+8]),compsize))!=Z_OK){
	      std::cerr << "(1)Failed in uncompress(ZLIB): " << zret << std::endl;
	      fatal_error_report(INPORT_ERROR);
	    }
	    In1_TotSiz=(int)origsize;
	  }else{
	    if ((zret=LZ4_decompress_safe((char *)&(m_in1_data.data[HEADER_BYTE_SIZE+8]),(char *)DataPos1,
					  (int)compsize,(int)origsize))<0){
	      std::cerr << "(1)Failed in uncompress(LZ4): " << zret << std::endl;
	      fatal_error_report(INPORT_ERROR);
	    }
	    In1_TotSiz=zret;
	  }
	  if (*decompsize!=In1_TotSiz){
	    std::cerr << "(1)Failed in uncompress, size mismatch..." << std::endl;
	    fatal_error_report(INPORT_ERROR);
	  }
	  if (m_debug) std::cerr << " uncomopress(1): " << compsize << "-> " << In1_TotSiz << std::endl;
	  In1_CurPos=(unsigned int *)DataPos1;
	}else{
	  In1_TotSiz=GlobSiz-HEADER_BYTE_SIZE-FOOTER_BYTE_SIZE;
	  In1_CurPos=(unsigned int *)&(m_in1_data.data[HEADER_BYTE_SIZE]);
	}
        recv_byte_size=(int)*In1_CurPos;
	In1_RemainSiz=In1_TotSiz-recv_byte_size;
	if (m_debug)
	  std::cerr << " reading(1): Tot=" << In1_TotSiz << ", Remain=" << In1_RemainSiz << ", Recv=" << recv_byte_size << std::endl;
      }
    }else{
      preSiz=(int)*In1_CurPos;
      In1_CurPos+=(preSiz/4);
      recv_byte_size=(int)*In1_CurPos;
      if (In1_RemainSiz<recv_byte_size){
	std::cerr << "Data(1) broken? Remain=" << In1_RemainSiz << ", Size=" << recv_byte_size << std::endl;
	fatal_error_report(USER_DEFINED_ERROR1,"Data broken...");
      }
      In1_RemainSiz-=recv_byte_size;
      if (m_debug) std::cerr << " reading(1): Remain=" << In1_RemainSiz << ", Recv=" << recv_byte_size << std::endl;
    }

    return recv_byte_size;
