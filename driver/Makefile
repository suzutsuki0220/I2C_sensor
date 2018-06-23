debug?=0
driver?=bme280

TEST_PROG_TARGET=get_environment
TEST_PROG_TARGET_SRC=$(TEST_PROG_TARGET).cpp
TARGET_STATIC=environment_driver.a

INTERFACE_DIR=interface
INTERFACE=$(INTERFACE_DIR)/interface.a

ifeq ($(driver), bme280)
	SENSOR_DRIVER_DIR=BME280
	SENSOR_DRIVER=$(SENSOR_DRIVER_DIR)/bme280_i2c.a
else
ifeq ($(driver), stub)
	SENSOR_DRIVER_DIR=stub
	SENSOR_DRIVER=$(SENSOR_DRIVER_DIR)/sensor_stub.a
endif
endif

CPP=g++
LIBS=

ifeq ($(debug), 1)
	CPPFLAGS=-g -Wall
else
	CPPFLAGS=-O2 -Wall
endif

.PHONY: all clean

all: $(TARGET_STATIC) $(TEST_PROG_TARGET)

%.o: %.cpp %.h
	$(CPP) $(CPPFLAGS) -o $@ -c $<

$(SENSOR_DRIVER):
	make -C $(SENSOR_DRIVER_DIR)

$(INTERFACE):
	make -C $(INTERFACE_DIR)/

$(TARGET_STATIC): $(SENSOR_DRIVER) $(INTERFACE)
	$(RM) $@
	ar cqT $@ $^
	echo -e "create $@\naddlib $@\nsave\nend" | ar -M

$(TEST_PROG_TARGET): $(TEST_PROG_TARGET_SRC) $(TARGET_STATIC)
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

clean:
	$(RM) $(TEST_PROG_TARGET) $(TARGET_STATIC) *~
	make -C $(SENSOR_DRIVER_DIR) clean
	make -C $(INTERFACE_DIR) clean
