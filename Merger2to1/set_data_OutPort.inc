    int i;
    unsigned char header[8];
    unsigned char footer[8];
    unsigned int tot_data_size;
    unsigned int tot_block_size;
    unsigned int data1_block_size, data2_block_size; // size without daq-mw header and footer
    unsigned int eventnum, eventtag, seq_num;
    //    time_t curtime;
    //    tm *ltime;
    unsigned int comp_header[19],comp_footer;

    //    unsigned int *datapos1, *datapos2; // top pointer for the data from in ports (just after the DAQ-MW header)
    unsigned int *datapos[2]; // top pointer for the data from in ports (just after the DAQ-MW header)

    datapos[0]=(unsigned int *)&(m_in1_data.data[HEADER_BYTE_SIZE]);
    datapos[1]=(unsigned int *)&(m_in2_data.data[HEADER_BYTE_SIZE]);

    //get&check event num
    //    std::cerr << "-----eventnum " << datapos[0][3] << ":" << datapos[1][3] << std::endl;
    if ((eventnum=*(datapos[0]+3))!=*(datapos[1]+3)){
      std::cerr << "Event mismatch(num):" << eventnum << " " << *(datapos[1]+3) << std::endl;
      fatal_error_report(USER_DEFINED_ERROR1,"Event mismatch(num)!");
    }
    //get&check event tag
    //    std::cerr << "-----eventtag " << datapos[0][4] << ":" << datapos[1][4] << std::endl;
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
    //    if ((eventtag=*(datapos1+4))!=*(datapos2+4)){
    //      std::cerr << "Event mismatch(tag):" << eventtag << " " << *(datapos2+4) << std::endl;
    //      fatal_error_report(USER_DEFINED_ERROR1,"Event mismatch(tag)!");
    //    }


    //get size info.
    data1_block_size=data1_byte_size-HEADER_BYTE_SIZE-FOOTER_BYTE_SIZE;
    data2_block_size=data2_byte_size-HEADER_BYTE_SIZE-FOOTER_BYTE_SIZE;
    tot_block_size=data1_block_size+data2_block_size;
    tot_data_size=tot_block_size+80; //+HEADER_BYTE_SIZE+FOOTER_BYTE_SIZE; //80 = Comp header + Comp Footer
    //    std::cerr << "-----size info. " << data1_block_size << ", " << data2_block_size << ", " << tot_data_size << std::endl;

    //get current time
    seq_num=get_sequence_num();
    //    std::cerr << "-----seq num " << seq_num << std::endl;

    // create component header
    Create_Header(comp_header, &comp_footer,
		  eventnum, eventtag,
		  ComponentType, ComponentID,
		  tot_data_size, (unsigned int)seq_num,
		  0,0);

//     curtime=time(0);
//     ltime=localtime(&curtime);
//     //create component hedaer&footer
//     comp_header[0]=tot_data_size;
//     comp_header[1]=(ltime->tm_year+1900)*1000000+(ltime->tm_mon+1)*10000+(ltime->tm_mday)*100+ltime->tm_hour;
//     comp_header[2]=(ltime->tm_min)*10000000 + (ltime->tm_sec)*100000;
//     comp_header[3]=eventnum;
//     comp_header[4]=eventtag;
//     comp_header[5]=ComponentType;
//     comp_header[6]=ComponentID;
//     comp_header[ 7]=0;   comp_header[ 8]=0;   comp_header[ 9]=0;   comp_header[10]=0;    comp_header[11]=0;
//     comp_header[12]=0;   comp_header[13]=0;   comp_header[14]=0;   comp_header[15]=0;
//     comp_header[16]=0; // numFADC
//     comp_header[17]=0; // numFADC
//     comp_header[18]=(unsigned int)seq_num;
//     comp_footer=0x00000011;

    //set daq-mw header&footer
    set_header(&header[0], tot_data_size);
    set_footer(&footer[0]);
    //    for(i=0;i<8;i++){
    //      std::cerr << (unsigned int)header[i] << ":";
    //    }std::cerr << std::endl;
    //    for(i=0;i<8;i++){
    //      std::cerr << (unsigned int)footer[i] << ":";
    //    }std::cerr << std::endl;
      
    ///set OutPort buffer length
    m_out_data.data.length(tot_data_size+HEADER_BYTE_SIZE+FOOTER_BYTE_SIZE);
    memcpy(&(m_out_data.data[0]), &header[0], HEADER_BYTE_SIZE);
    memcpy(&(m_out_data.data[HEADER_BYTE_SIZE]), &comp_header[0], 76);
    memcpy(&(m_out_data.data[HEADER_BYTE_SIZE+76]), &m_in1_data.data[HEADER_BYTE_SIZE], data1_block_size);
    memcpy(&(m_out_data.data[HEADER_BYTE_SIZE+76+data1_block_size]), 
	   &m_in2_data.data[HEADER_BYTE_SIZE],data2_block_size);
    memcpy(&(m_out_data.data[HEADER_BYTE_SIZE+76+tot_block_size]),&comp_footer, 4);
    memcpy(&(m_out_data.data[HEADER_BYTE_SIZE+76+tot_block_size+4]),&footer[0], FOOTER_BYTE_SIZE);

    return 0;
