    //Trig enab.
    if(fadc_trig_enable(TrigEnab)<0){
      fatal_error_report(USER_DEFINED_ERROR2,"Error in fadc_trig_enable");
    }else{
      std::cout << "fadc_trig_enable ... Done" << std::endl;
    }

    return 0;
