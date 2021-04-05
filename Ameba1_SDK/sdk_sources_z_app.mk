
# ---------------------------------------------------- Include folder list ----------------------------------------------------

INCLUDES += sdk/common/test
INCLUDES += sdk/common/utilities
INCLUDES += sdk/common/network/ssl/polarssl-1.3.8/include
INCLUDES += sdk/common/network/ssl/ssl_ram_map/rom
INCLUDES += sdk/common/example
INCLUDES += sdk/common/example/wlan_fast_connect
INCLUDES += sdk/common/application
INCLUDES += sdk/common/application/mqtt/MQTTClient
INCLUDES += sdk/common/application/mqtt/MQTTPacket

# ---------------------------------------------------- Source file list ----------------------------------------------------

#network - app
ifeq ($(APP_MQTT), 1)
SRC_C += sdk/common/application/mqtt/MQTTClient/MQTTClient.c
SRC_C += sdk/common/application/mqtt/MQTTPacket/MQTTConnectClient.c
SRC_C += sdk/common/application/mqtt/MQTTPacket/MQTTConnectServer.c
SRC_C += sdk/common/application/mqtt/MQTTPacket/MQTTDeserializePublish.c
SRC_C += sdk/common/application/mqtt/MQTTPacket/MQTTFormat.c
SRC_C += sdk/common/application/mqtt/MQTTClient/MQTTFreertos.c
SRC_C += sdk/common/application/mqtt/MQTTPacket/MQTTPacket.c
SRC_C += sdk/common/application/mqtt/MQTTPacket/MQTTSerializePublish.c
SRC_C += sdk/common/application/mqtt/MQTTPacket/MQTTSubscribeClient.c
SRC_C += sdk/common/application/mqtt/MQTTPacket/MQTTSubscribeServer.c
SRC_C += sdk/common/application/mqtt/MQTTPacket/MQTTUnsubscribeClient.c
SRC_C += sdk/common/application/mqtt/MQTTPacket/MQTTUnsubscribeServer.c
endif

SRC_C += sdk/common/api/network/src/ping_test.c
SRC_C += sdk/common/utilities/ssl_client.c
SRC_C += sdk/common/utilities/ssl_client_ext.c
SRC_C += sdk/common/utilities/tcptest.c
SRC_C += sdk/common/application/uart_adapter/uart_adapter.c
SRC_C += sdk/common/api/network/src/wlan_network.c

#network - httpc
ifeq ($(APP_HTTPC), 1)
SRC_C += sdk/common/network/httpc/httpc_tls.c
endif

#network - httpd
ifeq ($(APP_HTTPD), 1)
SRC_C += sdk/common/network/httpd/httpd_tls.c
endif

#network - mdns
ifeq ($(APP_MDNS), 1)
SRC_C += sdk/common/network/mDNS/mDNSPlatform.c
endif

#network - wsclient
ifeq ($(APP_WSCLIENT), 1)
SRC_C += sdk/common/network/websocket/wsclient_tls.c
endif

#peripheral - mbed api
SRC_C += sdk/common/mbed/targets/hal/rtl8711b/analogin_api.c
SRC_C += sdk/common/mbed/targets/hal/rtl8711b/dma_api.c
SRC_C += sdk/common/mbed/targets/hal/rtl8711b/efuse_api.c
SRC_C += sdk/common/mbed/targets/hal/rtl8711b/flash_api.c
SRC_C += sdk/common/mbed/targets/hal/rtl8711b/gpio_api.c
SRC_C += sdk/common/mbed/targets/hal/rtl8711b/gpio_irq_api.c
SRC_C += sdk/common/mbed/targets/hal/rtl8711b/i2c_api.c
SRC_C += sdk/common/mbed/targets/hal/rtl8711b/i2s_api.c
SRC_C += sdk/common/mbed/targets/hal/rtl8711b/nfc_api.c
SRC_C += sdk/common/mbed/targets/hal/rtl8711b/pinmap.c
SRC_C += sdk/common/mbed/targets/hal/rtl8711b/pinmap_common.c
SRC_C += sdk/common/mbed/targets/hal/rtl8711b/port_api.c
SRC_C += sdk/common/mbed/targets/hal/rtl8711b/pwmout_api.c
SRC_C += sdk/common/mbed/targets/hal/rtl8711b/rtc_api.c
SRC_C += sdk/common/mbed/targets/hal/rtl8711b/serial_api.c
SRC_C += sdk/common/mbed/targets/hal/rtl8711b/sleep.c
SRC_C += sdk/common/mbed/targets/hal/rtl8711b/spi_api.c
SRC_C += sdk/common/mbed/targets/hal/rtl8711b/sys_api.c
SRC_C += sdk/common/mbed/targets/hal/rtl8711b/timer_api.c
SRC_C += sdk/common/mbed/targets/hal/rtl8711b/us_ticker.c
SRC_C += sdk/common/mbed/targets/hal/rtl8711b/us_ticker_api.c
SRC_C += sdk/common/mbed/targets/hal/rtl8711b/wait_api.c
SRC_C += sdk/common/mbed/targets/hal/rtl8711b/wdt_api.c

#SDRAM - polarssl
ifeq ($(APP_POLAR_SSL), 1)
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/aesni.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/blowfish.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/camellia.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/ccm.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/certs.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/cipher.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/cipher_wrap.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/debug.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/ecp_ram.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/entropy.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/entropy_poll.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/error.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/gcm.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/havege.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/md2.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/md4.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/memory_buffer_alloc.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/net.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/padlock.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/pbkdf2.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/pkcs11.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/pkcs12.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/pkcs5.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/pkparse.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/platform.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/ripemd160.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/ssl_cache.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/ssl_ciphersuites.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/ssl_cli.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/ssl_srv.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/ssl_tls.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/threading.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/timing.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/version.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/version_features.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/x509.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/x509_create.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/x509_crl.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/x509_crt.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/x509_csr.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/x509write_crt.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/x509write_csr.c
SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/xtea.c

# ssl_ram_map
SRC_C += sdk/common/network/ssl/ssl_ram_map/ssl_ram_map.c
endif

#utilities
SRC_C += sdk/common/utilities/cJSON.c
SRC_C += sdk/common/utilities/http_client.c
SRC_C += sdk/common/utilities/uart_socket.c
SRC_C += sdk/common/utilities/webserver.c
SRC_C += sdk/common/utilities/xml.c

#utilities - example
ifeq ($(APP_SDK_EXAMPLES), 1)
SRC_C += sdk/common/example/example_entry.c
SRC_C += sdk/common/example/mqtt/example_mqtt.c
SRC_C += sdk/common/example/bcast/example_bcast.c
SRC_C += sdk/common/example/eap/example_eap.c
SRC_C += sdk/common/example/dct/example_dct.c
SRC_C += sdk/common/example/get_beacon_frame/example_get_beacon_frame.c
SRC_C += sdk/common/example/high_load_memory_use/example_high_load_memory_use.c
SRC_C += sdk/common/example/http_client/example_http_client.c
SRC_C += sdk/common/example/http_download/example_http_download.c
SRC_C += sdk/common/example/httpc/example_httpc.c
SRC_C += sdk/common/example/httpd/example_httpd.c
SRC_C += sdk/common/example/mcast/example_mcast.c
SRC_C += sdk/common/example/mdns/example_mdns.c
SRC_C += sdk/common/example/nonblock_connect/example_nonblock_connect.c
SRC_C += sdk/common/example/rarp/example_rarp.c
SRC_C += sdk/common/example/sntp_showtime/example_sntp_showtime.c
SRC_C += sdk/common/example/socket_select/example_socket_select.c
SRC_C += sdk/common/example/socket_tcp_trx/example_socket_tcp_trx_1.c
SRC_C += sdk/common/example/socket_tcp_trx/example_socket_tcp_trx_2.c
SRC_C += sdk/common/example/ssl_download/example_ssl_download.c
SRC_C += sdk/common/example/ssl_server/example_ssl_server.c
SRC_C += sdk/common/example/tcp_keepalive/example_tcp_keepalive.c
SRC_C += sdk/common/example/uart_firmware_update/example_uart_update.c
SRC_C += sdk/common/example/wifi_mac_monitor/example_wifi_mac_monitor.c
SRC_C += sdk/common/example/wlan_fast_connect/example_wlan_fast_connect.c
SRC_C += sdk/common/example/wlan_scenario/example_wlan_scenario.c
SRC_C += sdk/common/example/websocket/example_wsclient.c
SRC_C += sdk/common/example/xml/example_xml.c
SRC_C += sdk/common/example/uart_atcmd/example_uart_atcmd.c

# simple examples by A_D
SRC_C += sdk/common/example/udp_send/example_udp_send.c

# LwIP examples by A_D
SRC_C += sdk/common/example/lwip_app_mdns/example_lwip_app_mdns.c
SRC_C += sdk/common/example/lwip_app_mqtt/example_lwip_app_mqtt.c

endif
