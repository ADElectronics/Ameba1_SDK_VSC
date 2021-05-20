
# ---------------------------------------------------- Include folder list ----------------------------------------------------

INCLUDES =
INCLUDES += sdk/soc/realtek/common/bsp
INCLUDES += sdk/soc/realtek/8195a/misc/driver
INCLUDES += sdk/soc/realtek/8195a/misc/os
INCLUDES += sdk/soc/realtek/8195a/cmsis
INCLUDES += sdk/soc/realtek/8195a/cmsis/device
INCLUDES += sdk/soc/realtek/8195a/fwlib
INCLUDES += sdk/soc/realtek/8195a/fwlib/rtl8195a
INCLUDES += sdk/soc/realtek/8195a/misc/platform
INCLUDES += sdk/soc/realtek/8195a/misc/rtl_std_lib
INCLUDES += sdk/soc/realtek/8195a/misc/rtl_std_lib/include
INCLUDES += sdk/soc/realtek/8195a/fwlib/ram_lib/wlan/realtek/wlan_ram_map/rom
INCLUDES += sdk/soc/realtek/8195a/fwlib/ram_lib/usb_otg/include
INCLUDES += sdk/os/os_dep/include
INCLUDES += sdk/os/freertos
INCLUDES += sdk/os/freertos/freertos_v$(FREERTOS_VERSION)/Source/include
INCLUDES += sdk/os/freertos/freertos_v$(FREERTOS_VERSION)/Source/portable/GCC/ARM_CM3
INCLUDES += sdk/common/api/network/include
INCLUDES += sdk/common/api
INCLUDES += sdk/common/api/platform
INCLUDES += sdk/common/api/wifi
INCLUDES += sdk/common/api/wifi/rtw_wpa_supplicant/src
INCLUDES += sdk/common/network
INCLUDES += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/port/realtek
INCLUDES += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/port/realtek/freertos
INCLUDES += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/include
INCLUDES += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/include/lwip
INCLUDES += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/include/lwip/apps
INCLUDES += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/include/lwip/priv
INCLUDES += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/include/lwip/prot
INCLUDES += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/include/ipv4
INCLUDES += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/include/compat/posix
INCLUDES += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/include/compat/posix/arpa
INCLUDES += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/include/compat/posix/net
INCLUDES += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/include/compat/posix/sys
INCLUDES += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/include/compat/stdc
INCLUDES += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/include/netif
INCLUDES += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/include/netif/ppp
INCLUDES += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/include/netif/ppp/polarssl
INCLUDES += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/apps/altcp_tls
INCLUDES += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/apps/http
INCLUDES += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/apps/snmp
INCLUDES += sdk/common/drivers/wlan/realtek/include
INCLUDES += sdk/common/drivers/wlan/realtek/src/osdep
INCLUDES += sdk/common/drivers/wlan/realtek/src/hci
INCLUDES += sdk/common/drivers/wlan/realtek/src/hal
INCLUDES += sdk/common/drivers/wlan/realtek/src/hal/OUTSRC
INCLUDES += sdk/common/drivers/usb_class/host/uvc/inc
INCLUDES += sdk/common/drivers/usb_class/device
INCLUDES += sdk/common/drivers/usb_class/device/class
INCLUDES += sdk/common/drivers/i2s

# ---------------------------------------------------- Source file list ----------------------------------------------------

SRC_C =
DRAM_C =

# boot
ifeq ($(USE_AMEBA_STARTUP), 0)
SRC_C += sdk/soc/realtek/8195a/fwlib/ram_lib/boot/startup.c
endif
BOOT_C += sdk/soc/realtek/8195a/fwlib/ram_lib/boot/rtl_boot.c
SRC_C += sdk/soc/realtek/8195a/fwlib/ram_lib/boot/rtl_bios_data.c

#cmsis
#SRC_C += sdk/soc/realtek/8195a/cmsis/device/app_start.c
SRC_C += sdk/soc/realtek/8195a/cmsis/device/system_8195a.c

#console
#SRC_C += sdk/soc/realtek/8195a/misc/driver/low_level_io.c
SRC_C += sdk/soc/realtek/8195a/misc/driver/rtl_consol.c

#os - osdep
SRC_C += sdk/os/os_dep/device_lock.c
SRC_C += sdk/os/freertos/freertos_service.c
SRC_C += sdk/soc/realtek/8195a/misc/os/mailbox.c
SRC_C += sdk/soc/realtek/8195a/misc/os/osdep_api.c
SRC_C += sdk/os/os_dep/osdep_service.c
SRC_C += sdk/os/os_dep/tcm_heap.c

#peripheral - hal
SRC_C += sdk/soc/realtek/8195a/fwlib/src/hal_common.c
SRC_C += sdk/soc/realtek/8195a/fwlib/src/hal_dac.c
SRC_C += sdk/soc/realtek/8195a/fwlib/src/hal_32k.c
SRC_C += sdk/soc/realtek/8195a/fwlib/src/hal_adc.c
SRC_C += sdk/soc/realtek/8195a/fwlib/src/hal_gdma.c
SRC_C += sdk/soc/realtek/8195a/fwlib/src/hal_gpio.c
SRC_C += sdk/soc/realtek/8195a/fwlib/src/hal_i2c.c
SRC_C += sdk/soc/realtek/8195a/fwlib/src/hal_i2s.c
SRC_C += sdk/soc/realtek/8195a/fwlib/src/hal_mii.c
SRC_C += sdk/soc/realtek/8195a/fwlib/src/hal_nfc.c
SRC_C += sdk/soc/realtek/8195a/fwlib/src/hal_pcm.c
SRC_C += sdk/soc/realtek/8195a/fwlib/src/hal_pwm.c
SRC_C += sdk/soc/realtek/8195a/fwlib/src/hal_sdr_controller.c
SRC_C += sdk/soc/realtek/8195a/fwlib/src/hal_ssi.c
SRC_C += sdk/soc/realtek/8195a/fwlib/src/hal_timer.c
SRC_C += sdk/soc/realtek/8195a/fwlib/src/hal_uart.c
SRC_C += sdk/soc/realtek/8195a/fwlib/src/hal_soc_ps_monitor.c
SRC_C += sdk/soc/realtek/8195a/fwlib/src/hal_efuse.c
SRC_C += sdk/soc/realtek/8195a/fwlib/src/hal_log_uart.c
SRC_C += sdk/soc/realtek/8195a/fwlib/src/hal_pinmux.c
SRC_C += sdk/soc/realtek/8195a/fwlib/src/hal_misc.c
#SRC_C += sdk/soc/realtek/8195a/fwlib/src/hal_spi_flash_ram.c

#peripheral - rtl8195a
SRC_C += sdk/soc/realtek/8195a/fwlib/rtl8195a/src/rtl8195a_adc.c
SRC_C += sdk/soc/realtek/8195a/fwlib/rtl8195a/src/rtl8195a_gdma.c
SRC_C += sdk/soc/realtek/8195a/fwlib/rtl8195a/src/rtl8195a_gpio.c
SRC_C += sdk/soc/realtek/8195a/fwlib/rtl8195a/src/rtl8195a_i2c.c
SRC_C += sdk/soc/realtek/8195a/fwlib/rtl8195a/src/rtl8195a_i2s.c
SRC_C += sdk/soc/realtek/8195a/fwlib/rtl8195a/src/rtl8195a_mii.c
SRC_C += sdk/soc/realtek/8195a/fwlib/rtl8195a/src/rtl8195a_nfc.c
SRC_C += sdk/soc/realtek/8195a/fwlib/rtl8195a/src/rtl8195a_pwm.c
SRC_C += sdk/soc/realtek/8195a/fwlib/rtl8195a/src/rtl8195a_ssi.c
SRC_C += sdk/soc/realtek/8195a/fwlib/rtl8195a/src/rtl8195a_timer.c
SRC_C += sdk/soc/realtek/8195a/fwlib/rtl8195a/src/rtl8195a_uart.c

ifeq ($(USE_SDIOH), 1)
SRC_C += sdk/soc/realtek/8195a/fwlib/src/hal_sdio_host.c
SRC_C += sdk/common/drivers/sdio/realtek/sdio_host/src/sd.c 
SRC_C += sdk/common/drivers/sdio/realtek/sdio_host/src/sdio_host.c 
SRC_C += sdk/soc/realtek/8195a/fwlib/rtl8195a/src/rtl8195a_sdio_host_open.c 
endif

#os - freertos
SRC_C += sdk/soc/realtek/8195a/misc/platform/gcc_wrap.c
#SRC_C += sdk/os/freertos/freertos_v$(FREERTOS_VERSION)/Source/portable/Common/mpu_wrappers.c
SRC_C += sdk/os/freertos/freertos_v$(FREERTOS_VERSION)/Source/portable/MemMang/heap_5.c
SRC_C += sdk/os/freertos/freertos_v$(FREERTOS_VERSION)/Source/portable/GCC/ARM_CM3/port.c
SRC_C += sdk/os/freertos/freertos_v$(FREERTOS_VERSION)/Source/croutine.c
SRC_C += sdk/os/freertos/freertos_v$(FREERTOS_VERSION)/Source/event_groups.c
SRC_C += sdk/os/freertos/freertos_v$(FREERTOS_VERSION)/Source/list.c
SRC_C += sdk/os/freertos/freertos_v$(FREERTOS_VERSION)/Source/queue.c
SRC_C += sdk/os/freertos/freertos_v$(FREERTOS_VERSION)/Source/tasks.c
SRC_C += sdk/os/freertos/freertos_v$(FREERTOS_VERSION)/Source/timers.c
SRC_C += sdk/os/freertos/cmsis_os.c

#osdep
#SRC_C += sdk/os/freertos/freertos_pmu.c
SRC_C += sdk/soc/realtek/8195a/misc/os/freertos_pmu_8195a.c

# rtl_lib
SRC_C += sdk/soc/realtek/8195a/misc/rtl_std_lib/lib_rtlstd/ram_libc.c
#SRC_C += sdk/soc/realtek/8195a/misc/rtl_std_lib/lib_rtlstd/ram_pvvx_libc.c
SRC_C += sdk/soc/realtek/8195a/misc/rtl_std_lib/lib_rtlstd/ram_libgloss_retarget.c
SRC_C += sdk/soc/realtek/8195a/misc/rtl_std_lib/lib_rtlstd/rtl_eabi_cast_ram.c
SRC_C += sdk/soc/realtek/8195a/misc/rtl_std_lib/lib_rtlstd/rtl_math_ram.c

#network - lwip
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/api/api_lib.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/api/api_msg.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/api/err.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/api/netbuf.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/api/netdb.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/api/netifapi.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/api/sockets.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/api/tcpip.c

SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/core/ipv4/autoip.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/core/ipv4/icmp.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/core/ipv4/igmp.c

SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/core/def.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/core/dns.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/core/init.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/core/mem.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/core/memp.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/core/netif.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/core/pbuf.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/core/raw.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/core/stats.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/core/sys.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/core/tcp.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/core/tcp_in.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/core/tcp_out.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/core/udp.c

ifeq ($(LWIP_VERSION), 1.4.1)

SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/core/ipv4/inet.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/core/ipv4/inet_chksum.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/core/ipv4/ip.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/core/ipv4/ip_addr.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/core/ipv4/ip_frag.c

SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/core/dhcp.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/core/lwip_timers.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/etharp.c

SRC_C += sdk/common/network/sntp/sntp.c

else ifeq ($(LWIP_VERSION), 2.1.2)

SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/core/ip.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/api/if_api.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/core/ipv4/dhcp.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/core/ipv4/etharp.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/core/ipv4/ip4.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/core/ipv4/ip4_addr.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/core/ipv4/ip4_frag.c

SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/core/altcp.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/core/altcp_alloc.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/core/altcp_tcp.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/core/inet_chksum.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/core/timeouts.c

SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/bridgeif.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/bridgeif_fdb.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/ethernet.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/lowpan6.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/lowpan6_ble.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/lowpan6_common.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/slipif.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/zepif.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/ppp/auth.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/ppp/ccp.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/ppp/chap_ms.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/ppp/chap-md5.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/ppp/chap-new.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/ppp/demand.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/ppp/eap.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/ppp/ecp.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/ppp/eui64.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/ppp/fsm.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/ppp/ipcp.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/ppp/lcp.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/ppp/magic.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/ppp/mppe.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/ppp/multilink.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/ppp/ppp.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/ppp/pppapi.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/ppp/pppcrypt.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/ppp/pppoe.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/ppp/pppol2tp.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/ppp/pppos.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/ppp/upap.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/ppp/utils.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/ppp/vj.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/ppp/polarssl/arc4.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/ppp/polarssl/des.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/ppp/polarssl/md4.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/ppp/polarssl/md5.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/netif/ppp/polarssl/sha1.c

ifeq ($(LWIP_212_APPS), 1)
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/apps/altcp_tls/altcp_tls_mbedtls.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/apps/altcp_tls/altcp_tls_mbedtls_mem.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/apps/lwiperf/lwiperf.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/apps/mdns/mdns.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/apps/mqtt/mqtt.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/apps/netbiosns/netbiosns.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/apps/smtp/smtp.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/apps/sntp/sntp.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/apps/tftp/tftp_server.c
endif
endif

SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/port/realtek/freertos/ethernetif.c
SRC_C += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/port/realtek/freertos/sys_arch.c

SRC_C += sdk/common/drivers/wlan/realtek/src/osdep/lwip_intf.c
SRC_C += sdk/common/network/dhcp/dhcps.c

#network - api
SRC_C += sdk/common/api/wifi/rtw_wpa_supplicant/src/crypto/tls_polarssl.c
SRC_C += sdk/common/api/wifi/rtw_wpa_supplicant/wpa_supplicant/wifi_eap_config.c
SRC_C += sdk/common/api/wifi/rtw_wpa_supplicant/wpa_supplicant/wifi_p2p_config.c
SRC_C += sdk/common/api/wifi/rtw_wpa_supplicant/wpa_supplicant/wifi_wps_config.c
SRC_C += sdk/common/api/wifi/wifi_conf.c
SRC_C += sdk/common/api/wifi/wifi_ind.c
SRC_C += sdk/common/api/wifi/wifi_promisc.c
SRC_C += sdk/common/api/wifi/wifi_simple_config.c
SRC_C += sdk/common/api/wifi/wifi_util.c
SRC_C += sdk/common/api/lwip_netconf.c

#peripheral - wlan
#all:SRC_C += sdk/common/drivers/wlan/realtek/src/core/option/rtw_opt_skbuf.c
