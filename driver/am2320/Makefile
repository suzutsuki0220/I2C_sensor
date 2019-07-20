debug=1

TARGET_STATIC=am2320.a
SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:%.cpp=%.o)

INTERFACE_DIR=../interface
IF_ACCESSOR=$(INTERFACE_DIR)/interface.a

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

$(IF_ACCESSOR):
	make -C $(INTERFACE_DIR)

$(TARGET_STATIC): $(OBJECTS) $(IF_ACCESSOR)
	$(RM) $@
	cp $(IF_ACCESSOR) $@
	ar rv $@ $(OBJECTS)

clean:
	rm -f $(TARGET) $(TARGET_STATIC) $(OBJECTS) *~
