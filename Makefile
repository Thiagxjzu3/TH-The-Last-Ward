
.SUFFIXES:

.DEFAULT_GOAL := all

TARGET := THTheLastWard
TITLE := TH: THE LAST WARD
APPID := THLW0001

OFILES := $(patsubst %.cpp,%.o,$(wildcard source/*.cpp))

CXXFLAGS += -Iinclude -I$(PSL1GHT)/ppu/include -O2 -Wall -Wextra
CFLAGS += -Iinclude -I$(PSL1GHT)/ppu/include -O2 -Wall -Wextra

LDFLAGS +=

PKGFILES := data

include $(PSL1GHT)/ppu_rules

.PHONY: all pkg clean

all: $(TARGET).elf

pkg: $(TARGET).pkg

clean:
	rm -f source/*.o
	rm -f $(TARGET).elf
	rm -f $(TARGET).self
	rm -f $(TARGET).fake.self
	rm -f $(TARGET).pkg
	rm -f $(TARGET).gnpdrm.pkg
	rm -rf build
