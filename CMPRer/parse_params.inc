    std::cerr << "param list length:" << (*list).length() << std::endl;
    //set default value
    m_debug=false;
    ReadTimeout=10000;
    Stock_MaxNum=1;
    Stock_MaxSiz=2097044;
    OutCompress=0;

    int len = (*list).length();
    for (int i = 0; i < len; i+=2) {
        std::string sname  = (std::string)(*list)[i].value;
        std::string svalue = (std::string)(*list)[i+1].value;

	//        std::cerr << "sname: " << sname << "  ";
	//        std::cerr << "value: " << svalue << std::endl;

      if (sname == "DEBUG"){
      	if (svalue == "yes") m_debug=true;
      }

      if (sname == "OutCompress"){
      	if (svalue == "ZLIB") OutCompress=1;
      	if (svalue == "LZ4") OutCompress=2;
      }
    }

    return 0;
