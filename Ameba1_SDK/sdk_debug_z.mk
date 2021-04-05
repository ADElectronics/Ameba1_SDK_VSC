
# ---------------------------------------------------- Debug ----------------------------------------------------

.PHONY: run_in_ram start_gdb_server_m4

run_in_ram:
	@echo Debug - RAM =====================================================
	#$(GDB) -q -x JLink\rtl_gdb_ramdebug_jlink.txt
	#$(JLINK_CONSOLE) -device Cortex-M4 -if $(JLINK_INTERFACE) -speed $(JLINK_SPEED) JLink\RTL_RunInRAM.JLinkScript

start_gdb_server_m4:
	@echo Debug - Run GDB Server M4 =======================================
	@cmd /c "start $(JLINK_GDB) -select USB -device Cortex-M4 -if $(JLINK_INTERFACE) -speed $(JLINK_SPEED) -ir"

