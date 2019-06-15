    int OverFlow;

    if (check_trans_lock()){					// STOP comming
      if (m_out_status==BUF_TIMEOUT || m_out_status==BUF_NOBUF){	// if last write was fail
	if (write_OutPort()==0){					// try again
	  inc_total_data_size((unsigned int)Stock_Offset);
	  Stock_CurNum=0; Stock_Offset=0;
	}else{								// if fail, transit to CONFIGURE state.
	  set_trans_unlock();
	}
	return 0;
      }
      if (Stock_CurNum>0){						// if data exist in stock
	set_data(Stock_Offset);						// make out data
	if (write_OutPort()==0){					// write data
	  inc_total_data_size((unsigned int)Stock_Offset);
	  Stock_CurNum=0; Stock_Offset=0;
	}else{								// if fail, transit to CONFIGURE state.
	  set_trans_unlock();
	  return 0;
	}
      }
      if (m_inport1_recv_data_size==0){					// if there's no data on inport1
	if (In1_Done==0){						// try to read inport once
	  if (In1_RemainSiz==0) In1_Done=1;
	  m_inport1_recv_data_size = read_InPort1();
	}else{
	  if (In1_RemainSiz>0) m_inport1_recv_data_size = read_InPort1();
	}
      }
      if (m_inport2_recv_data_size==0){					// if there's no data on inport2
	if (In2_Done==0){						// try to read inport once
	  if (In2_RemainSiz==0) In2_Done=1;
	  m_inport2_recv_data_size = read_InPort2();
	}else{
	  if (In2_RemainSiz>0) m_inport2_recv_data_size = read_InPort2();
	}
      }
      if ((m_inport1_recv_data_size > 0) && (m_inport2_recv_data_size > 0)){ // if data on inport1 & 2 both
	Stock_data(m_inport1_recv_data_size,m_inport2_recv_data_size);	// store data into stock
	m_inport1_recv_data_size=0;
	m_inport2_recv_data_size=0;
      }else{
	set_trans_unlock();
	return 0;
      }
      if (Stock_CurNum>0){						// still there's data
	set_data(Stock_Offset);
	if (write_OutPort()==0){
	  inc_total_data_size((unsigned int)Stock_Offset);
	  Stock_CurNum=0; Stock_Offset=0;
	}else{
	  set_trans_unlock();
	  return 0;
	}
      }
      return 0;
    }

    In1_Done=0; In2_Done=0;
    OverFlow=0;

    if (m_out_status != BUF_TIMEOUT && m_out_status != BUF_NOBUF) {
      if (m_inport1_recv_data_size==0)
        m_inport1_recv_data_size = read_InPort1();
      if (m_inport2_recv_data_size==0)
        m_inport2_recv_data_size = read_InPort2();

      if ((m_inport1_recv_data_size > 0) && (m_inport2_recv_data_size > 0)){
        if ((Stock_Offset+m_inport1_recv_data_size+m_inport2_recv_data_size+80)>Stock_MaxSiz){
	  OverFlow=1;
        }else{
          OverFlow=0;
          Stock_data(m_inport1_recv_data_size,m_inport2_recv_data_size);
          m_inport1_recv_data_size=0;
          m_inport2_recv_data_size=0;
        }
      }
    }

    if (m_out_status == BUF_TIMEOUT || m_out_status == BUF_NOBUF){
      if (write_OutPort()<0){
	;
      }else{
	inc_total_data_size((unsigned int)Stock_Offset);  // increase total data byte size
	Stock_CurNum=0;
	Stock_Offset=0;
      }
    }

   if ( (Stock_CurNum==Stock_MaxNum) ||
	(Stock_CurNum>0 && (m_in1_timeout_counter>ReadTimeout||m_in2_timeout_counter>ReadTimeout)) ||
        (OverFlow==1) ){
      set_data(Stock_Offset);
      if (write_OutPort()<0){
	;
      }else{
	inc_total_data_size((unsigned int)Stock_Offset);  // increase total data byte size
	Stock_CurNum=0;
	Stock_Offset=0;
      }
    }

   return 0;
