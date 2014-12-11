all: depend abtrain abpredict

abtrain: abtrain.o AdaBoost.o readSampleDataFile.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

abpredict: abpredict.o AdaBoost.o readSampleDataFile.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

depend:
	$(CXX) -MM *.cpp > .depend

-include .depend

compress: abtrain abpredict
	gzexe abtrain && $(RM) abtrain~
	gzexe abpredict && $(RM) abpredict~

decompress:
	test -f abtrain && gzexe -d abtrain && $(RM) abtrain~ || $(MAKE)
	test -f abpredict && gzexe -d abpredict && $(RM) abpredict~ || $(MAKE)

.PHONY: clean distclean depend compress decompress

clean:
	$(RM) *.o

distclean: clean
	$(RM) $(BINARY)
