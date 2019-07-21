    int recv_byte_size = 0;
    int *decompsize;
    bool ret;
    int GlobSiz;
    int BufSiz;
    int preSiz;
    unsigned long compsize,origsize;
    int zret;
    unsigned int flag;

    //    std::cerr << "Entering read_InPort: Tot=" << In_TotSiz << ", Remain=" << In_RemainSiz << std::endl;

    if (In_RemainSiz==0){
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
	GlobSiz=m_in_data.data.length(); // size of DAQ-MW packet
	if (m_debug) std::cerr << "Received Size=" << GlobSiz << std::endl;
	memcpy(&BufSiz,&(m_in_data.data[HEADER_BYTE_SIZE]),4); // size of 
	if ((flag=BufSiz&0xf0000000)>0){ // get compressed data
	  decompsize=(int *)&(m_in_data.data[HEADER_BYTE_SIZE+4]); // original size
	  if (*decompsize>Cur_MaxDataSiz){
	    DataPos1=renew_buf(DataPos1,(size_t)Cur_MaxDataSiz,(size_t)*decompsize);
	    Cur_MaxDataSiz=*decompsize;
	  }
	  origsize=(unsigned long)*decompsize;
	  compsize=(unsigned long)(GlobSiz-HEADER_BYTE_SIZE-FOOTER_BYTE_SIZE-8);
	  if (m_debug) std::cerr << "uncompressing: datasize=" << compsize << ", bufsize=" << origsize << std::endl;
	  if (flag==0xf0000000){
	    if ((zret=uncompress(DataPos1,&origsize,&(m_in_data.data[HEADER_BYTE_SIZE+8]),compsize))!=Z_OK){
	      std::cerr << "Failed in uncompress(ZLIB): " << zret << std::endl;
	      fatal_error_report(INPORT_ERROR);
	    }
	    In_TotSiz=(int)origsize;
	  }else if (flag==0xe0000000){
	    if ((zret=LZ4_decompress_safe((char *)&(m_in_data.data[HEADER_BYTE_SIZE+8]),(char *)DataPos1,
					  (int)compsize,(int)origsize))<0){
	      std::cerr << "Failed in uncompress(LZ4): " << zret << std::endl;
	      fatal_error_report(INPORT_ERROR);
	    }
	    In_TotSiz=zret;
	  }else{
	    if ((zret=ZSTD_decompress(DataPos1,(size_t)origsize,
				      &(m_in_data.data[HEADER_BYTE_SIZE+8]),(size_t)compsize
				      ))<=0){
	      std::cerr << "Failed in uncompress(ZSTD): " << zret << std::endl;
	      fatal_error_report(INPORT_ERROR);
	    }
	    In_TotSiz=zret;
	  }
	  if (*decompsize!=In_TotSiz){
	    std::cerr << "Failed in uncompress, size mismatch..." << std::endl;
	    fatal_error_report(INPORT_ERROR);
	  }
	  if (m_debug) std::cerr << " uncomopress: " << compsize << "-> " << In_TotSiz << std::endl;
	  In_CurPos=(unsigned int *)DataPos1;
	}else{
	  In_TotSiz=GlobSiz-HEADER_BYTE_SIZE-FOOTER_BYTE_SIZE;
	  In_CurPos=(unsigned int *)&(m_in_data.data[HEADER_BYTE_SIZE]);
	}
	recv_byte_size=(int)*In_CurPos;
	In_RemainSiz=In_TotSiz-recv_byte_size;
	if (m_debug)
	  std::cerr << " reading: Tot=" << In_TotSiz << ", Remain=" << In_RemainSiz << ", Recv=" << recv_byte_size << std::endl;
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
      if (m_debug)
	std::cerr << " reading: Remain=" << In_RemainSiz << ", Recv=" << recv_byte_size << std::endl;
    }

    return recv_byte_size;
