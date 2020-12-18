CC	= /home/jalcaraz/spack/opt/spack/linux-centos7-sandybridge/gcc-4.8.5/gcc-10.2.0-ozjtuziwjq5wy3zfs7lcdorsd36k5c6h/bin/g++
CFLAGS	= -Wall
EXECNAME= Mutator-counter
APP= MiniApp
DYNINST_ROOT=/home/jalcaraz/spack/opt/spack/linux-centos7-sandybridge/gcc-10.2.0/dyninst-10.2.1-j5rxy2pjonovnryzzymvc7yir67zlda3/
INCLUDE=$(DYNINST_ROOT)/include

$(EXECNAME): $(EXECNAME).o
	$(CC) $(EXECNAME).o -o $(EXECNAME) -L$(DYNINST_ROOT)/lib -lelf -liberty -ldwarf -ldyninstAPI

$(EXECNAME).o: ./$(EXECNAME).C
	$(CC) -c $(CFLAGS) -I$(INCLUDE) --verbose  $(EXECNAME).C

	$(CC) -g $(APP).cc -o $(APP)

#build:
#	cd /home/aolmo/dyninst/install-9.1/build && make -j 8 && make install 

clean:
	@rm -f $(EXECNAME).o $(EXECNAME) core $(APP)

