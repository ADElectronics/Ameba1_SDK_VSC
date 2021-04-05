
# ---------------------------------------------------- Include folder list ----------------------------------------------------

INCLUDES =
INCLUDES += sdk/soc/realtek/8711b/app/monitor/include
INCLUDES += sdk/soc/realtek/8711b/cmsis
INCLUDES += sdk/soc/realtek/8711b/cmsis/device
INCLUDES += sdk/soc/realtek/8711b/fwlib
INCLUDES += sdk/soc/realtek/8711b/fwlib/include
INCLUDES += sdk/soc/realtek/8711b/swlib/os_dep/include
INCLUDES += sdk/soc/realtek/8711b/swlib/std_lib/include
INCLUDES += sdk/soc/realtek/8711b/swlib/std_lib/libc/rom/string
INCLUDES += sdk/soc/realtek/8711b/swlib/rtl_lib
INCLUDES += sdk/soc/realtek/8711b/misc
INCLUDES += sdk/common/api
INCLUDES += sdk/common/api/at_cmd
INCLUDES += sdk/common/api/platform
INCLUDES += sdk/common/api/network/include
INCLUDES += sdk/common/api/wifi
INCLUDES += sdk/common/api/wifi/rtw_wpa_supplicant/src
INCLUDES += sdk/common/api/wifi/rtw_wowlan
INCLUDES += sdk/common/mbed/api
INCLUDES += sdk/common/mbed/hal
INCLUDES += sdk/common/mbed/hal_ext
#INCLUDES += sdk/common/mbed/targets/cmsis/rtl8711b
INCLUDES += sdk/common/mbed/targets/hal/rtl8711b
INCLUDES += sdk/common/file_system
INCLUDES += sdk/common/drivers/wlan/realtek/include
INCLUDES += sdk/common/drivers/wlan/realtek/src/osdep
INCLUDES += sdk/common/drivers/wlan/realtek/wlan_ram_map/rom
INCLUDES += sdk/os/os_dep/include
INCLUDES += sdk/os/freertos
INCLUDES += sdk/os/freertos/freertos_v$(FREERTOS_VERSION)/Source/include
INCLUDES += sdk/os/freertos/freertos_v$(FREERTOS_VERSION)/Source/portable/GCC/ARM_CM4F
INCLUDES += sdk/common/network
INCLUDES += sdk/common/network/websocket
INCLUDES += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/port/realtek/freertos
INCLUDES += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/include
INCLUDES += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/include/lwip
INCLUDES += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/src/include/ipv4
INCLUDES += sdk/common/network/lwip/lwip_v$(LWIP_VERSION)/port/realtek

# ---------------------------------------------------- Source file list ----------------------------------------------------

SRC_C =
DRAM_C =
BOOT_C =

# boot
SRC_C += sdk/soc/realtek/8711b/cmsis/device/app_start.c
SRC_C += sdk/soc/realtek/8711b/fwlib/ram_lib/startup.c

# CMSIS
SRC_C += sdk/soc/realtek/8711b/cmsis/device/system_8195a.c

# 
SRC_C += sdk/common/api/lwip_netconf.c
SRC_C += sdk/common/drivers/wlan/realtek/src/osdep/lwip_intf.c
SRC_C += sdk/common/network/dhcp/dhcps.c
#SRC_C += sdk/common/network/sntp/sntp.c

#console
SRC_C += sdk/common/api/at_cmd/atcmd_lwip.c
SRC_C += sdk/common/api/at_cmd/atcmd_sys.c
SRC_C += sdk/common/api/at_cmd/atcmd_wifi.c
SRC_C += sdk/common/api/at_cmd/log_service.c
SRC_C += sdk/soc/realtek/8711b/app/monitor/ram/low_level_io.c
SRC_C += sdk/soc/realtek/8711b/app/monitor/ram/monitor.c
SRC_C += sdk/soc/realtek/8711b/app/monitor/ram/rtl_consol.c
SRC_C += sdk/soc/realtek/8711b/app/monitor/ram/rtl_trace.c

#os - osdep
SRC_C += sdk/os/os_dep/device_lock.c
SRC_C += sdk/os/os_dep/osdep_service.c

#os - freertos
#SRC_C += sdk/os/freertos/freertos_v$(FREERTOS_VERSION)/Source/portable/Common/mpu_wrappers.c
SRC_C += sdk/os/freertos/freertos_v$(FREERTOS_VERSION)/Source/portable/MemMang/heap_5.c
SRC_C += sdk/os/freertos/freertos_v$(FREERTOS_VERSION)/Source/portable/GCC/ARM_CM4F/port.c
SRC_C += sdk/os/freertos/freertos_v$(FREERTOS_VERSION)/Source/croutine.c
SRC_C += sdk/os/freertos/freertos_v$(FREERTOS_VERSION)/Source/event_groups.c
SRC_C += sdk/os/freertos/freertos_v$(FREERTOS_VERSION)/Source/list.c
SRC_C += sdk/os/freertos/freertos_v$(FREERTOS_VERSION)/Source/queue.c
SRC_C += sdk/os/freertos/freertos_v$(FREERTOS_VERSION)/Source/tasks.c
SRC_C += sdk/os/freertos/freertos_v$(FREERTOS_VERSION)/Source/timers.c
SRC_C += sdk/os/freertos/freertos_service.c
SRC_C += sdk/os/freertos/cmsis_os.c

# ram_lib
SRC_C += sdk/soc/realtek/8711b/misc/rtl8710b_ota.c
SRC_C += sdk/soc/realtek/8711b/fwlib/ram_lib/rtl8710b_dsleepcfg.c
SRC_C += sdk/soc/realtek/8711b/fwlib/ram_lib/rtl8710b_dstandbycfg.c
SRC_C += sdk/soc/realtek/8711b/fwlib/ram_lib/rtl8710b_intfcfg.c
SRC_C += sdk/soc/realtek/8711b/fwlib/ram_lib/rtl8710b_pinmapcfg.c
SRC_C += sdk/soc/realtek/8711b/fwlib/ram_lib/rtl8710b_sleepcfg.c

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

# network api
SRC_C += sdk/common/api/wifi/rtw_wpa_supplicant/wpa_supplicant/wifi_eap_config.c
SRC_C += sdk/common/api/wifi/rtw_wpa_supplicant/wpa_supplicant/wifi_p2p_config.c
SRC_C += sdk/common/api/wifi/rtw_wpa_supplicant/wpa_supplicant/wifi_wps_config.c
SRC_C += sdk/common/api/wifi/wifi_conf.c
SRC_C += sdk/common/api/wifi/wifi_ind.c
SRC_C += sdk/common/api/wifi/wifi_promisc.c
SRC_C += sdk/common/api/wifi/wifi_simple_config.c
SRC_C += sdk/common/api/wifi/wifi_util.c
