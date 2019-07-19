  struct timespec ts;
  double t0=0.,t1;

  unsigned char header[8];
  unsigned char footer[8];

  unsigned long compsize;
  unsigned int outsize, bufsize;

  bool InCompress;

  if (m_debug) {
    clock_gettime(CLOCK_MONOTONIC,&ts);
    t0=(ts.tv_sec*1.)+(ts.tv_nsec/1000000000.);
  }

  if (m_debug)
    std::cerr << "in set_data: data_byte_size=" << data_byte_size << std::endl;

  m_out_data.data.length((unsigned int)data_byte_size+HEADER_BYTE_SIZE + FOOTER_BYTE_SIZE);

  if ((InCompress=(*In_CurPos&0xf0000000)>0)){ //Compressed data coming
    std::cerr << "Compressed data is coming..." << std::endl;
    memcpy(&(m_out_data.data[HEADER_BYTE_SIZE]), (unsigned char *)In_CurPos, (size_t)data_byte_size);
    outsize=(unsigned int)data_byte_size;
  }else if(OutCompress==1){ //ZLIB
    compsize=data_byte_size;
    if (compress2(&(m_out_data.data[HEADER_BYTE_SIZE+8]),&compsize,
		  (unsigned char *)In_CurPos,(unsigned long)data_byte_size,
		  CompressLevel)!=Z_OK){
      std::cerr << "Error in compress2()" << std::endl;
      fatal_error_report(OUTPORT_ERROR);
    }
    if (m_debug) printf("Compressed: size=%d -> %lu\n",data_byte_size,compsize);
    outsize=(unsigned int)(compsize+8);
    bufsize=outsize|0xf0000000;
    memcpy(&(m_out_data.data[HEADER_BYTE_SIZE]), &bufsize, 4); // 4+4+compressed size
    memcpy(&(m_out_data.data[HEADER_BYTE_SIZE+4]), &data_byte_size, 4); //original size
  }else if (OutCompress==2){ //LZ4
    if ((compsize=LZ4_compress_fast((char *)In_CurPos, (char *)&(m_out_data.data[HEADER_BYTE_SIZE+8]),
				       data_byte_size,data_byte_size,CompressLevel))==0){
      std::cerr << "Error in LZ4_compress_default()" << std::endl;
      fatal_error_report(OUTPORT_ERROR);
    }
    if (m_debug) printf("Compressed: size=%d -> %lu\n",data_byte_size,compsize);
    outsize=(unsigned int)(compsize+8);
    bufsize=outsize|0xe0000000;
    memcpy(&(m_out_data.data[HEADER_BYTE_SIZE]), &bufsize, 4); // 4+4+compressed size
    memcpy(&(m_out_data.data[HEADER_BYTE_SIZE+4]), &data_byte_size, 4); //original size
  }else{
    memcpy(&(m_out_data.data[HEADER_BYTE_SIZE]), (unsigned char *)In_CurPos, (size_t)data_byte_size);
    outsize=(unsigned int)data_byte_size;
  }


  set_header(&header[0], outsize);
  set_footer(&footer[0]);

  ///set OutPort buffer length
  memcpy(&(m_out_data.data[0]), &header[0], HEADER_BYTE_SIZE);
//  memcpy(&(m_out_data.data[HEADER_BYTE_SIZE]), (unsigned char *)In_CurPos, (size_t)data_byte_size);
  memcpy(&(m_out_data.data[HEADER_BYTE_SIZE + outsize]), &footer[0],FOOTER_BYTE_SIZE);

  m_out_data.data.length(outsize + HEADER_BYTE_SIZE + FOOTER_BYTE_SIZE);

  if (m_debug) {
    clock_gettime(CLOCK_MONOTONIC,&ts);
    t1=(ts.tv_sec*1.)+(ts.tv_nsec/1000000000.);
    std::cout << "setdata time: " << std::fixed << std::setprecision(9) << t1-t0 << std::endl;
  }

  return 0;
