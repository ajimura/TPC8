    int i;
    unsigned int tot_data_size;
    unsigned int tot_block_size;
    unsigned int eventnum, eventtag, seq_num;
    unsigned int comp_footer;

    unsigned int *toppos;
    unsigned int *curpos;
    int stock_size;

    unsigned int *datapos[2]; // top pointer for the data from in ports (just after the DAQ-MW header)

    datapos[0]=In1_CurPos;
    datapos[1]=In2_CurPos;

    //get&check event num
    std::cerr << "-----eventnum " << datapos[0][3] << ":" << datapos[1][3] << std::endl;
    if ((eventnum=*(datapos[0]+3))!=*(datapos[1]+3)){
      std::cerr << "Event mismatch(num):" << eventnum << " " << *(datapos[1]+3) << std::endl;
      fatal_error_report(USER_DEFINED_ERROR1,"Event mismatch(num)!");
    }
    //get&check event tag
    std::cerr << "-----eventtag " << datapos[0][4] << ":" << datapos[1][4] << std::endl;
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
    std::cerr << "-----size info. " << data1_byte_size << ", " << data2_byte_size << ", " << tot_data_size << std::endl;

    //get current time
    seq_num=get_sequence_num();
        std::cerr << "-----seq num " << seq_num << std::endl;

    // create component header
    toppos=(unsigned int *)(m_data1+Stock_Offset);
    curpos=Create_Header(toppos, &comp_footer,
		  eventnum, eventtag,
		  ComponentType, ComponentID,
		  tot_data_size, (unsigned int)seq_num,
		  0,0);

    memcpy(curpos,In1_CurPos,data1_byte_size); curpos+=(data1_byte_size/4);
    memcpy(curpos,In2_CurPos,data1_byte_size); curpos+=(data2_byte_size/4);
    memcpy(curpos,&comp_footer,FOOTER_BYTE_SIZE); curpos+=(FOOTER_BYTE_SIZE/4);
    stock_size=(curpos-toppos)*4;
    Stock_TotSiz+=stock_size;
    Stock_Offset+=stock_size;
    Stock_CurNum++;
    inc_sequence_num();

    return 0;
