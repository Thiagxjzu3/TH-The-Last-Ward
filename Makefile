
TARGET      := THTheLastWard
BUILD       := build
SOURCES     := source
INCLUDES    := include

CFILES      :=
CPPFILES    := $(foreach dir,$(SOURCES),$(wildcard $(dir)/*.cpp))
AFILES      :=

OBJS        := $(CPPFILES:.cpp=.o)
OBJS        += $(CFILES:.c=.o)

LIBS        :=

CXXFLAGS    := -O2 -Wall -Wextra
CFLAGS      := -O2 -Wall -Wextra
LDFLAGS     :=

include $(PSL1GHT)/ppu_rules

