#include <time.h>
#include "daqmwlib.h"

// The temporal routine to setup header/footer for DAQ-MW component.
// For TPCreader component, two 4-bytes data are stored into the last two reserved header space.
// Only the current Year/Month/Date/Hour/Minuit/Second are obtained in this routine. Othres should
// be put them on calling, or change the values after calling this routine.
// (Maybe) This'll be in the new class for LEPS2 DAQ common. 'toLower' is one of the candidate.
// S. Ajimura

unsigned int *Create_Header(unsigned int *header,
		    unsigned int *footer,
		    unsigned int eventnum,
		    unsigned int eventtag,
		    unsigned int ComponentType,
		    unsigned int ComponentID,
		    unsigned int size,
		    unsigned int seq_num,
		    unsigned int UserData0,
		    unsigned int UserData1){

  unsigned int *curptr;

  time_t curtime;
  struct tm *ltime;
 
  //get current time
  curtime=time(0);
  ltime=localtime(&curtime);

  curptr=header;
  *(curptr++)=size;
  *(curptr++)=(ltime->tm_year+1900)*1000000+(ltime->tm_mon+1)*10000+(ltime->tm_mday)*100+ltime->tm_hour;
  *(curptr++)=(ltime->tm_min)*10000000 + (ltime->tm_sec)*100000;
  *(curptr++)=eventnum;
  *(curptr++)=eventtag;
  *(curptr++)=ComponentID;
  *(curptr++)=ComponentType;
  *(curptr++)=0; // Version: data structure(?)
  // reserved area
  *(curptr++)=0;  *(curptr++)=0;  *(curptr++)=0;  *(curptr++)=0;
  *(curptr++)=0;  *(curptr++)=0;  *(curptr++)=0;  *(curptr++)=0;
  // next 2 4bytes-data are reserved, but used for TPCreader
  //  *(curptr++)=0;  *(curptr++)=0;
  *(curptr++)=UserData0;  *(curptr++)=UserData1;
  *(curptr++)=seq_num;

  *footer=0x00000011; // why the footer has same value for all the kind of the components?

  return curptr;
}
