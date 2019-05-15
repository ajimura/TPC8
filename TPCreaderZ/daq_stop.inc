    //TGC TrigDisable
    if (fadc_trig_disable()<0){
      fatal_error_report(USER_DEFINED_ERROR2,"Error in fadc_trig_disable");
    }
    // flush buffer and get next buf ID
    if (fadc_setup_all()<0){
      fatal_error_report(USER_DEFINED_ERROR2,"Error in fadc_setup_all.");
    }else{
      std::cout << "fadc_setup_all ... Done" << std::endl;
    }
    //reset trig count
    if (fadc_reset_trigcount()<0){
      fatal_error_report(USER_DEFINED_ERROR2,"Error in fadc_reset_trigcount");
    }else{
      std::cout << "fadc_reset_trigcount ... Done" << std::endl;
    }
    //RPV-260 trigger count reset
    if (trigio_reset_count()<0){
      std::cout << "Error" << std::endl;
      fatal_error_report(USER_DEFINED_ERROR3,"Error in trigio_reset_count");
    }

    return 0;
