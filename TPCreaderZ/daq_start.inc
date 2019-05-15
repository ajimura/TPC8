    //reset trig count
    if (fadc_reset_trigcount()<0){
      std::cerr << "Error in fadc_reset_trigcount !!" << std::endl;
      fatal_error_report(USER_DEFINED_ERROR2,"Error in fadc_reset_trigcount");
    }else{
      std::cout << "fadc_reset_trigcount ... Done" << std::endl;
    }
    //RPV-260 trigger count reset

    if (trigio_reset_count()<0){
      std::cerr << "Error in trigio_reset_count !!" << std::endl;
      fatal_error_report(USER_DEFINED_ERROR3,"Error in trigio_reset_count");
    }else{
      std::cout << "trigio_reset_count ... Done" << std::endl;
    }

    //Trig enab.
    if(fadc_trig_enable(TrigEnab)<0){
      std::cerr << "Error in fadc_trig_enable !!" << std::endl;
      fatal_error_report(USER_DEFINED_ERROR2,"Error in fadc_trig_enable");
    }else{
      std::cout << "fadc_trig_enable ... Done" << std::endl;
    }

    m_out_status = BUF_SUCCESS;
    eventnum = 0;

    return 0;
