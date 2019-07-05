    int i;
    int tot_data_size;
    int tot_block_size;
    int tot_next_size;
    unsigned int eventnum, eventtag, seq_num;
    unsigned int comp_footer;

    unsigned int *toppos;
    unsigned int *curpos;
    int stock_size;

    unsigned int *datapos; // top pointer for the data from in ports (just after the DAQ-MW header)

    datapos=In_CurPos;

    //get size info.
    tot_block_size=data_byte_size;
    tot_data_size=tot_block_size;
    if (m_debug) std::cerr << "-----size info. " << data_byte_size << std::endl;

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
    curpos=toppos;

    memcpy(curpos,In_CurPos,(size_t)data_byte_size); curpos+=(data_byte_size/4);
    stock_size=(curpos-toppos)*4;
    Stock_Offset+=stock_size;
    Stock_CurNum++;
    inc_sequence_num();

    return;
