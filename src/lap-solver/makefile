CXX=g++
CPPFLAGS=-O3 -MD
LDFLAGS=-pthread
LDLIBS=-lm -lX11

SRCS=$(wildcard *.cpp)
OBJS=$(subst .cpp,.o,$(SRCS))

all: main

main: $(OBJS)
	$(CXX) $(LDFLAGS) -o main $(OBJS) $(LDLIBS)

%.o: %.c
	$(CXX) $(CPPFLAGS) -c $<

clean:
	rm -f *.o *.d main

-include $(OBJS:.o=.d)
