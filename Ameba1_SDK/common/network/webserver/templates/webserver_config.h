#ifndef _WEBSERVER_CONFIG_
#define _WEBSERVER_CONFIG_

//#define SYS_VERSION "1.0.1"
#define WEBSERVER_PORT        80
#define WEBSOCKET_EN          1
#define WEBSERVER_SNTP_EN     1
#define WEBSERVER_NETBIOS_EN  1
// дополнительные средства отладки
#define WEBSERVER_DEBUG_EN    0//1

// пока выкинут syscfg функционал, поэтому некоторые настройки тут
#define WEBSERVER_PCB_WAIT    1U
#define WEBSERVER_WAIT_REC    5U
#define WEBSERVER_WAIT_CLS    5U

#endif // _WEBSERVER_CONFIG_
