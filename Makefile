
MAP_FILE = 0

DUMP_FILE = 0

DEBUG = 0

TARGET = serial

CC = gcc

BUILD_DIR = build

CFLAGS = $(C_INCLUDES) $(OPT) -Wall -Wextra -pedantic

OPT = -O2

BIN = objcopy -O binary -S

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif

#Produce .map file
ifeq ($(MAP_FILE), 1)
LDFLAG = -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections
LDFLAG += -ffunction-sections -fdata-sections -fno-common
#The below line is from SPC560 makefile
#LDFLAGS   = $(MCFLAGS) $(OPT) -nostartfiles $(LLIBDIR) -Wl,-Map=$(BUILDDIR)/$(PROJECT).map,--cref,--no-warn-mismatch,--library-path=$
endif



# include the file include for build process
C_INCLUDES = \
-Iinclude \
# -Imy_library # add for header file

#The line below search .c file recursively in the src file 
#$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
rwildcard = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
C_SOURCES = $(call rwildcard,./src/,*.c)
# C_SOURCES += $(wildcard ./my_library/*.c)

# OBJECTS = $(C_SOURCES: .c = .o)
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:%.c=%.o)))
vpath %.c $(sort $(dir $(C_SOURCES))) 

# $(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
# 	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

#Generate dependency file
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).dmp

$(BUILD_DIR)/$(TARGET).dmp: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).bin
	objdump -x --syms -r $< > $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS)
	$(CC) $^ $(LDFLAG) -o $@
	@echo
	size $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	

#pattern rules
$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

#suppresses error if any file named clean exists
.PHONY: clean

clean:
	rm -rf build .depend

$(BUILD_DIR):
	mkdir $@
	
# depend: .depend

# .depend: $(C_SOURCES)
# 	$(CC) $(CFLAGS) -MM $^ > "$@"

# include .depend

-include $(wildcard $(BUILD_DIR)/*.d)