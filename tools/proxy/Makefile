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
	g++ -o proxy.o -c main.cpp -I $(OSW)/src/net \
		-I $(OSW)/src/arch/pc -I $(OSW)/src/include \
		-I $(OSW)/src/kernel \
		-I $(OSW)/src \
		-D__bool_defined

clean:
	rm -rf proxy.o pc-cloud
