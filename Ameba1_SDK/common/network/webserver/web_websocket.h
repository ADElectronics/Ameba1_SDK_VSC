/******************************************************************************
 * FileName: web_websocket.h
 * Description: websocket for web ESP8266
 * Author: pvvx
 * (c) pvvx 2016
 * 2021.05 A_D fixes
*******************************************************************************/

#ifndef _WEB_WEBSOCKET_H_
#define _WEB_WEBSOCKET_H_

#include "webserver_config.h"

#ifdef WEBSOCKET_EN

#include "websock.h"

err_t websock_tx_close_err(TCP_SERV_CONN *ts_conn, uint32_t err);
bool websock_rx_data(TCP_SERV_CONN *ts_conn);
err_t websock_tx_frame(TCP_SERV_CONN *ts_conn, uint32_t opcode, uint8_t *raw_data, uint32_t raw_len);

#endif // WEBSOCKET_EN
#endif // _WEB_WEBSOCKET_H_
