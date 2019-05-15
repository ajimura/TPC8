    const int message_freq = 1000;

    int received_data_size = 0;
    //    int i;
    time_t curtime;
    tm *ltime;
    int seq_num;
    unsigned int *curpos;
    int totsize;
    unsigned char eventtag=0;
    unsigned int comp_footer;

    //    std::cout << "Wait next event !" << std::endl;

    /// write your logic here
    // wait trigger
    if (rdychk_exist){
      if (fadc_wait_data_ready_sel()<0){
	std::cerr << "Timeout..." << std::endl;
	return received_data_size;
      }
    }else{
      if (fadc_wait_data_ready_all()<0){
	std::cerr << "Timeout..." << std::endl;
	return received_data_size;
      }
    }

    seq_num=get_sequence_num();

    //    std::cout << "Get data ready" << std::endl;

    //get data size
    if (fadc_get_totsizeM2()<0){
      //    if (fadc_get_totsizeM()<0){
      std::cerr << "Error in fadc_get_data_sizeM2" << std::endl;
      fatal_error_report(USER_DEFINED_ERROR2,"Error in fadc_get_data_sizeM2");
    }

    //    std::cout << "Get data size" << std::endl;

    // create component header
    //    curpos=m_data4;
    curpos=Create_Header(m_data4,&comp_footer,
			 eventnum,(unsigned int)eventtag,
			 ComponentType, ComponentID,
			 0, (unsigned int)seq_num,
			 totNumFADCforHeader2,totNumFADCforHeader1);

//
// 20181004: SA: all these stuff put into the function which should be one of the common for LEPS2 DAQ.
//
//     //get current time
//     curtime=time(0);
//     ltime=localtime(&curtime);

//     //    std::cout << "start header" << std::endl;

//     //header start  -> should be modified to fit DAQ-system
//     curpos++; //reserve for totalsize
//     *(curpos++)=(ltime->tm_year+1900)*1000000
//       +(ltime->tm_mon+1)*10000
//       +(ltime->tm_mday)*100
//       +ltime->tm_hour;
//     *(curpos++)=(ltime->tm_min)*10000000 + (ltime->tm_sec)*100000;
//     *(curpos++)=eventnum;
//     *(curpos++)=(unsigned int)eventtag;
// // originally hard-coded as 400 for bothe type and id
// // they should be get from header file and parameter from DAQ operator
//     //    *(curpos++)=400;
//     //    *(curpos++)=400;
//     *(curpos++)=ComponentType;
//     *(curpos++)=ComponentID;;
//     curpos = curpos+9;
//     *(curpos++)=totNumFADCforHeader2;
//     *(curpos++)=totNumFADCforHeader1;
//     *(curpos++)=(unsigned int)seq_num;

    //    std::cout << "start event data" << std::endl;

    // get&write data
    if ((totsize=fadc_get_event_dataM2(curpos,seq_num))<0){
    //    if ((totsize=fadc_get_event_dataM(curpos,seq_num))<0){
      std::cerr << "Error in fadc_get_eventdataM2" << std::endl;
      fatal_error_report(USER_DEFINED_ERROR2,"Error in fadc_get_event_dataM2");
    }
    curpos+=(totsize/4);

    //    std::cout << "end event data" << std::endl;

    //footer start  -> should be modified to fit DAQ-system (20181004: SA)
    //    *(curpos++)=0x00000011;
    *(curpos++)=comp_footer;

    //    std::cout << "Get event data " << totsize << std::endl;
    //    outfile.write((char *)rdata, totsize);
  
    // prepare for next event
    //ROC
    //    if (fadc_release_buffer()<0){
    //      fatal_error_report(USER_DEFINED_ERROR2,"Error in fadc_release_buffer");
    //    }

    //increment next
    fadc_prepare_next();

    //get event tag from RPC-260 TPC_TrigIO
    if (trigio_read_eventtag(&eventtag)<0){
      std::cerr << "Error in trigio_read_eventtag !!" << std::endl;
      fatal_error_report(USER_DEFINED_ERROR2,"Error in trigio_read_eventtag");
    }

    //modify header info (totsize, eventtag)
    *m_data4=(int)(curpos-m_data4)*4;
    *(m_data4+4)=eventtag;

    //set return value
    received_data_size=(int)(curpos-m_data4)*4;

    //increment ring buffer read pointer for TrigIO
    if (TrigIO_done){
      usleep(TrigIO_waittime);
      trigio_read_done();
    }

    if (seq_num%message_freq == 0){
      curtime=time(0);
      ltime=localtime(&curtime);
      std::cout << std::setw(2) << ltime->tm_hour << ":" << 
        std::setw(2) << ltime->tm_min << ":" << 
        std::setw(2) << ltime->tm_sec << ": ";
      std::cout << "Seq Num(" << seq_num << ") EventTag(" << (int)eventtag << ")" << std::endl;
    }

    //    usleep(500000);
    //    for(i=0;i<16;i++){
    //      m_data1[i]=i;
    //    }
    //    received_data_size=16;

    eventnum++;

    return received_data_size;
