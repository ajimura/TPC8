    ::NVList* paramList;
    paramList = m_daq_service0.getCompParams();
    parse_params(paramList);

    Stock_CurNum=0;
    Stock_Offset=0;
    SwitchAB=0; // 0:A, 1:B
    Resv_In=0;
    Resv_Size=0;

    // show obtained parameter
    std::cout << "NumFADC: " <<
          NumFADC[0] << " " << NumFADC[1] << " " << NumFADC[2] << " " << NumFADC[3] <<  " " <<
          NumFADC[4] << " " << NumFADC[5] << " " << NumFADC[6] << " " << NumFADC[7] << std::endl;
    std::cout << std::hex;
    std::cout << 
      "NumFADC: 0x" << std::setfill('0') << std::setw(8) << totNumFADCforHeader1 << 
             ", 0x" << std::setfill('0') << std::setw(8) << totNumFADCforHeader2 << std::endl;
    std::cout << std::dec;
    if (pedestal_exist){
      std::cout << "Pedestal Set: yes,  File: " << pedestal_file << std::endl;
      std::cout << "           Thres. Margin: " << ThresMargin << std::endl;
    }else
      std::cout << "Pedestal Data: no" << std::endl;
    if (trigio_exist)
      std::cout << "TrigIO Set: yes, File: " << trigio_file << std::endl;
    else
      std::cout << "TrigIO Set: no" << std::endl;
    if (rdychk_exist)
      std::cout << "RdyChk Set: yes, File: " << rdychk_file << std::endl;
    else
      std::cout << "RdyChk Set: no" << std::endl;
    std::cout << std::hex;
    std::cout << "Compression Type: 0x" << 
      std::setfill('0') << std::setw(2) << std::hex << (ComprType & 0xff) << std::endl;
    std::cout << std::dec;
    if (excess_exist)
      std::cout << "Excess Set from " << excess_file << std::endl;
    else
      std::cout << "  Peak Excess=" << PExcess << ", Dip Excess=" << DExcess << std::endl;
    std::cout << "Trigger Type: 0x" << 
      std::setfill('0') << std::setw(2) << std::hex << (TrigEnab & 0xff) << std::endl;
    std::cout << std::dec;
    std::cout << "Trigger Type: 0x" << 
      std::setfill('0') << std::setw(2) << std::hex << (TrigEnab & 0xff) << std::endl;
    std::cout << std::dec;
    // Event Tag
    if (GetETag==0){
      std::cout << "Event Tag: None" << std::endl;
    }else{
      if (GetETag==1){
        std::cout << "Event Tag from RPV260: IP=" << RPV260IP << ", Port=" << RPV260Port << std::endl;
      }else{
        std::cout << "Event Tag from SWIO" << std::endl;
      }
      if (ETagDone){
        std::cout << "  Issue Done: WaitTime=" << ETagWaitTime << std::endl;
      }
    }
    //
    std::cout << "Stock Max Num: " << Stock_MaxNum << std::endl;
    std::cout << "Stock Max Size: " << Stock_MaxSiz << std::endl;
    //
    std::cout << "Compress Method on OutPort: ";
    switch(OutCompress){
    case 1: std::cout << "ZLIB" << std::endl; break;
    case 2: std::cout << "LZ4" << std::endl; break;
    case 3: std::cout << "ZSTD" << std::endl; break;
    default: std::cout << "NONE" << std::endl; break;
    }
    //
    std::cout << std::hex;
    std::cout << "Component Type: 0x" << 
      std::setfill('0') << std::setw(8) << std::hex << ComponentType << std::endl;
    std::cout << "Component ID: 0x" << 
      std::setfill('0') << std::setw(8) << std::hex << ComponentID << std::endl;
    std::cout << std::dec;

    // Initialization start
    std::cout << "--- Initialization starting..." << std::endl;
    // open space-wire port and check link status
    if ((totNumFADC=sw_open_check(NumFADC))<0){
      std::cerr << "Error in sw_open_check !!" << std::endl;
      fatal_error_report(USER_DEFINED_ERROR1,"Error in sw_open_check");
    }else if (totNumFADC>120){
      std::cerr << "FADC's are too much... #=" << totNumFADC << std::endl;
      fatal_error_report(USER_DEFINED_ERROR1,"Error in sw_open_check");
    }else{
      std::cout << "SpaceWire port(s) opened" << std::endl;
    }
    // set adc info
    std::cout << "fadc_set_adc_info ... ";
    fadc_set_adc_info();
    std::cout << "Done" << std::endl;

    // allocate data buffer
    m_out_data.data.length((unsigned int)Stock_MaxSiz);

    try{
      m_dataA=new unsigned char[(CompHeaderSize + MaxFADCSize*totNumFADC) * Stock_MaxNum];
      m_dataB=new unsigned char[(CompHeaderSize + MaxFADCSize*totNumFADC) * Stock_MaxNum];
    }
    catch(const std::bad_alloc&){
      std::cerr << "Bad allocation..." << std::endl;
      fatal_error_report(USER_DEFINED_ERROR3);
    }
    catch(...){
      std::cerr << "Got exception..." << std::endl;
      fatal_error_report(USER_DEFINED_ERROR3);
    }
    m_data1=m_dataA;
    m_data4=(unsigned int *)m_dataA;
    m_resv1=m_dataB;
    m_resv4=(unsigned int *)m_dataB;
    std::cout << "Prepare data buffer done" << std::endl;

    // setup adc card
    if (fadc_setup_all()<0){
      std::cerr << "Error in fadc_setup_all !!" << std::endl;
      fatal_error_report(USER_DEFINED_ERROR2,"Error in fadc_setup_all.");
    }else{
      std::cout << "fadc_setup_all ... Done" << std::endl;
    }
    // set threshold
    std::cout << "fadc_set_thres_zero ... ";
    fadc_set_thres_zero();
    std::cout << "Done" << std::endl;
    if (pedestal_exist==true){
      if (fadc_read_thres(pedestal_file.c_str(),ThresMargin)<0){
        std::cerr << "Error in fadc_read_thres !!" << std::endl;
	fatal_error_report(USER_DEFINED_ERROR3,"Error in fadc_read_thres");
      }else{
	std::cout << "fadc_read_thres ... Done" << std::endl;
      }
    }
    //reset trig count
    if (fadc_reset_trigcount()<0){
      std::cerr << "Error in fadc_reset_trigcount !!" << std::endl;
      fatal_error_report(USER_DEFINED_ERROR2,"Error in fadc_reset_trigcount");
    }else{
      std::cout << "fadc_reset_trigcount ... Done" << std::endl;
    }
    //set trig I/O
    if (trigio_exist==true){
      fadc_enable_localtrig_all();
      if (fadc_read_set_trigio(trigio_file.c_str())<0){
        std::cerr << "Error in fadc_read_set_trigio !!" << std::endl;
	fatal_error_report(USER_DEFINED_ERROR3,"Error in fadc_read_set_trigio");
      }else{
	std::cout << "fadc_read_set_trigio ... Done" << std::endl;
      }
    }else{
      if (fadc_disable_localtrig_all()<0){
	std::cerr << "Error in fadc_disable_localtrig_all !!" << std::endl;
	fatal_error_report(USER_DEFINED_ERROR2,"Error in fadc_disable_localtrig_all");
      }else{
	std::cout << "fadc_disable_localtrig_all ... Done" << std::endl;
      }
    }
    //set ready chk
    if (rdychk_exist==true){
      fadc_read_readychk(rdychk_file.c_str());
    }
    // set compression type
    if (fadc_set_comp_all(ComprType)<0){
      std::cerr << "Error in fadc_set_comp_all !!" << std::endl;
      fatal_error_report(USER_DEFINED_ERROR2,"Error in fadc_set_comp_all");
    }else{
      std::cout << "fadc_set_comp_all ... Done" << std::endl;
    }
    // excess
    if (excess_exist==true){
      fadc_read_excess(excess_file.c_str());
      if (fadc_set_excess_all()<0){
	std::cerr << "Error in setting excess !!" << std::endl;
	fatal_error_report(USER_DEFINED_ERROR2,"Error in setting excess");
      }else{
	std::cout << "fadc_set_excess_all ... Done" << std::endl;
      }
    }else{
      if (fadc_set_peakexcess_all(PExcess)<0){
	std::cerr << "Error in fadc_set_peakexcess_all !!" << std::endl;
	fatal_error_report(USER_DEFINED_ERROR2,"Error in fadc_set_peakexcess_all");
      }
      if (fadc_set_dipexcess_all(DExcess)<0){
	std::cerr << "Error in fadc_set_dipexcess_all !!" << std::endl;
	fatal_error_report(USER_DEFINED_ERROR2,"Error in fadc_set_dipexcess_all");
      }
    }
    fadc_set_mark_level(MarkLevel);
    // RPV260
    if (GetETag==1){
      if (trigio_ini(RPV260IP.c_str(), RPV260Port)<0){
        std::cerr << "Error in trigio_ini !!" << std::endl;
        fatal_error_report(USER_DEFINED_ERROR3,"Error in trigio_ini");
      }else{
        std::cout << "trigini ... Done" << std::endl;
      }
      //RPV-260 trigger count reset
      if (ETagDone==true){
        if (trigio_reset_count()<0){
          std::cerr << "Error in trigio_reset_count !!" << std::endl;
          fatal_error_report(USER_DEFINED_ERROR3,"Error in trigio_reset_count");
        }else{
          std::cout << "trigio_reset_count ... Done" << std::endl;
        }
      }
    }
#ifdef DE10
    if (GetETag==2){
      if (swio_open()<0){
        std::cerr << "Error in swio_open !!" << std::endl;
        fatal_error_report(USER_DEFINED_ERROR3,"Error in swio_open");
      }
      if (ETagDone==true){
        if (swio_reset()){
          std::cerr << "Error in swio_reset !!" << std::endl;
          fatal_error_report(USER_DEFINED_ERROR3,"Error in swio_reset");
        }else{
          std::cout << "swio_reset ... Done" << std::endl;
        }
      }
    }
#endif
    std::cout << "--- Now ready to start !!" << std::endl;

    return 0;
