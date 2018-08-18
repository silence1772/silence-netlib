LIB_SRC = 			\
	main2.cpp \
	eventloop.cpp \
	epoller.cpp \
	eventitem.cpp \
	timer.cpp \
	timerqueue.cpp \
	timestamp.cpp \
	currentthread.cpp \
	thread.cpp \
	mutex.h \
	condition.h \
	eventloopthread.cpp \
	log/logstream.cpp \
	log/logger.cpp \
	socketops.cpp \
	socket.cpp \
	inetaddress.cpp \
	acceptor.cpp \
	tcpconnection.cpp \
	tcpserver.cpp  \
	buffer.cpp

BINARIES = test
CXXFLAGS = -std=c++11
LDFLAGS = -lpthread

$(BINARIES):$(LIB_SRC)
	g++ $(CXXFLAGS) $(LIB_SRC) $(LDFLAGS) -o $(BINARIES)