debug?=0

TARGET_STATIC=stub.a
OBJECTS=sensor_stub.o

CPP=g++
LIBS=

ifeq ($(debug), 1)
	CPPFLAGS=-g -Wall
else
	CPPFLAGS=-O2 -Wall
endif

.PHONY: all clean

all: $(TARGET_STATIC)

%.o: %.cpp %.h
	$(CPP) $(CPPFLAGS) -o $@ -c $<

$(TARGET_STATIC): $(OBJECTS)
	ar rv $@ $^

clean:
	rm -f $(TARGET_STATIC) $(OBJECTS) *~
