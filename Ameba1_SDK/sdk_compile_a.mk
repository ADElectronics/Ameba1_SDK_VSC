# ---------------------------------------------------- Generate obj list ----------------------------------------------------

SRC_O = $(patsubst %.c,%.o,$(SRC_USER_C))
SRC_O += $(patsubst %.c,%.o,$(patsubst sdk/%,%,$(SRC_C)))
DRAM_O = $(patsubst %.c,%.o,$(patsubst sdk/%,%,$(DRAM_C)))
BOOT_O = $(patsubst %.c,%.o,$(patsubst sdk/%,%,$(BOOT_C)))

OBJ_LIST = $(addprefix $(OBJ_DIR)/,$(patsubst %.c,%.o,$(patsubst sdk/%,%,$(SRC_C))))
OBJ_LIST += $(addprefix $(OBJ_DIR)/,$(patsubst %.c,%.o,$(patsubst sdk/%,%,$(DRAM_C))))
OBJ_LIST += $(addprefix $(OBJ_DIR)/,$(patsubst %.c,%.o,$(patsubst sdk/%,%,$(BOOT_C))))
OBJ_LIST += $(addprefix $(OBJ_DIR)/,$(patsubst %.c,%.o,$(SRC_USER_C)))

DEPENDENCY_LIST = $(patsubst %.c,%.d,$(patsubst sdk/%,$(OBJ_DIR)/%,$(SRC_C)))
DEPENDENCY_LIST += $(patsubst %.c,%.d,$(patsubst sdk/%,$(OBJ_DIR)/%,$(DRAM_C)))
DEPENDENCY_LIST += $(patsubst %.c,%.d,$(patsubst sdk/%,$(OBJ_DIR)/%,$(BOOT_C)))
DEPENDENCY_LIST += $(patsubst %.c,%.d,$(SRC_USER_C))

# ------------------------------------------------------------ Target ------------------------------------------------------------

all: manipulate_images
mp: manipulate_images

.PHONY: link prerequirement manipulate_images build_info clean

TARGET = build

OBJ_DIR = $(TARGET)/obj
BIN_DIR = $(TARGET)/bin
ELF_FILE = $(BIN_DIR)/$(TARGET).axf
ASM_FILE = $(BIN_DIR)/$(TARGET).asm
IMAGE_TOOL = rtlaimage.py

# ---------------------------------------------------- Compile ----------------------------------------------------

link: $(SRC_O) $(DRAM_O) $(BOOT_O)
	@echo Link ============================================================
	@$(file > $(OBJ_DIR)/obj_list.lst, $(OBJ_LIST))
#	@$(LD) $(LFLAGS) -o $(BIN_DIR)/$(TARGET).axf $(OBJ_LIST) $(LIBFLAGS) $(OBJ_DIR)/ram_1.r.o -T$(LDFILE)
	@$(LD) $(LFLAGS) -o $(BIN_DIR)/$(TARGET).axf $(OBJ_LIST) $(LIBFLAGS) -T$(LDFILE)
	@$(OBJDUMP) -d $(ELF_FILE) > $(ASM_FILE)

$(SRC_O): %.o : %.c
	@echo $<
	@mkdir -p $(OBJ_DIR)/$(dir $@)
	@$(CC) $(CFLAGS) $(INCFLAGS) -c $< -o $(OBJ_DIR)/$@
	@$(CC) -MM $(CFLAGS) $(INCFLAGS) $< -MT $@ -MF $(OBJ_DIR)/$(patsubst %.o,%.d,$@)

$(DRAM_O): %.o : %.c
	@echo $<
	@mkdir -p $(OBJ_DIR)/$(dir $@)
	@$(CC) $(CFLAGS) $(INCFLAGS) -c $< -o $(OBJ_DIR)/$@
	@$(OBJCOPY) --prefix-alloc-sections .sdram $(OBJ_DIR)/$@
	@$(CC) -MM $(CFLAGS) $(INCFLAGS) $< -MT $@ -MF $(OBJ_DIR)/$(patsubst %.o,%.d,$@)

$(BOOT_O): %.o : %.c
	@echo $<
	@mkdir -p $(OBJ_DIR)/$(dir $@)
	@$(CC) $(CFLAGS) $(INCFLAGS) -c $< -o $(OBJ_DIR)/$@
	@$(OBJCOPY) --prefix-alloc-sections .boot $(OBJ_DIR)/$@
	@$(CC) -MM $(CFLAGS) $(INCFLAGS) $< -MT $@ -MF $(OBJ_DIR)/$(patsubst %.o,%.d,$@)

-include $(DEPENDENCY_LIST)
VPATH:=$(OBJ_DIR) $(AMEBA1_SDK_PATH)


# ---------------------------------------------------- Prerequirement ---------------------------------------------
prerequirement:
#	@echo Current dir: ${CURDIR}
	@echo Build ===========================================================
	mkdir -p $(OBJ_DIR)
	mkdir -p $(BIN_DIR)
#	cp -f "$(AMEBA1_SDK_PATH)soc/realtek/8195a/misc/bsp/image/ram_1.r.bin" "$(BIN_DIR)/ram_1.r.bin"
#	$(OBJCOPY) --rename-section .data=.loader.data,contents,alloc,load,readonly,data -I binary -O elf32-littlearm -B arm $(BIN_DIR)/ram_1.r.bin $(OBJ_DIR)/ram_1.r.o

# ---------------------------------------------------- Manipulate Image ----------------------------------------------------

manipulate_images: build_info prerequirement link
	@echo Image manipulating ==============================================
#	$(NM) $(BIN_DIR)/$(TARGET).axf | sort > $(BIN_DIR)/$(TARGET).nmap
	$(PYTHON) $(AMEBA1_SDK_PATH)$(IMAGE_TOOL) -a -r -o $(BIN_DIR)/ $(ELF_FILE)

#	$(OBJCOPY) -j .image2.start.table -j .ram_image2.text -j .ram_image2.rodata -j .ram.data -Obinary $(BIN_DIR)/$(TARGET).axf $(BIN_DIR)/ram_2.bin
#	$(OBJCOPY) -j .sdr_text -j .sdr_rodata -j .sdr_data -Obinary $(BIN_DIR)/$(TARGET).axf $(BIN_DIR)/sdram.bin

#	cp -f "$(AMEBA1_SDK_PATH)soc/realtek/8195a/misc/bsp/image/ram_1.p.bin" "$(BIN_DIR)/ram_1.p.bin"

#	$(PICK) 0x`grep __ram_image2_text_start__ $(BIN_DIR)/$(TARGET).nmap | gawk '{print $$1}'` 0x`grep __ram_image2_text_end__ $(BIN_DIR)/$(TARGET).nmap | gawk '{print $$1}'` $(BIN_DIR)/ram_2.bin $(BIN_DIR)/ram_2.p.bin body+reset_offset+sig
#	$(PICK) 0x`grep __ram_image2_text_start__ $(BIN_DIR)/$(TARGET).nmap | gawk '{print $$1}'` 0x`grep __ram_image2_text_end__ $(BIN_DIR)/$(TARGET).nmap | gawk '{print $$1}'` $(BIN_DIR)/ram_2.bin $(BIN_DIR)/ram_2.ns.bin body+reset_offset
#	$(PICK) 0x`grep __sdram_data_start__ $(BIN_DIR)/$(TARGET).nmap | gawk '{print $$1}'` 0x`grep __sdram_data_end__ $(BIN_DIR)/$(TARGET).nmap | gawk '{print $$1}'` $(BIN_DIR)/sdram.bin $(BIN_DIR)/ram_3.p.bin body+reset_offset
#	$(PAD) 44k 0xFF $(BIN_DIR)/ram_1.p.bin
#	cat $(BIN_DIR)/ram_1.p.bin > $(BIN_DIR)/$(RAMALL_BIN)

#	cat $(BIN_DIR)/ram_2.p.bin >> $(BIN_DIR)/$(RAMALL_BIN)
#	if [ -s $(BIN_DIR)/sdram.bin ]; then cat $(BIN_DIR)/ram_3.p.bin >> $(BIN_DIR)/$(RAMALL_BIN); fi
#	cat $(BIN_DIR)/ram_2.ns.bin > $(BIN_DIR)/$(OTA_BIN)

#	if [ -s $(BIN_DIR)/sdram.bin ]; then cat $(BIN_DIR)/ram_3.p.bin >> $(BIN_DIR)/$(OTA_BIN); fi
	@echo Image size ======================================================
	$(SZ) $(BIN_DIR)/$(TARGET).axf
#	$(CHKSUM) $(BIN_DIR)/$(OTA_BIN) || true
#	rm $(BIN_DIR)/ram_*.p.bin $(BIN_DIR)/ram_*.ns.bin

# ---------------------------------------------------- Generate build info ----------------------------------------------------

build_info:
	@echo Generate Build Info =============================================
	@echo \#define UTS_VERSION \"`date +%Y/%m/%d-%T`\" > .ver
	@echo \#define RTL8195AFW_COMPILE_TIME \"`date +%Y/%m/%d-%T`\" >> .ver
	@echo \#define RTL8195AFW_COMPILE_DATE \"`date +%Y%m%d`\" >> .ver
	@echo \#define RTL8195AFW_COMPILE_BY \"`id -u -n`\" >> .ver
	@echo \#define RTL8195AFW_COMPILE_HOST \"`$(HOSTNAME_APP)`\" >> .ver
	@if [ -x /bin/dnsdomainname ]; then \
		echo \#define RTL8195AFW_COMPILE_DOMAIN \"`dnsdomainname`\"; \
	elif [ -x /bin/domainname ]; then \
		echo \#define RTL8195AFW_COMPILE_DOMAIN \"`domainname`\"; \
	else \
		echo \#define RTL8195AFW_COMPILE_DOMAIN ; \
	fi >> .ver

	@echo \#define RTL195AFW_COMPILER \"gcc `$(CC) $(CFLAGS) -dumpversion | tr -d '\r'`\" >> .ver
	@mv -f ".ver" "Inc/$@.h"

# ---------------------------------------------------- Clean ----------------------------------------------------

clean:
	@echo Clean ===========================================================
	@rm -rf $(TARGET)
	@rm -f $(SRC_O) $(DRAM_O) $(BOOT_O)
	@rm -f $(patsubst %.o,%.d,$(SRC_O)) $(patsubst %.o,%.d,$(DRAM_O)) $(patsubst %.o,%.d,$(BOOT_O))
