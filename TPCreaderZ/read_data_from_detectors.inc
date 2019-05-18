    const int message_freq = 1000;

    int received_data_size = 0;
    //    int i;
    time_t curtime;
    tm *ltime;
    int seq_num;
    unsigned int *toppos;
    unsigned int *curpos;
    int totsize;
    unsigned char eventtag=0;
    unsigned int comp_footer;

    int ret;
    struct timespec ts;
    double t0;

    //    std::cout << "Wait next event !" << std::endl;

//    clock_gettime(CLOCK_MONOTONIC,&ts);
//    t0=(ts.tv_sec*1.)+(ts.tv_nsec/1000000000.);
//    std::cout << ">" << std::fixed << std::setprecision(9) << t0 << std::endl;

    // wait trigger
    if (rdychk_exist)
      ret=fadc_check_data_ready_sel();
    else
      ret=fadc_check_data_ready_all();

    if (ret<0){
      std::cerr << "Error in check data ready...(" << ret << ")" << std::endl;
      fatal_error_report(USER_DEFINED_ERROR2,"Error in check_data_ready");
    }else if (ret==0){
//      usleep(10);
      m_recv_timeout_counter++;
      return 0; // not ready
    }

    clock_gettime(CLOCK_MONOTONIC,&ts);
    t0=(ts.tv_sec*1.)+(ts.tv_nsec/1000000000.);
    std::cout << "<" << std::fixed << std::setprecision(9) << t0 << ":" << m_recv_timeout_counter << std::endl;

    m_recv_timeout_counter=0;
    seq_num=get_sequence_num();

    std::cout << "Get data ready (" << seq_num << ")" << std::endl;

    //get data size
    if (fadc_get_totsizeM2()<0){
      //    if (fadc_get_totsizeM()<0){
      std::cerr << "Error in fadc_get_data_sizeM2" << std::endl;
      fatal_error_report(USER_DEFINED_ERROR2,"Error in fadc_get_data_sizeM2");
    }

    //    std::cout << "Get data size" << std::endl;

    // create component header
    toppos=(unsigned int *)(m_data1+Stock_Offset);
//    curpos=Create_Header(m_data4,&comp_footer,
    curpos=Create_Header(toppos,&comp_footer,
			 eventnum,(unsigned int)eventtag,
			 ComponentType, ComponentID,
			 0, (unsigned int)seq_num,
			 totNumFADCforHeader2,totNumFADCforHeader1);

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

    //set return value
//    received_data_size=(int)(curpos-m_data4)*4;
    received_data_size=(int)(curpos-toppos)*4;
    Stock_TotSiz+=received_data_size;
    Stock_Offset+=received_data_size;
    Stock_CurNum++;

    //modify header info (totsize, eventtag)
//    *m_data4=(int)(curpos-m_data4)*4;
//    *(m_data4+4)=eventtag;
    *toppos=(int)(curpos-toppos)*4;
    *(toppos+4)=eventtag;

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

    inc_sequence_num();                     // increase sequence num.
    eventnum++;

    return received_data_size;