.SUFFIXES:
ifeq ($(strip $(PSL1GHT)),)
$(error "Please set PSL1GHT in your environment. export PSL1GHT=<path>")
endif
include $(PSL1GHT)/ppu_rules
TARGET		:=	STALKERD01
BUILD		:=	build
SOURCES		:=	source
DATA		:=
INCLUDES	:=	include
TITLE		:=	STALKER PS3 Custom Engine
APPID		:=	STALKERD01
CONTENTID	:=	UP0001-$(APPID)_00-0000000000000000
CFLAGS		=	-O2 -Wall -mcpu=cell $(MACHDEP) $(INCLUDE)
CXXFLAGS	=	$(CFLAGS) -std=c++11
LDFLAGS		=	$(MACHDEP) -Wl,-Map,$(notdir $@).map
LIBS	:=	-lrsx -lgcm_sys -lio -lsysutil -lm
LIBDIRS	:=
ifneq ($(BUILD),$(notdir $(CURDIR)))
export OUTPUT	:=	$(CURDIR)/$(TARGET)
export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) $(foreach dir,$(DATA),$(CURDIR)/$(dir))
export DEPSDIR	:=	$(CURDIR)/$(BUILD)
export BUILDDIR	:=	$(CURDIR)/$(BUILD)
CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
sFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.S)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))
ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
endif
export OFILES	:=	$(addsuffix .o,$(BINFILES)) $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(sFILES:.s=.o) $(SFILES:.S=.o)
export INCLUDE	:=	$(foreach dir,$(INCLUDES), -I$(CURDIR)/$(dir)) $(foreach dir,$(LIBDIRS),-I$(dir)/include) $(LIBPSL1GHT_INC) -I$(CURDIR)/$(BUILD)
export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib) $(LIBPSL1GHT_LIB)
export OUTPUT	:=	$(CURDIR)/$(TARGET)
.PHONY: $(BUILD) clean
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
clean:
	@rm -fr $(BUILD) $(OUTPUT).elf $(OUTPUT).self $(OUTPUT).pkg
pkg: $(BUILD) $(OUTPUT).pkg
else
DEPENDS	:=	$(OFILES:.o=.d)
$(OUTPUT).self: $(OUTPUT).elf
$(OUTPUT).elf:	$(OFILES)
%.bin.o	:	%.bin
	@echo $(notdir $<)
	@$(bin2o)
-include $(DEPENDS)
endif
