
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
.PHONY:  flash_all

flash_all:
	@$(MAKE) -f app.mk flash_all

# ====================================== Debug Commands ====================================== #
.PHONY: start_gdb_server_m3 run_in_ram

run_in_ram:
	@$(MAKE) -f app.mk run_in_ram

start_gdb_server_m3:
	@$(MAKE) -f app.mk start_gdb_server_m3

