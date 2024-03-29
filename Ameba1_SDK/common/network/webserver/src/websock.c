/******************************************************************************
 * FileName: websock.c
 * Description: websocket for web ESP8266
 * Author: PV`
 * (c) PV` 2016
*******************************************************************************/

#include "webserver_config.h"

#ifdef WEBSOCKET_EN
#include "autoconf.h"
#include "FreeRTOS.h"
#include "task.h"
#include "diag.h"
#include "tcpsrv.h"
#include "web_utils.h" // base64encode()
#include "web_srv.h"
#include "device_lock.h"
#include "lwip/tcp.h"
#include "websock.h"
#include "hal_crypto.h"

// HTTP/1.1 101 Web Socket Protocol Handshake\r\n
const uint8_t WebSocketHTTPOkKey[] = "HTTP/1.1 101 Switching Protocols\r\nAccess-Control-Allow-Origin: *\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: %s\r\n\r\n";
const uint8_t WebSocketAddKey[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
const uint8_t *HTTPUpgrade = "Upgrade:";
const uint8_t *HTTPwebsocket = "websocket";
const uint8_t *HTTPSecWebSocketKey = "Sec-WebSocket-Key:";

//=============================================================================
// WebSocketAcceptKey()
// 1) взять строковое значение из заголовка Sec-WebSocket-Key и объединить со
//	строкой 258EAFA5-E914-47DA-95CA-C5AB0DC85B11
// 2) вычислить бинарный хеш SHA-1 (бинарная строка из 20 символов) от полученной
//	в первом пункте строки
// 3) закодировать хеш в Base64
// skey[24+36]:'dGhlIHNhbXBsZSBub25jZQ==258EAFA5-E914-47DA-95CA-C5AB0DC85B11'
// cha:'b37a4f2cc0624f1690f64606cf385945b2bec4ea'
// key[28]:'s3pPLMBiTxaQ9kYGzzhZRbK+xOo='
//=============================================================================
uint8_t buff[maxsizeWebSocketKey + sizeWebSocketAddKey]; // [68]

bool WebSocketAcceptKey(uint8_t* dkey, uint8_t* skey)
{
	int len = 0;
	bool ret = FALSE;
	uint8_t keybuf[CRYPTO_SHA1_DIGEST_LENGTH];
	uint8_t * buff = malloc(maxsizeWebSocketKey + sizeWebSocketAddKey);
	if(buff)
   {
		while(skey[len] >= '+' && len < maxsizeWebSocketKey)
      {
			buff[len] = skey[len];
			len++;
		};
		if(len > minsizeWebSocketKey) {
			rtl_memcpy(&buff[len], WebSocketAddKey, sizeWebSocketAddKey+1);
			device_mutex_lock(RT_DEV_LOCK_CRYPTO);
         // TODO: переделать на стандартное API...
			//if(g_rtl_cipherEngine.isInit != 1)
         //{
#if WEBSERVER_DEBUG_EN > 1
			//	info_printf("(crypto on!) ");
#endif
			//	rtl_cryptoEngine_init();
			//}
			rtl_crypto_sha1(buff, len + sizeWebSocketAddKey, keybuf);
//			rtl_cryptoEngine_info();
			device_mutex_unlock(RT_DEV_LOCK_CRYPTO);
			len = base64encode(dkey, FileNameSize, keybuf, CRYPTO_SHA1_DIGEST_LENGTH);
#if WEBSERVER_DEBUG_EN > 2
			os_printf("\ncha:'");
			print_hex_dump(keybuf, CRYPTO_SHA1_DIGEST_LENGTH, '\0');
			os_printf("'\n");
			os_printf("key[%u]:'%s'\n", len, dkey);
#endif
		   	ret = TRUE;
		}
		free(buff);
	}
	return ret;

}
//=============================================================================
// websock_mask() размаскирование блока
//=============================================================================
void WebsocketMask(WS_FRSTAT *ws, uint8_t *raw_data, uint32_t raw_len)
{
	uint32_t i, x = ws->cur_len;
#if WEBSERVER_DEBUG_EN > 3
	os_printf("mask[%u]%u ", raw_len, x);
#endif
    for (i = 0; i < raw_len; i++)
    {
        raw_data[i] ^= ws->mask.uc[x++ & 3];
    }
}
//=============================================================================
// websock_head() разбор заголовка
//=============================================================================
uint32_t WebsocketHead(WS_FRSTAT *ws, uint8_t *raw_data, uint32_t raw_len)
{
	// определить размер заголовка фрейма
	uint32_t head_len = 2;
	if(raw_len < head_len) return 0; // докачивать
	uint32_t data_len = raw_data[1] & WS_SIZE1_BITS;
	if(data_len == 127) head_len = 10;
	else if(data_len == 126) head_len = 4;
	if(raw_data[1] & WS_MASK_FLG) head_len += 4;
	if(raw_len < head_len) return 0; // докачивать
	ws->head_len = head_len;
	ws->cur_len = 0;
	ws->flg = 0;
	data_len = raw_data[1] & WS_SIZE1_BITS;
	if(data_len >= 126)
   {
		if(data_len == 127)
      {
			for(uint32_t i = 3; i < 6; i++)
         {
				if(raw_data[i] != 0)
            {
					ws->status = sw_frs_close;
					ws->frame_len = 0;
					return WS_CLOSE_MESSAGE_TOO_BIG;
				}
			}
			data_len = (raw_data[6] << 24) | (raw_data[7] << 16) | (raw_data[8] << 8) | raw_data[9];
		}
		else
      {
			data_len = (raw_data[2] << 8) | raw_data[3];
		}
	}
	if(raw_data[1] & WS_MASK_FLG)
   {
		ws->flg |= WS_FLG_MASK;
		ws->mask.uc[0] = raw_data[head_len-4];
		ws->mask.uc[1] = raw_data[head_len-3];
		ws->mask.uc[2] = raw_data[head_len-2];
		ws->mask.uc[3] = raw_data[head_len-1];
	}
//	else ws->mask = 0;
	uint8_t opcode = raw_data[0] & WS_OPCODE_BITS;
	switch(opcode)
   {
		case WS_OPCODE_PING: // эхо - дублировать прием
			raw_data[0] &= ~WS_FRAGMENT_FIN;
			raw_data[0] |= WS_OPCODE_PONG;
			ws->status = sw_frs_pong;
			ws->frame_len = data_len;
			break;
		case WS_OPCODE_PONG: // эхо - дублировать прием
			ws->status = sw_frs_ping;
			ws->frame_len = data_len;
			break;
		case WS_OPCODE_CONTINUE: // продолжить
			if(ws->status == sw_frs_pong)
         {
				ws->frame_len = data_len;
				break;
			}
			else ws->frame_len += data_len;
			break;
		case WS_OPCODE_CLOSE: //
			ws->status = sw_frs_close;
			ws->frame_len = data_len;
			break;
		case WS_OPCODE_TEXT:
			ws->status = sw_frs_text;
			ws->frame_len = data_len;
			break;
		case WS_OPCODE_BINARY:
			ws->status = sw_frs_binary;
			ws->frame_len = data_len;
			break;
		default:
			ws->status = sw_frs_close;
			ws->frame_len = 0;
			return WS_CLOSE_WRONG_TYPE;
	}
//	uint32_t len = mMIN(raw_len - head_len, data_len);
//	if((ws->flg & WS_FLG_MASK) != 0) websock_mask(ws, &raw_data[head_len], mMIN(raw_len - head_len, len));
//	ws->cur_len += len;
	if((raw_data[0] & WS_FRAGMENT_FIN) != 0) { // конец - данные на обработку
		ws->flg |= WS_FLG_FIN;
	}
#if WEBSERVER_DEBUG_EN > 1
	os_printf("ws#%02xrx[%u] ", raw_data[0], data_len);
#endif
	return 1;
/*
	if(data_len + head_len <= raw_len) { // весь пакет уже в буфере?
		return 1;
	}
	return 0; // докачивать */
}
//=============================================================================
// websock_tx_frame() - передача фрейма
//=============================================================================
err_t WebsocketTxFrame(TCP_SERV_CONN *ts_conn, uint32_t opcode, uint8_t *raw_data, uint32_t raw_len)
{
	union
   {
		uint8_t uc[8];
		uint16_t uw[4];
		uint32_t ud[2];
	} head;

	union
   {
		uint8_t uc[4];
		uint16_t uw[2];
		uint32_t ud;
	} mask;

	if(raw_data == NULL) raw_len = 0;
	head.ud[0] = opcode;
	uint32_t head_len;
	if(raw_len > 126)
   {
		head.uc[1] = 126;
		head.uc[2] = raw_len>>8;
		head.uc[3] = raw_len;
		head_len = 4;
	}
	else
   {
		head.uc[1] = raw_len;
		head_len = 2;
	};

	if(opcode & (WS_MASK_FLG << 8))
   {
		mask.ud = rand();
		head.uc[1] |= WS_MASK_FLG;
		head.uc[head_len] = mask.uc[0];
		head.uc[head_len+1] = mask.uc[1];
		head.uc[head_len+2] = mask.uc[2];
		head.uc[head_len+3] = mask.uc[3];
		head_len += 4;
	}

	uint32_t len = tcp_sndbuf(ts_conn->pcb);
	err_t err = 1; // ERR_BUF;
	if(len >= raw_len + head_len)
   {
#if WEBSERVER_DEBUG_EN > 1
		os_printf("ws#%02xtx[%u] ", head.uc[0], raw_len);
#endif
		ts_conn->flag.nagle_disabled = 0;
		tcp_nagle_disable(ts_conn->pcb);
		err = tcpsrv_int_sent_data(ts_conn, head.uc, head_len);
		ts_conn->flag.nagle_disabled = 1;
		if(err == ERR_OK && raw_len != 0)
      {
			if(opcode & (WS_MASK_FLG << 8))
         {
				uint32_t i;
			    for (i = 0; i < raw_len; i++)
             {
			        raw_data[i] ^= mask.uc[i & 3];
			    }
			}
			err = tcpsrv_int_sent_data(ts_conn, raw_data, raw_len);
		}
	}
#if WEBSERVER_DEBUG_EN > 1
	else
		os_printf("ws#tx err! ");
#endif
	return err;
}

#endif // WEBSOCKET_EN
