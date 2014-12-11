CC=gcc
CFLAGS=-Iincludes -Wall -Wextra
ifeq ($(CC), gcc)
	CFLAGS += --short-enums
endif

BINARY=ngram_sorter

all: CFLAGS += -Ofast
all: LDFLAGS += -s
all: depend $(BINARY)

debug: CFLAGS += -g -DDEBUG
debug: depend $(BINARY)

$(BINARY): utilsV5.o $(BINARY).o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

depend:
	$(CC) -Iincludes -MM *.c > .depend

-include .depend

compress: $(BINARY)
	gzexe $(BINARY) && $(RM) $(BINARY)~

decompress:
	test -f $(BINARY) && gzexe -d $(BINARY) && $(RM) $(BINARY)~ || $(MAKE)

.PHONY: clean distclean depend

clean:
	$(RM) *.o

distclean: clean
	$(RM) $(BINARY)
