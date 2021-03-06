    int i;
    int tot_data_size;
    int tot_block_size;
    int tot_next_size;
    unsigned int eventnum, eventtag, seq_num;
    unsigned int comp_footer;

    unsigned int *toppos;
    unsigned int *curpos;
    int stock_size;

    unsigned int *datapos[2]; // top pointer for the data from in ports (just after the DAQ-MW header)

    datapos[0]=In1_CurPos;
    datapos[1]=In2_CurPos;

    //get&check event num
    if (m_debug) std::cerr << "-----eventnum " << datapos[0][3] << ":" << datapos[1][3] << std::endl;
    if ((eventnum=*(datapos[0]+3))!=*(datapos[1]+3)){
      std::cerr << "Event mismatch(num):" << eventnum << " " << *(datapos[1]+3) << std::endl;
      fatal_error_report(USER_DEFINED_ERROR1,"Event mismatch(num)!");
    }
    //get&check event tag
    if (m_debug) std::cerr << "-----eventtag " << datapos[0][4] << ":" << datapos[1][4] << std::endl;
    eventtag=255;
    for(i=0;i<2;i++){
      if (*(datapos[i]+4)!=255){
	if (eventtag==255) eventtag=*(datapos[i]+4);
	else
	  if (eventtag!=*(datapos[i]+4)){
	    std::cerr << "Event mismatch(tag):" << *(datapos[0]+4) << " " << *(datapos[1]+4) <<
	      std::endl;
	    fatal_error_report(USER_DEFINED_ERROR1,"Event mismatch(tag) !");
	  }
      }
    }

    //get size info.
    tot_block_size=data1_byte_size+data2_byte_size;
    tot_data_size=tot_block_size+80; //80 = Comp header + Comp Footer
    if (m_debug) std::cerr << "-----size info. " << data1_byte_size << ", " << data2_byte_size << ", " << tot_data_size << std::endl;

    //get current time
    seq_num=(unsigned int)get_sequence_num();
    if (m_debug) std::cerr << "-----seq num " << seq_num << std::endl;

    // check size of buf
    tot_next_size=Stock_Offset+tot_data_size;
    if (tot_next_size>Cur_MaxDataSiz){
       m_data1=renew_buf(m_data1,(size_t)Stock_Offset,(size_t)tot_next_size);
       Cur_MaxDataSiz=tot_next_size;
    }

    // create component header
    toppos=(unsigned int *)(m_data1+Stock_Offset);
    curpos=Create_Header(toppos, &comp_footer,
		  eventnum, eventtag,
		  ComponentType, ComponentID,
		  (unsigned int)tot_data_size, seq_num,
		  0,0);

    memcpy(curpos,In1_CurPos,(size_t)data1_byte_size); curpos+=(data1_byte_size/4);
    memcpy(curpos,In2_CurPos,(size_t)data2_byte_size); curpos+=(data2_byte_size/4);
    memcpy(curpos,&comp_footer,4); curpos++;
    stock_size=(curpos-toppos)*4;
    Stock_Offset+=stock_size;
    Stock_CurNum++;
    inc_sequence_num();

    return;
