# ====================================== Build Commands ====================================== #
all: ram_all
.PHONY: ram_all mp clean

ram_all:
	@$(MAKE) -f app.mk

mp:
	@$(MAKE) -f app.mk mp

clean:
	@$(MAKE) -f app.mk clean

# ====================================== Flash Commands ====================================== #
.PHONY: flash_read flash_boot flash_write_ota1 flash_write_ota2

flash_read:
	@$(MAKE) -f app.mk flash_read

flash_boot:
	@$(MAKE) -f app.mk flash_boot

flash_write_ota1:
	@$(MAKE) -f app.mk flash_write_ota1

flash_write_ota2:
	@$(MAKE) -f app.mk flash_write_ota2

# ====================================== Debug Commands ====================================== #
.PHONY: start_gdb_server_m4 run_in_ram

run_in_ram:
	@$(MAKE) -f app.mk run_in_ram

start_gdb_server_m4:
	@$(MAKE) -f app.mk start_gdb_server_m4
