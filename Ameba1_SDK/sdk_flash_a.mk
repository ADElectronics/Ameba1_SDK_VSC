
# ---------------------------------------------------- Flash ----------------------------------------------------

.PHONY: flash_all run_in_ram

flash_all:
	@echo Flash All =======================================================
	@echo define call1>$(JLINK_TOOLS_PATH)/flash_file_info.jlink
	@echo SetFirwareSize $(BIN_DIR)/ram_all.bin>>$(JLINK_TOOLS_PATH)/flash_file_info.jlink
	@echo end>>$(JLINK_TOOLS_PATH)/flash_file_info.jlink
	@echo define call2>>$(JLINK_TOOLS_PATH)/flash_file_info.jlink
	@echo FlasherWrite $(BIN_DIR)/ram_all.bin 0 '$$'Image1Size>>$(JLINK_TOOLS_PATH)/flash_file_info.jlink
	@echo end>>$(JLINK_TOOLS_PATH)/flash_file_info.jlink
	@echo define call3>>$(JLINK_TOOLS_PATH)/flash_file_info.jlink
	@echo FlasherWrite $(BIN_DIR)/ram_all.bin '$$'Image2Addr '$$'Image2Size>>$(JLINK_TOOLS_PATH)/flash_file_info.jlink
	@echo end>>$(JLINK_TOOLS_PATH)/flash_file_info.jlink
	@$(GDB) -q -ex "set pagination off" -x $(JLINK_TOOLS_PATH)/gdb_wrflash.jlink

# ---------------------------------------------------- RAM ----------------------------------------------------

run_in_ram:
	@$(JLINK_PATH)$(JLINK_CONSOLE) -Device Cortex-M3 -jtagconf -1,-1 -If $(JLINK_INTERFACE) -Speed $(JLINK_SPEED) $(JLINK_TOOLS_PATH)RTL_RunInRAM.JLinkScript
