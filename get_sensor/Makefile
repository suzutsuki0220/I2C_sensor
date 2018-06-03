debug=1

TARGET=get_sensor
OBJECTS=

DRIVER_DIR=../driver
STATIC_DRIVER=$(DRIVER_DIR)/environment_driver.a
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
	make -C $(DRIVER_DIR)

%.o: %.cpp %.h
	$(CPP) $(CPPFLAGS) -o $@ -c $<

$(TARGET): $(OBJECTS) get_sensor.cpp $(STATIC_DRIVER)
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f $(TARGET) $(OBJECTS) *~
