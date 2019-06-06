    if (check_trans_lock()) {  // check if stop command has come
      std::cout << "Stop command has come. Now Stock_CurNum=" << Stock_CurNum << std::endl;
      if (Stock_CurNum>0){
	if (m_out_status!=BUF_TIMEOUT && m_out_status!=BUF_NOBUF) set_data(Stock_Offset);
	if (write_OutPort()<0){
	  ;
	}else{
	  inc_total_data_size(Stock_Offset);  // increase total data byte size
	  Stock_CurNum=0;
	  Stock_Offset=0;
	}
      }
      set_trans_unlock();    // transit to CONFIGURED state
      return 0;
    }

    if (m_out_status == BUF_SUCCESS) {   // previous OutPort.write() successfully done
        m_recv_byte_size = read_data_from_detectors();
    }

    if (m_out_status == BUF_TIMEOUT || m_out_status == BUF_NOBUF){
      if (write_OutPort()<0){
	;
      }else{
	inc_total_data_size(Stock_Offset);  // increase total data byte size
	Stock_CurNum=0;
	Stock_Offset=0;
      }
    }

    if ( (Stock_CurNum==Stock_MaxNum) || (Stock_CurNum>0 && m_recv_timeout_counter>ReadTimeout) ){
      set_data(Stock_Offset);
      if (write_OutPort()<0){
	;
      }else{
	inc_total_data_size(Stock_Offset);  // increase total data byte size
	Stock_CurNum=0;
	Stock_Offset=0;
      }
    }

    return 0;
