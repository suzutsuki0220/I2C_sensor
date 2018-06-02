debug=1

TARGET_STATIC=bme280_i2c.a
OBJECTS=i2c_access.o spi_access.o BME280_I2C.o BME280_config.o
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

$(TARGET): $(OBJECTS) main.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f $(TARGET) $(TARGET_STATIC) $(OBJECTS) *~
