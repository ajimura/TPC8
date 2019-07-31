    int i;

    std::cout << "--- param list (length:" << (*list).length() << ")" << std::endl;

    //set default value
    m_debug=false;
    for(i=0;i<8;i++) NumFADC[i]=0;
    pedestal_exist=false;
    ThresMargin=10;
    trigio_exist=false;
    rdychk_exist=false;
    ComprType=FADC_Sup;
    PExcess=0;
    DExcess=0;
    TrigEnab=FADC_ClkTrig;

    GetETag=0;
    RPV260IP="0.0.0.0";
    RPV260Port=4660;
    ETagDone=false;
    ETagWaitTime=10;

//    UseTrigIO=true;
//    TrigIOIP="0.0.0.0";
//    TrigIOOirt=4660;
//    TrigIO_done=false;
//    TrigIO_waittime=10;
//    UseSWIO=false;
//    SWIO_done=false;

    ComponentID=0;
    ReadTimeout=10000;
    Stock_MaxNum=1;
    Stock_MaxSiz=2097044;
    OutCompress=0;
    CompressLevel=1;

    int len = (*list).length();
    for (i = 0; i < len; i+=2) {
      std::string sname  = (std::string)(*list)[i].value;
      std::string svalue = (std::string)(*list)[i+1].value;

      //      std::cerr << "sname: " << sname << "  ";
      //      std::cerr << "value: " << svalue << std::endl;

      if (sname == "DEBUG"){
      	toLower(svalue);
      	if (svalue == "yes") m_debug=true;
      }

      if (sname == "NumFADC0") NumFADC[0]=atoi(svalue.c_str());
      if (sname == "NumFADC1") NumFADC[1]=atoi(svalue.c_str());
      if (sname == "NumFADC2") NumFADC[2]=atoi(svalue.c_str());
      if (sname == "NumFADC3") NumFADC[3]=atoi(svalue.c_str());
      if (sname == "NumFADC4") NumFADC[4]=atoi(svalue.c_str());
      if (sname == "NumFADC5") NumFADC[5]=atoi(svalue.c_str());
      if (sname == "NumFADC6") NumFADC[6]=atoi(svalue.c_str());
      if (sname == "NumFADC7") NumFADC[7]=atoi(svalue.c_str());
      totNumFADCforHeader1=NumFADC[0]+256*(NumFADC[1]+256*(NumFADC[2]+256*NumFADC[3]));
      totNumFADCforHeader2=NumFADC[4]+256*(NumFADC[5]+256*(NumFADC[6]+256*NumFADC[7]));
      if (sname == "SetPedestal"){
      	toLower(svalue);
      	if (svalue == "yes") pedestal_exist=true;
      }
      if (sname == "PedestalFile"){
	pedestal_file=svalue;
      }
      if (sname == "ThresMargin") ThresMargin=atoi(svalue.c_str());
      if (sname == "SetTrigIO"){
      	toLower(svalue);
      	if (svalue == "yes") trigio_exist=true;
      }
      if (sname == "TrigIOFile"){
	trigio_file=svalue;
      }
      if (sname == "SetRdyChk"){
      	toLower(svalue);
      	if (svalue == "sel") rdychk_exist=true;
      }
      if (sname == "RdyChkFile"){
	rdychk_file=svalue;
      }
      if (sname == "CompressionType"){
      	toLower(svalue);
	if (svalue=="simple") ComprType=FADC_Sup;
	else{
	  ComprType=0;
	  if ((int)svalue.find("p")>=0) ComprType+=FADC_Peak;
	  if ((int)svalue.find("q")>=0) ComprType+=FADC_Peak2;
	  if ((int)svalue.find("r")>=0) ComprType+=FADC_Peak3;
	  if ((int)svalue.find("d")>=0) ComprType+=FADC_Dip;
	  if ((int)svalue.find("e")>=0) ComprType+=FADC_Dip2;
	  if ((int)svalue.find("f")>=0) ComprType+=FADC_Dip3;
	  if ((int)svalue.find("m")>=0) ComprType+=FADC_Mark;
	  if ((int)svalue.find("z")>=0) ComprType+=FADC_Zero;
	}
      }
      if (sname == "PeakExcess") PExcess=atoi(svalue.c_str());
      if (sname == "DipExcess") DExcess=atoi(svalue.c_str());
      if (sname == "TrigType"){
      	toLower(svalue);
	if (svalue=="ext") TrigEnab=FADC_ExtTrig;
	if (svalue=="clock") TrigEnab=FADC_ClkTrig;
//	if (svalue=="local") TrigEnab=FADC_IntTrig;
      }
      if (sname == "ExtReset"){
	toLower(svalue);
	if (svalue=="yes") TrigEnab+=FADC_ExtRst;
      }

      if (sname == "GetETag"){
	toLower(svalue);
	if (svalue=="rpv260") GetETag=1;
#ifdef DE10
	if (svalue=="swio")   GetETag=2;
#endif
      }
      if (sname == "RPV260IP") RPV260IP=svalue;
      if (sname == "RPV260Port") RPV260Port=atoi(svalue.c_str());
      if (sname == "ETagDone"){
	toLower(svalue);
      	if (svalue == "yes") ETagDone=true;
      }
      if (sname == "ETagWaitTime") ETagWaitTime=atoi(svalue.c_str());

      if (sname == "StockNum") Stock_MaxNum=atoi(svalue.c_str());
      if (sname == "StockMaxSize") Stock_MaxSiz=atoi(svalue.c_str());
      if (sname == "OutCompress"){
      	if (svalue == "ZLIB") OutCompress=1;
      	if (svalue == "LZ4") OutCompress=2;
      	if (svalue == "ZSTD") OutCompress=3;
      }
      if (sname == "CompressLevel") CompressLevel=atoi(svalue.c_str());

      if (sname == "ComponentID") ComponentID=atoi(svalue.c_str());
    }

    return 0;
