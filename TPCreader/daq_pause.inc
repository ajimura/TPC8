    //TGC TrigDisable
    if (fadc_trig_disable()<0){
      fatal_error_report(USER_DEFINED_ERROR2,"Error in fadc_trig_disable");
    }

    return 0;
