
.SUFFIXES:

.DEFAULT_GOAL := all

TARGET := THTheLastWard
TITLE := TH: THE LAST WARD
APPID := THLW00001
CONTENTID := UP0001-$(APPID)_00-0000000000000000

OFILES := $(patsubst %.cpp,%.o,$(wildcard source/*.cpp))

CXXFLAGS += -Iinclude $(LIBPSL1GHT_INC) -O2 -Wall -Wextra
CFLAGS += -Iinclude $(LIBPSL1GHT_INC) -O2 -Wall -Wextra

LDFLAGS +=
LIBPATHS += $(LIBPSL1GHT_LIB)
LIBS += -lrsx -lgcm_sys -lsysutil -lio -lm -llv2

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
