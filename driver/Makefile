debug=1

TARGET=get_sensor
OBJECTS=i2c_access.o spi_access.o BME280_I2C.o BME280_config.o
CPP=g++
LIBS=

ifeq ($(debug), 1)
	CPPFLAGS=-g -Wall
else
	CPPFLAGS=-O2 -Wall
endif

.PHONY: all clean

all: $(TARGET)

%.o: %.cpp %.h
	$(CPP) $(CPPFLAGS) -o $@ -c $<

$(TARGET): $(OBJECTS) main.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f $(TARGET) $(OBJECTS) *~
