    if (check_trans_lock()){
      std::cout << "Stop command has come. Now Stock_CurNum=" << Stock_CurNum << std::endl;
      set_trans_unlock();    // transit to CONFIGURED state
      return 0;
    }

    if (Stock_CurNum==0){
      if (Resv_In==0){
        m_recv_byte_size = read_data_from_detectors();
      }else{
	//switch
	if (SwitchAB==0){
	  m_data1=m_dataB;	  m_data4=(unsigned char *)m_dataB;
	  m_resv1=m_dataA;	  m_resv4=(unsigned char *)m_dataA;
	  SwitchAB=1;
	}else{
	  m_data1=m_dataA;	  m_data4=(unsigned char *)m_dataA;
	  m_resv1=m_dataB;	  m_resv4=(unsigned char *)m_dataB;
	  SwitchAB=0;
	}
	Stock_CurNum=1;
	Stock_TotSiz=Resv_Size;
	Stock_Offset=Resv_Size;
	return 0;
      }
    }
    if (Stock_CurNum==0) return 0;
    if (m_out_status==TIMEOUT || m_out_status==BUF_NOBUF){
      if (write_OutPort()<0){
	;
      }else{
	inc_total_data_size(Stock_Offset);  // increase total data byte size
	Stock_CurNum=0;
	Stock_Offset=0;
      }
    }else if (Resv_In==1){
      set_data(Stock_Offset);
      if (write_OutPort()<0){
	;
      }else{
	inc_total_data_size(Stock_Offset);  // increase total data byte size
	Stock_CurNum=0;
	Stock_Offset=0;
      }
    }else if (Stock_CurNum<Stock_MaxNum){
        m_recv_byte_size = read_data_from_detectors();
    }else{
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
