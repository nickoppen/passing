
CC = gcc
DEFS += -DDEBUG

CCFLAGS += -g $(DEFS)
INCS = -I. -I/usr/local/browndeer/coprthr2/include 
LIBS = -L/usr/local/browndeer/coprthr2/lib -lcoprthr -lcoprthrcc -lm -ldl

COPRCC = /usr/local/browndeer/coprthr2/bin/coprcc
COPRCC_FLAGS = -g --info
COPRCC_DEFS = $(DEFS) -DCOPRTHR_MPI_COMPAT
COPRCC_INCS = $(INCS)
COPRCC_LIBS = -L/usr/local/browndeer/coprthr2/lib \
	-lcoprthr_hostcall -lcoprthr_mpi -lcoprthr2_dev -lesyscall

TARGET = passing.x passing.e32

all: $(TARGET)
Debug: passing.x
CL: passing.e32

.PHONY: clean install uninstall $(SUBDIRS)

.SUFFIXES:
.SUFFIXES: .c .o .x .e32

passing.x: pass.o
	$(CC) -o passing.x pass.o $(LIBS)

passing.e32: passCL.c passing.h
	$(COPRCC) $(COPRCC_FLAGS) $(COPRCC_LIBS) $(COPRCC_INCS) $(COPRCC_DEFS) -o passing.e32 $<

pass.o: pass.c passing.h
	$(CC) $(CCFLAGS) $(INCS) -c pass.c -o pass.o 

cleanDebug: $(SUBDIRS)
	rm -f *.o 
	rm -f passing.x

cleanCL: $(SUBDIRS)
	rm -f *.e32 

clean: cleanDebug cleanCL
distclean: clean 


