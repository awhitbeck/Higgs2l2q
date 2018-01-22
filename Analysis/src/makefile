CC=g++
CFLAGS=
LIBS=`root-config --cflags --glibs` -L $(ROOTSYS)/lib -I$(ROOTSYS)/include
ROOFITLIBS=-lRooFit -lHtml -lMinuit -lRooFitCore
INCLUDES=-I$(CMSSW_BASE)/src/Higgs2l2q/Analysis/src/
EXECS=plotObs_baseline
$(EXECS):h2l2qTree.cc definitions.cc skimSamples.cc plotterUtils.cc
plotObs_VBFselection:plotObs_VBFselection.cc
plotObs_ZHselection:plotObs_ZHselection.cc


all:$(EXECS) 
plotObs_VBFselection:
	$(CC) $(CFLAGS) $(LIBS) $(INCLUDES) plotObs_VBFselection.cc -o ../bin/$@
plotObs_ZHselection:
	$(CC) $(CFLAGS) $(LIBS) $(INCLUDES) plotObs_ZHselection.cc -o ../bin/$@
clean : 
	rm ../bin/*