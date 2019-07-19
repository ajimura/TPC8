    ::NVList* paramList;
    paramList = m_daq_service0.getCompParams();
    parse_params(paramList);

    In_TotSiz=0;
    In_RemainSiz=0;
    In_CurPos=NULL;
    In_Done=0;

    Stock_CurNum=0;
    Stock_Offset=0;
    //    Cur_MaxDataSiz=67108864; // 64M (tempolary)
    Cur_MaxDataSiz=10240; // 10k (tempolary)
    Cur_MaxDataSiz1=10240; // 10k (tempolary)

    try{
      DataPos1=new unsigned char[Cur_MaxDataSiz1];
      m_data1=new unsigned char[Cur_MaxDataSiz];
      m_data4=(unsigned int *)m_data1;
    }
    catch(std::bad_alloc){
      std::cerr << "Bad allocation..." << std::endl;
      fatal_error_report(USER_DEFINED_ERROR1);
    }
    catch(...){
      std::cerr << "Got exception..." << std::endl;
      fatal_error_report(USER_DEFINED_ERROR1);
    }

    // show obtained parameter
    std::cout << "Stock Max Num: " << Stock_MaxNum << std::endl;
    std::cout << "Stock Max Size: " << Stock_MaxSiz << std::endl;
    //
    std::cout << "Compress Method on OutPort: ";
    switch(OutCompress){
    case 1: std::cout << "ZLIB" << std::endl; break;
    case 2: std::cout << "LZ4" << std::endl; break;
    default: std::cout << "NONE" << std::endl; break;
    }
    std::cout << "--- Now ready to start !!" << std::endl;

    return 0;
