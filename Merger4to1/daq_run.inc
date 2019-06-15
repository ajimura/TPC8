  bool flag_timeout;
  struct timespec ts;
  double t0;

  //  unsigned int event_data_size;
    if (m_debug) {
        std::cerr << "*** Merger4to1::run" << std::endl;
    }

    if (check_trans_lock()){
      if (m_out_status==BUF_TIMEOUT || m_out_status==BUF_NOBUF){
	if (write_OutPort()==0){
	  inc_total_data_size((unsigned int)Stock_Offset);  // increase total data byte size
	  Stock_CurNum=0; Stock_Offset=0;
	}else{
	  set_trans_unlock();       // Transit to CONFIGURE state.
	}
	return 0;
      }
      if (Stock_CurNum>0){
	set_data(Stock_Offset);
	if (write_OutPort()==0){
	  inc_total_data_size((unsigned int)Stock_Offset);  // increase total data byte size
	  Stock_CurNum=0; Stock_Offset=0;
	}else{
	  set_trans_unlock();       // Transit to CONFIGURE state.
	  return 0;
	}
      }
      if (m_inport1_recv_data_size==0){
	if (In1_Done==0){
	  if (In1_RemainSiz==0) In1_Done=1;
	  m_inport1_recv_data_size = read_InPort1();
	}else{
	  if (In1_RemainSiz>0) m_inport1_recv_data_size = read_InPort1();
	}
      }
      if (m_inport2_recv_data_size==0){
	if (In2_Done==0){
	  if (In2_RemainSiz==0) In2_Done=1;
	  m_inport2_recv_data_size = read_InPort2();
	}else{
	  if (In2_RemainSiz>0) m_inport2_recv_data_size = read_InPort2();
	}
      }
      if (m_inport3_recv_data_size==0){
	if (In3_Done==0){
	  if (In3_RemainSiz==0) In3_Done=1;
	  m_inport3_recv_data_size = read_InPort3();
	}else{
	  if (In3_RemainSiz>0) m_inport3_recv_data_size = read_InPort3();
	}
      }
      if (m_inport4_recv_data_size==0){
	if (In4_Done==0){
	  if (In4_RemainSiz==0) In4_Done=1;
	  m_inport4_recv_data_size = read_InPort4();
	}else{
	  if (In4_RemainSiz>0) m_inport4_recv_data_size = read_InPort4();
	}
      }
      if ((m_inport1_recv_data_size > 0) &&
	  (m_inport2_recv_data_size > 0) &&
	  (m_inport3_recv_data_size > 0) &&
	  (m_inport4_recv_data_size > 0)    ){
	Stock_data(m_inport1_recv_data_size,m_inport2_recv_data_size,
		   m_inport3_recv_data_size,m_inport4_recv_data_size);
	m_inport1_recv_data_size=0;
	m_inport2_recv_data_size=0;
	m_inport3_recv_data_size=0;
	m_inport4_recv_data_size=0;
      }else{
	set_trans_unlock();       // Transit to CONFIGURE state.
	return 0;
      }
      if (Stock_CurNum>0){
	set_data(Stock_Offset);
	if (write_OutPort()==0){
	  inc_total_data_size((unsigned int)Stock_Offset);  // increase total data byte size
	  Stock_CurNum=0; Stock_Offset=0;
	}else{
	  set_trans_unlock();       // Transit to CONFIGURE state.
	  return 0;
	}
      }
      return 0;
    }

    In1_Done=0; In2_Done=0; In3_Done=0; In4_Done=0;

    if (m_out_status != BUF_TIMEOUT && m_out_status != BUF_NOBUF) {
      if (m_inport1_recv_data_size==0)
	m_inport1_recv_data_size = read_InPort1();
      if (m_inport2_recv_data_size==0)
        m_inport2_recv_data_size = read_InPort2();
      if (m_inport3_recv_data_size==0)
        m_inport3_recv_data_size = read_InPort3();
      if (m_inport4_recv_data_size==0)
        m_inport4_recv_data_size = read_InPort4();

      if ((m_inport1_recv_data_size > 0) &&
	  (m_inport2_recv_data_size > 0) &&
	  (m_inport3_recv_data_size > 0) &&
	  (m_inport4_recv_data_size > 0)    ){
	Stock_data(m_inport1_recv_data_size,m_inport2_recv_data_size,
		   m_inport3_recv_data_size,m_inport4_recv_data_size);
	m_inport1_recv_data_size=0;
	m_inport2_recv_data_size=0;
	m_inport3_recv_data_size=0;
	m_inport4_recv_data_size=0;
      }
    }

    if (m_out_status == BUF_TIMEOUT || m_out_status == BUF_NOBUF){
      clock_gettime(CLOCK_MONOTONIC,&ts);
      t0=(ts.tv_sec*1.)+(ts.tv_nsec/1000000000.);
      std::cout << "-w>" << std::fixed << std::setprecision(9) << t0 << std::endl;
      if (write_OutPort()<0){
	;
      }else{
	inc_total_data_size((unsigned int)Stock_Offset);  // increase total data byte size
	Stock_CurNum=0;
	Stock_Offset=0;
      }
      clock_gettime(CLOCK_MONOTONIC,&ts);
      t0=(ts.tv_sec*1.)+(ts.tv_nsec/1000000000.);
      std::cout << "+w>" << std::fixed << std::setprecision(9) << t0 << std::endl;
    }

    flag_timeout=( m_in1_timeout_counter>ReadTimeout ||
		   m_in2_timeout_counter>ReadTimeout ||
		   m_in3_timeout_counter>ReadTimeout ||
		   m_in4_timeout_counter>ReadTimeout ); 
    if ( (Stock_CurNum==Stock_MaxNum) || (Stock_CurNum>0 && flag_timeout) ){
      clock_gettime(CLOCK_MONOTONIC,&ts);
      t0=(ts.tv_sec*1.)+(ts.tv_nsec/1000000000.);
      std::cout << "-w>" << std::fixed << std::setprecision(9) << t0 << std::endl;
      set_data(Stock_Offset);
      if (write_OutPort()<0){
	;
      }else{
	inc_total_data_size((unsigned int)Stock_Offset);  // increase total data byte size
	Stock_CurNum=0;
	Stock_Offset=0;
      }
      clock_gettime(CLOCK_MONOTONIC,&ts);
      t0=(ts.tv_sec*1.)+(ts.tv_nsec/1000000000.);
      std::cout << "+w>" << std::fixed << std::setprecision(9) << t0 << std::endl;
    }

   return 0;
