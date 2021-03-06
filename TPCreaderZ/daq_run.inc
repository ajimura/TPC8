//    if (m_debug) std::cerr << "*** TPCreader::run" << std::endl;

    if (check_trans_lock()) {  // check if stop command has come
      std::cout << "Stop command has come. Now Stock_CurNum=" << Stock_CurNum << std::endl;
      if (Stock_CurNum>0){
        if (m_out_status!=BUF_TIMEOUT && m_out_status!=BUF_NOBUF) set_data((unsigned int)Stock_Offset);
        if (write_OutPort()<0){
          set_trans_unlock();
          return 0;
        }else{
          inc_total_data_size((unsigned int)Stock_Offset);  // increase total data byte size
          Stock_CurNum=0;
          Stock_Offset=0;
        }
      }
      if (Resv_In==1){
        //switch
        if (m_debug) std::cerr << "Switching buffer ..." << std::endl;
	switch_buffer();
        Stock_CurNum=1;
        Stock_Offset=Resv_Size;
        Resv_In=0;
        set_data((unsigned int)Stock_Offset);
        if (write_OutPort()<0){
          ;
        }else{
          inc_total_data_size((unsigned int)Stock_Offset);  // increase total data byte size
          Stock_CurNum=0;
          Stock_Offset=0;
        }
      }
      set_trans_unlock();    // transit to CONFIGURED state
      return 0;
    }

    if (Stock_CurNum==0){
      if (Resv_In==0){
        m_recv_byte_size = read_data_from_detectors();
      }else{
        //switch
        if (m_debug) std::cerr << "Switching buffer ..." << std::endl;
	switch_buffer();
        Stock_CurNum=1;
        Stock_Offset=Resv_Size;
        Resv_In=0;
        return 0;
      }
    }
    if (Stock_CurNum==0) return 0;
    if (m_out_status==BUF_TIMEOUT || m_out_status==BUF_NOBUF){
      if (write_OutPort()<0){
        ;
      }else{
        inc_total_data_size((unsigned int)Stock_Offset);  // increase total data byte size
        Stock_CurNum=0;
        Stock_Offset=0;
      }
    }else if (Resv_In==1){
      if (m_debug) std::cerr << "Buffer overflow... writing from regular tank..." << std::endl;
      set_data((unsigned int)Stock_Offset);
      if (write_OutPort()<0){
        ;
      }else{
        inc_total_data_size((unsigned int)Stock_Offset);  // increase total data byte size
        Stock_CurNum=0;
        Stock_Offset=0;
      }
    }else if (Stock_CurNum<Stock_MaxNum){
        m_recv_byte_size = read_data_from_detectors();
    }else{
      set_data((unsigned int)Stock_Offset);
      if (write_OutPort()<0){
        ;
      }else{
        inc_total_data_size((unsigned int)Stock_Offset);  // increase total data byte size
        Stock_CurNum=0;
        Stock_Offset=0;
      }
    }

    return 0;
