
obj=jhlib.o \
	main.o \
	sha2.o \
	transaction.o \
	xptClient.o \
	xptClientPacketHandler.o \
	xptPacketbuffer.o \
	xptServer.o \
	xptServerPacketHandler.o \
	win.o \
	CProtoshareProcessor.o \
	AbstractMomentum.o \
	OpenCLObjects.o \
	OpenCLMomentum2.o \
	OpenCLMomentumV3.o \
	OpenCLMomentumV4.o \
	OpenCLMomentumV5.o \

app=jhProtominer

all: $(app)

.cpp.o: 
	$(CXX) $(CFLAGS) -O3 -march=native -c -o $@ $<

jhProtominer: $(obj)
	$(CXX) $(LDFLAGS) -o $@ $(obj) -lpthread -framework OpenCL

clean:
	$(RM) *.o $(app)

.PHONY: all clean
