# Makefile для PSL1GHT / ps3dev
# Требует: ppu-gcc, PSL1GHT, tiny3D, libfont
ifeq ($(strip $(PSL1GHT)),)
$(error "PSL1GHT not set. Установи ps3dev или запусти через docker ps3dev/ps3dev")
endif

include $(PSL1GHT)/ppu_rules

TITLE       := STALKER PS3 Demo Extended
APPID       := STALKERD01
CONTENTID   := UP0001-$(APPID)_00-0000000000000000

SOURCES     := source
DATA        := data
INCLUDES    := include
SHADERS     :=

CFILES      := $(foreach dir,$(SOURCES), $(wildcard $(dir)/*.c))
CPPFILES    := $(foreach dir,$(SOURCES), $(wildcard $(dir)/*.cpp))
SFILES      := $(foreach dir,$(SOURCES), $(wildcard $(dir)/*.s))
BINFILES    := $(foreach dir,$(DATA), $(wildcard $(dir)/*.bin))

OBJS        := $(CFILES:.c=.o) $(CPPFILES:.cpp=.o) $(SFILES:.s=.o) $(BINFILES:.bin=.o)

CFLAGS      := -O2 -Wall -mcpu=cell $(MACHDEP) $(LIBPSL1GHT_INC) -I$(PORTLIBS)/include -I$(INCLUDES)
CXXFLAGS    := $(CFLAGS) -std=c++11
LDFLAGS     := -L$(PORTLIBS)/lib -L$(PSL1GHT)/ppu/lib -L$(PS3DEV)/ppu/lib
LIBS        := -ltiny3d -lfont -lfreetype -lgcm_sys -lrsx -lsysutil -lio -lsysmodule -lm

TARGET      := $(APPID)
ELF         := $(TARGET).elf
SELF        := EBOOT.BIN
PKG         := stalker_demo.pkg

all: $(SELF) pkg

$(ELF): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS) $(LIBS)

$(SELF): $(ELF)
	$(SELFTOOL) --sfo-app-version 01.00 --sfo-title "$(TITLE)" -e $< $@

pkg: $(SELF)
	@mkdir -p pkgfiles/USRDIR
	@cp $(SELF) pkgfiles/USRDIR/EBOOT.BIN
	@cp pkgfiles/ICON0.PNG pkgfiles/ 2>/dev/null || true
	@cp pkgfiles/PARAM.SFO pkgfiles/ 2>/dev/null || true
	@echo "Building PKG $(PKG)..."
	@$(PS3DEV)/bin/pkg.py --contentid $(CONTENTID) pkgfiles/ $(PKG) || \
	 $(SELFTOOL) --pkg pkgfiles $(PKG) || \
	 echo "pkg.py not found, используй make_self_npdrm"
	@echo "PKG готов: $(PKG)"

clean:
	rm -f $(OBJS) $(ELF) $(SELF) $(PKG)
	rm -rf pkgfiles/USRDIR/EBOOT.BIN

run: $(SELF)
	ps3load $(SELF)

.PHONY: all clean pkg run
