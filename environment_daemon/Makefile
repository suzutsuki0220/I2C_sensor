debug=1

TARGET=environment_daemon
OBJECTS=config.o file_operate.o log.o

DRIVER_DIR=../driver
STATIC_DRIVER=$(DRIVER_DIR)/bme280_i2c.a
CPP=g++
LIBS=

ifeq ($(debug), 1)
	CPPFLAGS=-g -Wall
else
	CPPFLAGS=-O2 -Wall
endif
CPPFLAGS+= -I$(DRIVER_DIR)/

.PHONY: all clean

all: $(TARGET)

$(STATIC_DRIVER):
	make -C ../driver

%.o: %.cpp %.h
	$(CPP) $(CPPFLAGS) -o $@ -c $<

$(TARGET): $(OBJECTS) environment_daemon.cpp $(STATIC_DRIVER)
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f $(TARGET) $(OBJECTS) *~
