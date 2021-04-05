
# ---------------------------------------------------- Debug ----------------------------------------------------

.PHONY: debug_via_swd debug_in_ram

debug_via_swd:
	@echo Debug - SWD =====================================================

debug_in_ram:
	@echo Debug - RAM =====================================================
	$(GDB) -q -ex "set pagination off" -x $(FLASH_TOOLDIR)rtl_gdb_ramdebug_jlink.txt

start_gdb_server_m3:
	@echo Debug - Run GDB Server M3 =======================================
	@cmd /c "start $(JLINK_GDB) -select USB -device Cortex-M3 -if $(JLINK_INTERFACE) -speed $(JLINK_SPEED) -ir"

