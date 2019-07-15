    if (check_trans_lock()){					// STOP comming
      if (m_out_status==BUF_TIMEOUT || m_out_status==BUF_NOBUF){	// if last write was fail
	if (write_OutPort()==0){					// try again
	  inc_total_data_size((unsigned int)(In_TotSiz&0x0fffffff));
	}else{								// if fail, transit to CONFIGURE state.
	  set_trans_unlock();
	}
	return 0;
      }
      if (m_inport_recv_data_size==0){					// if there's no data on inport1
	if (In_Done==0) In_Done=1;					// try to read inport once
        m_inport_recv_data_size = read_InPort();
      }
      if ((m_inport_recv_data_size > 0)){				// if data on inport
	set_data(m_inport_recv_datea_size);
	m_inport_recv_data_size=0;
	if (write_OutPort()==0){
	  inc_total_data_size((unsigned int)(In_TotSiz&0x0fffffff));
	}else{
	  set_trans_unlock();
	  return 0;
	}
      }else{
	set_trans_unlock();
	return 0;
      }
      return 0;
    }

    In_Done=0;

    if (m_out_status != BUF_TIMEOUT && m_out_status != BUF_NOBUF) {
      if (m_inport_recv_data_size==0)
        m_inport_recv_data_size = read_InPort();

      if ((m_inport_recv_data_size > 0)){
	set_data(m_inport_recv_data_size);
	m_inport_recv_data_size=0;
	if (write_OutPort()==0){
	  inc_total_data_size((unsigned int)(In_TotSiz&0x0fffffff));
	}
      }
    }

    if (m_out_status == BUF_TIMEOUT || m_out_status == BUF_NOBUF){
      if (write_OutPort()<0){
	;
      }else{
	inc_total_data_size((unsigned int)(In_TotSiz&0x0fffffff));  // increase total data byte size
      }
    }

   return 0;
