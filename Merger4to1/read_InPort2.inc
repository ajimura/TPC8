    int recv_byte_size = 0;
    bool ret;
    int GlobSiz;
    int preSiz;
    int *decompsize;
    int BufSiz;
    unsigned long compsize,origsize;
    int zret;
    unsigned int flag;

//    std::cerr << "Entering read_InPort2: Tot=" << In2_TotSiz << ", Remain=" << In2_RemainSiz << std::endl;

    if (In2_RemainSiz==0){
      ret = m_InPort2.read();
      if (ret==false){ // false: TIMEOUT or FATAL
	m_in2_status=check_inPort_status(m_InPort2);
	if (m_in2_status==BUF_TIMEOUT){ // Buffer empty
          m_in2_timeout_counter++;
        }else if (m_in2_status==BUF_FATAL){
	  fatal_error_report(INPORT_ERROR);
	}
      }else{
        if (m_debug) std::cerr << "Get Data(2) Timeout: " << m_in2_timeout_counter << " -> 0" << std::endl;
        m_in2_timeout_counter = 0;
        m_in2_status = BUF_SUCCESS;
	GlobSiz=m_in2_data.data.length();
	if (m_debug) std::cerr << "(2)Received Size=" << GlobSiz << std::endl;
	memcpy(&BufSiz,&(m_in2_data.data[HEADER_BYTE_SIZE]),4); // size of 
	if ((flag=BufSiz&0xf0000000)>0){ // get compressed data
	  decompsize=(int *)&(m_in2_data.data[HEADER_BYTE_SIZE+4]); // original size
	  if (*decompsize>Cur_MaxDataSiz2){
	    DataPos2=renew_buf(DataPos2,(size_t)Cur_MaxDataSiz2,(size_t)*decompsize);
	    Cur_MaxDataSiz2=*decompsize;
	  }
	  origsize=(unsigned long)*decompsize;
	  compsize=(unsigned long)(GlobSiz-HEADER_BYTE_SIZE-FOOTER_BYTE_SIZE-8);
	  if (m_debug) std::cerr << "(2)uncompressing: datasize=" << compsize << ", bufsize=" << origsize << std::endl;
	  if (flag==0xf0000000){
	    if ((zret=uncompress(DataPos2,&origsize,&(m_in2_data.data[HEADER_BYTE_SIZE+8]),compsize))!=Z_OK){
	      std::cerr << "(2)Failed in uncompress(ZLIB): " << zret << std::endl;
	      fatal_error_report(INPORT_ERROR);
	    }
	    In2_TotSiz=(int)origsize;
	  }else{
	    if ((zret=LZ4_decompress_safe((char *)&(m_in2_data.data[HEADER_BYTE_SIZE+8]),(char *)DataPos2,
					  (int)compsize,(int)origsize))<0){
	      std::cerr << "(2)Failed in uncompress(LZ4): " << zret << std::endl;
	      fatal_error_report(INPORT_ERROR);
	    }
	    In2_TotSiz=zret;
	  }
	  if (*decompsize!=In2_TotSiz){
	    std::cerr << "(2)Failed in uncompress, size mismatch..." << std::endl;
	    fatal_error_report(INPORT_ERROR);
	  }
	  if (m_debug) std::cerr << " uncomopress(2): " << compsize << "-> " << In2_TotSiz << std::endl;
	  In2_CurPos=(unsigned int *)DataPos2;
	}else{
	  In2_TotSiz=GlobSiz-HEADER_BYTE_SIZE-FOOTER_BYTE_SIZE;
	  In2_CurPos=(unsigned int *)&(m_in2_data.data[HEADER_BYTE_SIZE]);
	}
        recv_byte_size=(int)*In2_CurPos;
	In2_RemainSiz=In2_TotSiz-recv_byte_size;
	if (m_debug)
	  std::cerr << " reading(2): Tot=" << In2_TotSiz << ", Remain=" << In2_RemainSiz << ", Recv=" << recv_byte_size << std::endl;
      }
    }else{
      preSiz=(int)*In2_CurPos;
      In2_CurPos+=(preSiz/4);
      recv_byte_size=(int)*In2_CurPos;
      if (In2_RemainSiz<recv_byte_size){
	std::cerr << "Data(2) broken? Remain=" << In2_RemainSiz << ", Size=" << recv_byte_size << std::endl;
	fatal_error_report(USER_DEFINED_ERROR1,"Data broken...");
      }
      In2_RemainSiz-=recv_byte_size;
      if (m_debug) std::cerr << " reading(2): Remain=" << In2_RemainSiz << ", Recv=" << recv_byte_size << std::endl;
    }

    return recv_byte_size;
