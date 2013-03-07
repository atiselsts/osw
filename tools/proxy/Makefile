PROJDIR = $(CURDIR)
ifndef OSW
  OSW = $(CURDIR)/../..
endif

all: pc-cloud

run: pc-cloud
	make all && ./pc-cloud

pc-cloud: proxy.o
	g++ -o pc-cloud proxy.o -lpthread

proxy.o: main.cpp
	g++ -o proxy.o -c main.cpp -I $(OSW)/mos/mod/net \
		-I $(OSW)/mos/hal/platforms/pc -I $(OSW)/mos/hil \
		-I $(OSW)/mos/kernel \
		-I $(OSW)/mos \
		-D__bool_defined
	
clean:
	rm -rf proxy.o pc-cloud
