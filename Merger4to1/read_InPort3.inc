    int recv_byte_size = 0;
    bool ret;
    int GlobSiz;
    int preSiz;
    int *decompsize;
    int BufSiz;
    unsigned long compsize,origsize;
    int zret;
    unsigned int flag;

//    std::cerr << "Entering read_InPort3: Tot=" << In3_TotSiz << ", Remain=" << In3_RemainSiz << std::endl;

    if (In3_RemainSiz==0){
      ret = m_InPort3.read();
      if (ret==false){ // false: TIMEOUT or FATAL
	m_in3_status=check_inPort_status(m_InPort3);
	if (m_in3_status==BUF_TIMEOUT){ // Buffer empty
          m_in3_timeout_counter++;
        }else if (m_in3_status==BUF_FATAL){
	  fatal_error_report(INPORT_ERROR);
	}
      }else{
        if (m_debug) std::cerr << "Get Data(3) Timeout: " << m_in3_timeout_counter << " -> 0" << std::endl;
        m_in3_timeout_counter = 0;
        m_in3_status = BUF_SUCCESS;
	GlobSiz=m_in3_data.data.length();
	if (m_debug) std::cerr << "(3)Received Size=" << GlobSiz << std::endl;
	memcpy(&BufSiz,&(m_in3_data.data[HEADER_BYTE_SIZE]),4); // size of 
	if ((flag=BufSiz&0xf0000000)>0){ // get compressed data
	  decompsize=(int *)&(m_in3_data.data[HEADER_BYTE_SIZE+4]); // original size
	  if (*decompsize>Cur_MaxDataSiz3){
	    DataPos3=renew_buf(DataPos3,(size_t)Cur_MaxDataSiz3,(size_t)*decompsize);
	    Cur_MaxDataSiz3=*decompsize;
	  }
	  origsize=(unsigned long)*decompsize;
	  compsize=(unsigned long)(GlobSiz-HEADER_BYTE_SIZE-FOOTER_BYTE_SIZE-8);
	  if (m_debug) std::cerr << "(3)uncompressing: datasize=" << compsize << ", bufsize=" << origsize << std::endl;
	  if (flag==0xf0000000){
	    if ((zret=uncompress(DataPos3,&origsize,&(m_in3_data.data[HEADER_BYTE_SIZE+8]),compsize))!=Z_OK){
	      std::cerr << "(3)Failed in uncompress(ZLIB): " << zret << std::endl;
	      fatal_error_report(INPORT_ERROR);
	    }
	    In3_TotSiz=(int)origsize;
	  }else{
	    if ((zret=LZ4_decompress_safe((char *)&(m_in3_data.data[HEADER_BYTE_SIZE+8]),(char *)DataPos3,
					  (int)compsize,(int)origsize))<0){
	      std::cerr << "(3)Failed in uncompress(LZ4): " << zret << std::endl;
	      fatal_error_report(INPORT_ERROR);
	    }
	    In3_TotSiz=zret;
	  }
	  if (*decompsize!=In3_TotSiz){
	    std::cerr << "(3)Failed in uncompress, size mismatch..." << std::endl;
	    fatal_error_report(INPORT_ERROR);
	  }
	  if (m_debug) std::cerr << " uncomopress(3): " << compsize << "-> " << In3_TotSiz << std::endl;
	  In3_CurPos=(unsigned int *)DataPos3;
	}else{
	  In3_TotSiz=GlobSiz-HEADER_BYTE_SIZE-FOOTER_BYTE_SIZE;
	  In3_CurPos=(unsigned int *)&(m_in3_data.data[HEADER_BYTE_SIZE]);
	}
        recv_byte_size=(int)*In3_CurPos;
	In3_RemainSiz=In3_TotSiz-recv_byte_size;
	if (m_debug)
	  std::cerr << " reading(3): Tot=" << In3_TotSiz << ", Remain=" << In3_RemainSiz << ", Recv=" << recv_byte_size << std::endl;
      }
    }else{
      preSiz=(int)*In3_CurPos;
      In3_CurPos+=(preSiz/4);
      recv_byte_size=(int)*In3_CurPos;
      if (In3_RemainSiz<recv_byte_size){
	std::cerr << "Data(3) broken? Remain=" << In3_RemainSiz << ", Size=" << recv_byte_size << std::endl;
	fatal_error_report(USER_DEFINED_ERROR1,"Data broken...");
      }
      In3_RemainSiz-=recv_byte_size;
      if (m_debug) std::cerr << " reading(3): Remain=" << In3_RemainSiz << ", Recv=" << recv_byte_size << std::endl;
    }

    return recv_byte_size;
