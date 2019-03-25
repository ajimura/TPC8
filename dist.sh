mkdir TPC8
cd TPC8
cp -p ../cleanall ./
cp -p ../makeall ./
cp -p ../clean.linux ./
cp -p ../make.linux ./
#mkdir drv
mkdir lib
mkdir Merger2to1
mkdir Merger2to1A
mkdir Merger2to1B
mkdir Merger4to1
mkdir TPClogger
mkdir TPCreader
mkdir TPCreaderA
mkdir TPCreaderB
mkdir TPCreaderC
mkdir TPCreaderD
mkdir utils
mkdir utils/obsoleted
mkdir work

#cd drv
#cp -p ../../drv/dev.sh ./
#cp -p ../../drv/Makefile ./
#cp -p ../../drv/swsoc.c ./
#cp -p ../../drv/swsoc.h ./
#cd ..

cd lib
cp -p ../../lib/address.h ./
cp -p ../../lib/daqmwlib.c ./
cp -p ../../lib/daqmwlib.h ./
cp -p ../../lib/Makefile ./
cp -p ../../lib/Makefile.linux ./
cp -p ../../lib/rmap_lib.c ./
cp -p ../../lib/rmap_lib.h ./
cp -p ../../lib/tpclib.c ./
cp -p ../../lib/tpclib.h ./
cp -p ../../lib/tpcmath.c ./
cp -p ../../lib/tpcmath.h ./
cp -p ../../lib/trigio_lib.c ./
cp -p ../../lib/trigio_lib.h ./
cd ..

cd Merger2to1
cp -p ../../Merger2to1/Makefile ./
cp -p ../../Merger2to1/Merger2to1Comp.cpp ./
cp -p ../../Merger2to1/Merger2to1.cpp ./
cp -p ../../Merger2to1/Merger2to1.h ./
cd ..

cd Merger2to1A
cp -p ../../Merger2to1A/Makefile ./
cp -p ../../Merger2to1A/Merger2to1AComp.cpp ./
cp -p ../../Merger2to1A/Merger2to1A.cpp ./
cp -p ../../Merger2to1A/Merger2to1A.h ./
cd ..

cd Merger2to1B
cp -p ../../Merger2to1B/Makefile ./
cp -p ../../Merger2to1B/Merger2to1BComp.cpp ./
cp -p ../../Merger2to1B/Merger2to1B.cpp ./
cp -p ../../Merger2to1B/Merger2to1B.h ./
cd ..

cd Merger4to1
cp -p ../../Merger4to1/Makefile ./
cp -p ../../Merger4to1/Merger4to1Comp.cpp ./
cp -p ../../Merger4to1/Merger4to1.cpp ./
cp -p ../../Merger4to1/Merger4to1.h ./
cd ..

cd TPClogger
cp -p ../../TPClogger/FileUtils.cpp ./
cp -p ../../TPClogger/FileUtils.h ./
cp -p ../../TPClogger/Makefile ./
cp -p ../../TPClogger/TPCloggerComp.cpp ./
cp -p ../../TPClogger/TPClogger.cpp ./
cp -p ../../TPClogger/TPClogger.h ./
cd ..

cd TPCreader
cp -p ../../TPCreader/Makefile ./
cp -p ../../TPCreader/TPCreaderComp.cpp ./
cp -p ../../TPCreader/TPCreader.cpp ./
cp -p ../../TPCreader/TPCreader.h ./
cp -p ../../TPCreader/daq_configure.inc ./
cp -p ../../TPCreader/daq_pause.inc ./
cp -p ../../TPCreader/daq_resume.inc ./
cp -p ../../TPCreader/daq_start.inc ./
cp -p ../../TPCreader/daq_stop.inc ./
cp -p ../../TPCreader/parse_params.inc ./
cp -p ../../TPCreader/read_data_from_detectors.inc ./
cd ..

cd TPCreaderA
cp -p ../../TPCreaderA/Makefile ./
cp -p ../../TPCreaderA/TPCreaderAComp.cpp ./
cp -p ../../TPCreaderA/TPCreaderA.cpp ./
cp -p ../../TPCreaderA/TPCreaderA.h ./
cd ..

cd TPCreaderB
cp -p ../../TPCreaderB/Makefile ./
cp -p ../../TPCreaderB/TPCreaderBComp.cpp ./
cp -p ../../TPCreaderB/TPCreaderB.cpp ./
cp -p ../../TPCreaderB/TPCreaderB.h ./
cd ..

cd TPCreaderC
cp -p ../../TPCreaderC/Makefile ./
cp -p ../../TPCreaderC/TPCreaderCComp.cpp ./
cp -p ../../TPCreaderC/TPCreaderC.cpp ./
cp -p ../../TPCreaderC/TPCreaderC.h ./
cd ..

cd TPCreaderD
cp -p ../../TPCreaderD/Makefile ./
cp -p ../../TPCreaderD/TPCreaderDComp.cpp ./
cp -p ../../TPCreaderD/TPCreaderD.cpp ./
cp -p ../../TPCreaderD/TPCreaderD.h ./
cd ..

cd utils
cp -p ../../utils/Makefile ./
cp -p ../../utils/Makefile.linux ./
cp -p ../../utils/adcini.c ./
cp -p ../../utils/analdata.c ./
cp -p ../../utils/analmwbuf.c ./
cp -p ../../utils/analped.c ./
cp -p ../../utils/analpeak.c ./
#cp -p ../../utils/chk_busy.c ./
cp -p ../../utils/chk_def.c ./
#cp -p ../../utils/chk_dready.c ./
cp -p ../../utils/chk_linkup.c ./
#cp -p ../../utils/chk_trigio.c ./
cp -p ../../utils/de10reset.c ./
cp -p ../../utils/fe_read.c ./
cp -p ../../utils/flush.c ./
cp -p ../../utils/flushall.c ./
cp -p ../../utils/get_data.c ./
cp -p ../../utils/get_ped.c ./
cp -p ../../utils/gettemp.c ./
cp -p ../../utils/getver.c ./
cp -p ../../utils/hardreset.c ./
cp -p ../../utils/linkdown.c ./
cp -p ../../utils/linkup.c ./
cp -p ../../utils/makeped.c ./
cp -p ../../utils/nodedef.c ./
cp -p ../../utils/nodeini.c ./
cp -p ../../utils/nodeini0.c ./
cp -p ../../utils/port0read.c ./
cp -p ../../utils/port2reset.c ./
cp -p ../../utils/port3read.c ./
cp -p ../../utils/port3reset.c ./
cp -p ../../utils/regread.c ./
cp -p ../../utils/regwrite.c ./
cp -p ../../utils/reset.c ./
cp -p ../../utils/set_clkdiv.c ./
cp -p ../../utils/set_def.c ./
cp -p ../../utils/set_fullrange.c ./
cp -p ../../utils/softreset.c ./
cp -p ../../utils/statusall.c ./
cp -p ../../utils/status.c ./
cp -p ../../utils/trigio_cntrst.c ./
cp -p ../../utils/trigio_rdone.c ./
cp -p ../../utils/trigio_read.c ./
cp -p ../../utils/obsoleted/*.c obsoleted/
cd ..

cd work
mkdir examples
cp -p ../../work/examples/LM4RRRR.xml examples/
cp -p ../../work/examples/LMRR.xml examples/
cp -p ../../work/examples/LMRRZ.xml examples/
cp -p ../../work/examples/LR.xml examples/
cp -p ../../work/examples/ped1200.txt examples/
cp -p ../../work/examples/pedestal.txt examples/
cp -p ../../work/examples/pedmax.txt examples/
cp -p ../../work/examples/readychk.txt examples/
cp -p ../../work/examples/trigio.txt examples/
cd ..

cd ..

tar zcf TPC8.tar.gz TPC8

rm -r TPC8
