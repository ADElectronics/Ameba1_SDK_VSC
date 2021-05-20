
# ---------------------------------------------------- Include folder list ----------------------------------------------------


INCLUDES += sdk/common/application
INCLUDES += sdk/common/application/iotdemokit
INCLUDES += sdk/common/application/google
INCLUDES += sdk/common/media/framework
INCLUDES += sdk/common/example
INCLUDES += sdk/common/example/wlan_fast_connect
INCLUDES += sdk/common/mbed/api
INCLUDES += sdk/common/mbed/hal
INCLUDES += sdk/common/mbed/hal_ext
INCLUDES += sdk/common/mbed/targets/hal/rtl8195a
INCLUDES += sdk/common/file_system
INCLUDES += sdk/common/test
INCLUDES += sdk/common/network/ssl/polarssl-1.3.8/include
INCLUDES += sdk/common/network/ssl/mbedtls-2.4.0/include
INCLUDES += sdk/common/network/ssl/ssl_ram_map/rom
INCLUDES += sdk/common/utilities
INCLUDES += sdk/common/application/apple/WACServer/External/Curve25519
INCLUDES += sdk/common/application/apple/WACServer/External/GladmanAES
INCLUDES += sdk/common/video/v4l2/inc
INCLUDES += sdk/common/media/rtp_codec
INCLUDES += sdk/common/file_system/fatfs
INCLUDES += sdk/common/file_system/fatfs/r0.10c/include
INCLUDES += sdk/common/drivers/sdio/realtek/sdio_host/inc
INCLUDES += sdk/common/audio
INCLUDES += sdk/common/application/xmodem
INCLUDES += sdk/common/application/mqtt/MQTTClient

ifeq ($(APP_WEBSERVER), 1)
INCLUDES += sdk/common/network/webserver
endif

# ---------------------------------------------------- Source file list ----------------------------------------------------

#console
#SRC_C += sdk/common/api/at_cmd/atcmd_cloud.c
SRC_C += sdk/common/api/at_cmd/atcmd_ethernet.c
#SRC_C += sdk/common/api/at_cmd/atcmd_lwip.c
SRC_C += sdk/common/api/at_cmd/atcmd_sys.c
SRC_C += sdk/common/api/at_cmd/atcmd_wifi.c
SRC_C += sdk/common/api/at_cmd/log_service.c

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
SRC_C += sdk/soc/realtek/8195a/misc/platform/ota_8195a.c

#network - webserver by pvvx
ifeq ($(APP_WEBSERVER), 1)
SRC_C += sdk/common/network/webserver/src/flash_utils.c
SRC_C += sdk/common/network/webserver/src/tcpsrv.c
SRC_C += sdk/common/network/webserver/src/web_auth.c
SRC_C += sdk/common/network/webserver/src/web_srv.c
SRC_C += sdk/common/network/webserver/src/web_utils.c
SRC_C += sdk/common/network/webserver/src/web_websocket.c
SRC_C += sdk/common/network/webserver/src/webfs.c
SRC_C += sdk/common/network/webserver/src/websock.c
endif

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
SRC_C += sdk/common/mbed/targets/hal/rtl8195a/analogin_api.c
SRC_C += sdk/common/mbed/targets/hal/rtl8195a/dma_api.c
SRC_C += sdk/common/mbed/targets/hal/rtl8195a/efuse_api.c
SRC_C += sdk/common/mbed/targets/hal/rtl8195a/ethernet_api.c
SRC_C += sdk/common/drivers/ethernet_mii/ethernet_mii.c
SRC_C += sdk/common/mbed/targets/hal/rtl8195a/flash_api.c
SRC_C += sdk/common/mbed/targets/hal/rtl8195a/gpio_api.c
SRC_C += sdk/common/mbed/targets/hal/rtl8195a/gpio_irq_api.c
SRC_C += sdk/common/mbed/targets/hal/rtl8195a/i2c_api.c
SRC_C += sdk/common/mbed/targets/hal/rtl8195a/i2s_api.c
SRC_C += sdk/common/mbed/targets/hal/rtl8195a/log_uart_api.c
SRC_C += sdk/common/mbed/targets/hal/rtl8195a/nfc_api.c
SRC_C += sdk/common/mbed/targets/hal/rtl8195a/pinmap.c
SRC_C += sdk/common/mbed/targets/hal/rtl8195a/pinmap_common.c
SRC_C += sdk/common/mbed/targets/hal/rtl8195a/port_api.c
SRC_C += sdk/common/mbed/targets/hal/rtl8195a/pwmout_api.c
SRC_C += sdk/common/mbed/targets/hal/rtl8195a/rtc_api.c
SRC_C += sdk/common/mbed/targets/hal/rtl8195a/serial_api.c
SRC_C += sdk/common/mbed/targets/hal/rtl8195a/sleep.c
SRC_C += sdk/common/mbed/targets/hal/rtl8195a/spdio_api.c
SRC_C += sdk/common/mbed/targets/hal/rtl8195a/spi_api.c
SRC_C += sdk/common/mbed/targets/hal/rtl8195a/sys_api.c
SRC_C += sdk/common/mbed/targets/hal/rtl8195a/timer_api.c
SRC_C += sdk/common/mbed/targets/hal/rtl8195a/us_ticker.c
SRC_C += sdk/common/mbed/common/us_ticker_api.c
SRC_C += sdk/common/mbed/common/wait_api.c
SRC_C += sdk/common/mbed/targets/hal/rtl8195a/wdt_api.c

#SDRAM
DRAM_C += sdk/common/api/platform/stdlib_patch.c

#SDRAM - polarssl
ifeq ($(APP_POLAR_SSL), 1)
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/aes.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/aesni.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/arc4.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/asn1parse.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/asn1write.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/base64.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/blowfish.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/camellia.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/ccm.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/certs.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/cipher.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/cipher_wrap.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/ctr_drbg.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/debug.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/des.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/dhm.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/ecp.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/ecp_curves.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/ecdh.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/ecdsa.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/entropy.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/entropy_poll.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/error.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/gcm.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/havege.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/hmac_drbg.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/md.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/md_wrap.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/md2.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/md4.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/md5.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/memory_buffer_alloc.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/net.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/oid.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/padlock.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/pbkdf2.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/pem.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/pkcs5.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/pkcs11.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/pkcs12.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/pk.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/pk_wrap.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/pkparse.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/pkwrite.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/platform.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/ripemd160.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/rsa.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/sha1.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/sha256.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/sha512.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/ssl_cache.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/ssl_ciphersuites.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/ssl_cli.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/ssl_srv.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/ssl_tls.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/threading.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/timing.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/version.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/version_features.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/x509.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/x509_crt.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/x509_crl.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/x509_csr.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/x509_create.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/x509write_crt.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/x509write_csr.c
DRAM_C += sdk/common/network/ssl/polarssl-1.3.8/library/xtea.c

SRC_C += sdk/common/network/ssl/polarssl-1.3.8/library/bignum.c
endif

#SDRAM - mbedtls
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/aes.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/aesni.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/arc4.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/asn1parse.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/asn1write.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/base64.c
#SRC_C += sdk/common/network/ssl/mbedtls-2.4.0/library/bignum.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/blowfish.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/camellia.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/ccm.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/certs.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/cipher.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/cipher_wrap.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/ctr_drbg.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/debug.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/des.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/dhm.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/ecp.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/ecp_curves.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/ecdh.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/ecdsa.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/entropy.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/entropy_poll.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/error.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/gcm.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/havege.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/hmac_drbg.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/md.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/md_wrap.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/md2.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/md4.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/md5.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/memory_buffer_alloc.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/net_sockets.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/oid.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/padlock.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/pem.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/pkcs5.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/pkcs11.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/pkcs12.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/pk.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/pk_wrap.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/pkparse.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/pkwrite.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/platform.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/ripemd160.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/rsa.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/sha1.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/sha256.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/sha512.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/ssl_cache.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/ssl_ciphersuites.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/ssl_cli.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/ssl_srv.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/ssl_tls.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/threading.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/timing.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/version.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/version_features.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/x509.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/x509_crt.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/x509_crl.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/x509_csr.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/x509_create.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/x509write_crt.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/x509write_csr.c
#DRAM_C += sdk/common/network/ssl/mbedtls-2.4.0/library/xtea.c

#SDRAM - ssl_ram_map
ifeq ($(APP_POLAR_SSL), 1)
DRAM_C += sdk/common/network/ssl/ssl_ram_map/rom/rom_ssl_ram_map.c
DRAM_C += sdk/common/network/ssl/ssl_ram_map/ssl_ram_map.c
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
SRC_C += sdk/common/example/ota_http/example_ota_http.c
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

DRAM_C += sdk/common/example/uart_atcmd/example_uart_atcmd.c

# simple examples by A_D
SRC_C += sdk/common/example/udp_send/example_udp_send.c

# LwIP examples by A_D
SRC_C += sdk/common/example/lwip_app_mdns/example_lwip_app_mdns.c
SRC_C += sdk/common/example/lwip_app_mqtt/example_lwip_app_mqtt.c
endif

#utilities - xmodem update
SRC_C += sdk/common/application/xmodem/uart_fw_update.c
