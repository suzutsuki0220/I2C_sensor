debug=1

TARGET_STATIC=bme280_i2c.a
OBJECTS=BME280_I2C.o BME280_config.o

INTERFACE_DIR=../interface
I2C_ACCESSOR=$(INTERFACE_DIR)/interface.a

CPP=g++
LIBS=

ifeq ($(debug), 1)
	CPPFLAGS=-g -Wall
else
	CPPFLAGS=-O2 -Wall
endif
CPPFLAGS+= -I$(INTERFACE_DIR)

.PHONY: all clean

all: $(TARGET_STATIC)

%.o: %.cpp %.h
	$(CPP) $(CPPFLAGS) -o $@ -c $<

$(I2C_ACCESSOR):
	make -C $(INTERFACE_DIR)

$(TARGET_STATIC): $(OBJECTS) $(I2C_ACCESSOR)
	ar rv $@ $^

$(TARGET): $(OBJECTS) main.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f $(TARGET) $(TARGET_STATIC) $(OBJECTS) *~
