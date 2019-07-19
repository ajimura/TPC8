    const int message_freq = 1000;

    int received_data_size = 0;
    time_t curtime;
    tm *ltime;
    unsigned int seq_num;
    unsigned int *toppos;
    unsigned int *curpos;
    unsigned short *curpos2;
    int totsize;
    unsigned char eventtag=0;
    unsigned int comp_footer;
    int i;

    // wait trigger
    usleep(interval_time);

    if (Stock_Offset+generate_size+80>Stock_MaxSiz){
      m_recv_timeout_counter++;
      return 0;
    }else{
      m_recv_timeout_counter=0;
    }

    seq_num=(unsigned int)get_sequence_num();

    //    std::cout << "Get data ready" << std::endl;

    // create component header
    //    curpos=m_data4;
    toppos=(unsigned int *)(m_data1+Stock_Offset);
    curpos=Create_Header(toppos,&comp_footer,
			 eventnum,(unsigned int)eventtag,
			 ComponentType, ComponentID,
			 0, (unsigned int)seq_num,
			 0,0);

    // get&write data
    totsize=generate_size;
    curpos2=curpos;
//random
    for(i=0;i<totsize/2;i++)
      *(curpos2++)=(short)(rand()%65536);
    std::cerr << *curpos << " " << *(curpos+1) << " "  <<
      *(curpos+2) << " "  << *(curpos+3) << std::endl;
//
    curpos+=(totsize/4);

    //footer
    *(curpos++)=comp_footer;

    //    std::cout << "Get event data " << totsize << std::endl;
    //    outfile.write((char *)rdata, totsize);
  
    eventtag=255;

    //set return value
    received_data_size=(int)(curpos-toppos)*4;
    Stock_Offset+=received_data_size;
    Stock_CurNum++;

    //modify header info (totsize, eventtag)
    *toppos=(int)(curpos-toppos)*4;
    *(toppos+4)=eventtag;

    if (seq_num%message_freq == 0){
      curtime=time(0);
      ltime=localtime(&curtime);
      std::cout << std::setw(2) << ltime->tm_hour << ":" << 
        std::setw(2) << ltime->tm_min << ":" << 
        std::setw(2) << ltime->tm_sec << ": ";
      std::cout << "Seq Num(" << seq_num << ") EventTag(" << (int)eventtag << ")" << std::endl;
    }

    inc_sequence_num();
    eventnum++;

    return received_data_size;
