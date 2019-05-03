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
    usleep(interval_time);

    seq_num=get_sequence_num();

    //    std::cout << "Get data ready" << std::endl;

    // create component header
    //    curpos=m_data4;
    curpos=Create_Header(m_data4,&comp_footer,
			 eventnum,(unsigned int)eventtag,
			 ComponentType, ComponentID,
			 0, (unsigned int)seq_num,
			 totNumFADCforHeader2,totNumFADCforHeader1);

    //    std::cout << "start event data" << std::endl;

    // get&write data
    totsize=generate_size;
    curpos+=(totsize/4);

    //    std::cout << "end event data" << std::endl;

    //footer start  -> should be modified to fit DAQ-system (20181004: SA)
    //    *(curpos++)=0x00000011;
    *(curpos++)=comp_footer;

    //    std::cout << "Get event data " << totsize << std::endl;
    //    outfile.write((char *)rdata, totsize);
  
    //get event tag from RPC-260 TPC_TrigIO
    eventtag=0;

    //modify header info (totsize, eventtag)
    *m_data4=(int)(curpos-m_data4)*4;
    *(m_data4+4)=eventtag;

    //set return value
    received_data_size=(int)(curpos-m_data4)*4;

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
