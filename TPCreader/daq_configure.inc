
    ::NVList* paramList;
    paramList = m_daq_service0.getCompParams();
    parse_params(paramList);

    // show obtained parameter
    std::cout << "NumFADC: " << NumFADC[0] << " " << NumFADC[1] << " " << NumFADC[2] << " " << NumFADC[3] <<  " " <<
      NumFADC[4] << " " << NumFADC[5] << " " << NumFADC[6] << " " << NumFADC[7] << std::endl;
    std::cout << std::hex;
    std::cout << 
      "NumFADC: 0x" << std::setfill('0') << std::setw(8) << totNumFADCforHeader1 << 
      ", 0x" << std::setfill('0') << std::setw(8) << totNumFADCforHeader2 << 
      std::endl;
    std::cout << std::dec;
    std::cout << "Pedestal Data: " << pedestal_exist << std::endl;
    std::cout << "Pedestal File: " << pedestal_file << std::endl;
    std::cout << "Threshold Margin: " << ThresMargin << std::endl;
    std::cout << "TrigIO Data: " << trigio_exist << std::endl;
    std::cout << "TrigIO File: " << trigio_file << std::endl;
    std::cout << "RdyChk Data: " << rdychk_exist << std::endl;
    std::cout << "RdyChk File: " << rdychk_file << std::endl;
    std::cout << std::hex;
    std::cout << "Compression Type: 0x" << 
      std::setfill('0') << std::setw(4) << std::hex << ComprType << std::endl;
    std::cout << "Trigger Type: 0x" << 
      std::setfill('0') << std::setw(4) << std::hex << TrigEnab << std::endl;
    std::cout << std::dec;
    // TrigIO param.
    std::cout << "TrigIO IP Addr: " << TrigIOIP << std::endl;
    std::cout << "TrigIO UDP Port: " << TrigIOPort << std::endl;
    std::cout << "TrigIO Done: " << TrigIO_done << std::endl;
    std::cout << "TrigIO Wait: " << TrigIO_waittime << std::endl;
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
    }else{
      std::cout << "SpaceWire port(s) opened" << std::endl;
    }
    // set adc info
    std::cout << "fadc_set_adc_info ... ";
    fadc_set_adc_info();
    std::cout << "Done" << std::endl;

    // allocate data buffer
    m_data4=new unsigned int[
		CompHeaderSize+
		EventHeaderSize+
		(FADCHeaderSize+(256+ChHeaderSize)*16)*totNumFADC
	     ];
    m_data1=(unsigned char *)m_data4;
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
    // TrigIO
    if (trigio_ini(TrigIOIP.c_str(), TrigIOPort)<0){
      std::cerr << "Error in trigio_ini !!" << std::endl;
      fatal_error_report(USER_DEFINED_ERROR3,"Error in trigio_ini");
    }else{
      std::cout << "trigini ... Done" << std::endl;
    }
    //RPV-260 trigger count reset

    if (TrigIO_done==true)
      if (trigio_reset_count()<0){
        std::cerr << "Error in trigio_reset_count !!" << std::endl;
        fatal_error_report(USER_DEFINED_ERROR3,"Error in trigio_reset_count");
      }else{
        std::cout << "trigio_reset_count ... Done" << std::endl;
      }

    std::cout << "--- Now ready to start !!" << std::endl;

    return 0;
