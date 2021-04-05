
# ---------------------------------------------------- Flash ----------------------------------------------------

GDB_FLASH_FILE = JLink/rtl_gdb_writefile.txt

.PHONY: flash_boot flash_read flash_write_ota1 flash_write_ota2

flash_read:
	@echo Flash - Read =====================================================
	$(JLINK_CONSOLE) -device Cortex-M4 -if $(JLINK_INTERFACE) -speed $(JLINK_SPEED) JLink\RTL_FlashRead.JLinkScript

flash_boot:
	@echo Flash - Boot =====================================================
	@echo 'set $$ImageType = 0x01'> $(GDB_FLASH_FILE)
	@echo 'set $$StartAddr = 0x00'>> $(GDB_FLASH_FILE)
	@echo 'set $$FileSize = '`stat -L -c %s $(BIN_DIR)/$(BOOT_ALL)`>> $(GDB_FLASH_FILE)
	$(GDB) -q -x JLink\rtl_gdb_flash_write.txt

flash_system:
	@echo Flash - System ===================================================
	@echo 'set $$ImageType = 0x02'> $(GDB_FLASH_FILE)
	@echo 'set $$StartAddr = 0x9000'>> $(GDB_FLASH_FILE)
	@echo 'set $$FileSize = 32'>> $(GDB_FLASH_FILE)
	$(GDB) -q -x JLink\rtl_gdb_flash_write.txt

flash_write_ota1:
	@echo Flash - Write OTA1 ===============================================
	@echo 'set $$ImageType = 0x03'> $(GDB_FLASH_FILE)
	@echo 'set $$StartAddr = 0xB000'>> $(GDB_FLASH_FILE)
	@echo 'set $$FileSize = '`stat -L -c %s $(BIN_DIR)/$(IMAGE2_OTA1) `>> $(GDB_FLASH_FILE)
	$(GDB) -q -x JLink\rtl_gdb_flash_write.txt

flash_write_ota2:
	@echo Flash - Write OTA2 ===============================================
	@echo 'set $$ImageType = 0x04'> $(GDB_FLASH_FILE)
	@echo 'set $$StartAddr = 0x80000'>> $(GDB_FLASH_FILE)
	@echo 'set $$FileSize = '`stat -L -c %s $(BIN_DIR)/$(IMAGE2_OTA2)`>> $(GDB_FLASH_FILE)
	$(GDB) -q -x JLink\rtl_gdb_flash_write.txt
