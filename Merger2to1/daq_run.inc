  struct timespec ts;
  double t0;

    if (m_debug) {
        std::cerr << "*** Merger2to1::run" << std::endl;
    }

    if (check_trans_lock()){ // STOP comming
      if (m_out_status==BUF_TIMEOUT || m_out_status==BUF_NOBUF){ // if last write was fail
	if (write_OutPort()==0){ // try again
	  inc_total_data_size(Stock_Offset);
	  Stock_CurNum=0; Stock_Offset=0;
	}else{ // if fail, transit to CONFIGURE state.
	  set_trans_unlock();
	}
	return 0;
      }
      if (Stock_CurNum>0){ // if data exist in stock
	set_data(Stock_Offset); // make out data
	if (write_OutPort()==0){ // write data
	  inc_total_data_size(Stock_Offset);
	  Stock_CurNum=0; Stock_Offset=0;
	}else{ // if fail, transit to CONFIGURE state.
	  set_trans_unlock();
	  return 0;
	}
      }
      if (m_inport1_recv_data_size==0){
	if (In1_Done==0){ // try to read inport once
	  if (In1_RemainSiz==0) In1_Done=1;
	  m_inport1_recv_data_size = read_InPort1();
	}else{
	  if (In1_RemainSiz>0) m_inport1_recv_data_size = read_InPort1();
	}
      }
      if (m_inport2_recv_data_size==0){
	if (In2_Done==0){ // try to read inport once
	  if (In2_RemainSiz==0) In2_Done=1;
	  m_inport2_recv_data_size = read_InPort2();
	}else{
	  if (In2_RemainSiz>0) m_inport2_recv_data_size = read_InPort2();
	}
      }
      if ((m_inport1_recv_data_size > 0) && (m_inport2_recv_data_size > 0)){
	Stock_data(m_inport1_recv_data_size,m_inport2_recv_data_size);
	m_inport1_recv_data_size=0;
	m_inport2_recv_data_size=0;
      }else{
	set_trans_unlock();
	return 0;
      }
      if (Stock_CurNum>0){
	set_data(Stock_Offset);
	if (write_OutPort()==0){
	  inc_total_data_size(Stock_Offset);
	  Stock_CurNum=0; Stock_Offset=0;
	}else{
	  set_trans_unlock();
	  return 0;
	}
      }
      return 0;
    }

    In1_Done=0; In2_Done=0;

    if (m_out_status != BUF_TIMEOUT && m_out_status != BUF_NOBUF) {
      if (m_inport1_recv_data_size==0)
        m_inport1_recv_data_size = read_InPort1();
      if (m_inport2_recv_data_size==0)
        m_inport2_recv_data_size = read_InPort2();

      if ((m_inport1_recv_data_size > 0) && (m_inport2_recv_data_size > 0)){
	Stock_data(m_inport1_recv_data_size,m_inport2_recv_data_size);
	m_inport1_recv_data_size=0;
	m_inport2_recv_data_size=0;
      }
    }

    if (m_out_status == BUF_TIMEOUT || m_out_status == BUF_NOBUF){
      clock_gettime(CLOCK_MONOTONIC,&ts);
      t0=(ts.tv_sec*1.)+(ts.tv_nsec/1000000000.);
      std::cout << "-w>" << std::fixed << std::setprecision(9) << t0 << std::endl;
      if (write_OutPort()<0){
	;
      }else{
	inc_total_data_size(Stock_Offset);  // increase total data byte size
	Stock_CurNum=0;
	Stock_Offset=0;
      }
      clock_gettime(CLOCK_MONOTONIC,&ts);
      t0=(ts.tv_sec*1.)+(ts.tv_nsec/1000000000.);
      std::cout << "+w>" << std::fixed << std::setprecision(9) << t0 << std::endl;
    }

   if ( (Stock_CurNum==Stock_MaxNum) ||
	(Stock_CurNum>0 && (m_in1_timeout_counter>ReadTimeout||m_in2_timeout_counter>ReadTimeout)) ){
      clock_gettime(CLOCK_MONOTONIC,&ts);
      t0=(ts.tv_sec*1.)+(ts.tv_nsec/1000000000.);
      std::cout << "-w>" << std::fixed << std::setprecision(9) << t0 << std::endl;
      set_data(Stock_Offset);
      if (write_OutPort()<0){
	;
      }else{
	inc_total_data_size(Stock_Offset);  // increase total data byte size
	Stock_CurNum=0;
	Stock_Offset=0;
      }
      clock_gettime(CLOCK_MONOTONIC,&ts);
      t0=(ts.tv_sec*1.)+(ts.tv_nsec/1000000000.);
      std::cout << "+w>" << std::fixed << std::setprecision(9) << t0 << std::endl;
    }

   return 0;