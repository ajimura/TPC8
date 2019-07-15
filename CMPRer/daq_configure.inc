    ::NVList* paramList;
    paramList = m_daq_service0.getCompParams();
    parse_params(paramList);

    In_TotSiz=0;
    In_CurPos=NULL;
    In_Done=0;


    // show obtained parameter
//    std::cout << "Stock Max Num: " << Stock_MaxNum << std::endl;
//    std::cout << "Stock Max Size: " << Stock_MaxSiz << std::endl;
    //
    std::cout << "--- Now ready to start !!" << std::endl;

    return 0;
