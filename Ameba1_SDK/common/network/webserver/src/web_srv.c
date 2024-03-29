/******************************************************************************
 * FileName: webserver.c
 * Description: Small WEB server + WebSocket
 * Author: pvvx
 * ver1.0 25/12/2014  SDK 0.9.4
 * ver1.1 02/04/2015  SDK 1.0.0
 * ver2.0 14/14/2017  RTL871x
 * ver2.1 2021.05     A_D fixes
*******************************************************************************/

#include "webserver_config.h"

#ifdef WEBSERVER_PORT
#include "autoconf.h"
#include "FreeRTOS.h"
#include "task.h"
#include "diag.h"
#include "lwip/tcp.h"
#include "tcpsrv.h"
#include "web_srv_int.h"
#include "web_utils.h"
#include "flash_utils.h"
#include "device_lock.h"
#include "webfs.h"
//#include "wifi_api.h"

#ifdef WEBSOCKET_EN
#include "web_websocket.h"
#endif

extern int rom_atoi(const char *);
#undef atoi
#define atoi(s) rom_atoi(s)

#endif // WEBSERVER_PORT

#ifdef WEBSERVER_PORT

#define USE_WEB_NAGLE // https://en.wikipedia.org/wiki/Nagle%27s_algorithm
#define MIN_REQ_LEN  7  // Minimum length for a valid HTTP/0.9 request: "GET /\r\n" -> 7 bytes
#define CRLF "\r\n"

#define max_len_buf_write_flash 2048 // размер буфера при записи flash. Увеличение/уменньшение размера (до сектора 4096) ускорения не дает (1..2%)

static void web_print_headers(HTTP_CONN *CurHTTP, TCP_SERV_CONN *ts_conn) ;
//static void webserver_discon(void *arg);
//static void webserver_recon(void *arg, sint8 err);
static void webserver_send_fdata(TCP_SERV_CONN *ts_conn);
static void web_int_disconnect(TCP_SERV_CONN *ts_conn) ;
static bool webserver_open_file(HTTP_CONN *CurHTTP, TCP_SERV_CONN *ts_conn);
static bool webserver_file_ext(HTTP_CONN *CurHTTP, uint8_t *pfname);

const char http_default_file[] = "index.html";
const char web_cgi_fname[] = "web.cgi";
const char fsupload_fname[] = "fsupload";
#ifdef USE_CAPTDNS
const char ncsi_txt_fname[] = "ncsi.txt";
//const char generate_204_fname[] = "generate_204";
const char *HTTPHost ="Host:";
#define sizeHTTPHost 5
#endif
#define ProtectedFilesName    "protect"

#define MAX_NO_DATA_BUF_SIZE (8192) // if(ts_conn->sizei > MAX_NO_DATA_BUF_SIZE) CurHTTP->httpStatus = 418; // 418: Out of Coffee

QueueHandle_t xQueueWebSrv;

/****************************************************************************
  Section:
        Commands and Server Responses
  ***************************************************************************/
const char HTTPresponse_200_head[] = "OK";
const char HTTPresponse_302_head[] = "Found";
const char HTTPresponse_304_head[] = "Not Modified";
const char HTTPresponse_400_head[] = "Bad Request";
const char HTTPresponse_401_head[] = "Unauthorized\r\nWWW-Authenticate: Basic realm=\"Protected%u\"";
const char HTTPresponse_404_head[] = "Not found";
const char HTTPresponse_411_head[] = "Length Required";
const char HTTPresponse_413_head[] = "Request Entity Too Large";
const char HTTPresponse_414_head[] = "Request-URI Too Long";
const char HTTPresponse_418_head[] = "I'm a teapot";
const char HTTPresponse_429_head[] = "Too Many Requests\r\nRetry-After: 30";
const char HTTPresponse_500_head[] = "Internal Server Error";
const char HTTPresponse_501_head[] = "Not Implemented\r\nAllow: GET, POST";

const char HTTPresponse_401_content[] = "401 Unauthorized: Password required\r\n";
const char HTTPresponse_404_content[] = "404: File not found\r\n";
const char HTTPresponse_411_content[] = "411 The request must have a content length\r\n";
const char HTTPresponse_413_content[] = "413 Request Entity Too Large: There's too many letters :)\r\n";
const char HTTPresponse_414_content[] = "414 Request-URI Too Long: Buffer overflow detected\r\n";
const char HTTPresponse_418_content[] = "418: Out of Coffee\r\n";
const char HTTPresponse_500_content[] = "500 Internal Server Error\r\n";
const char HTTPresponse_501_content[] = "501 Not Implemented: Only GET and POST supported\r\n";

// Initial response strings (Corresponding to HTTP_STATUS)
static const HTTP_RESPONSE HTTPResponse[] =
{
   { 200, HTTP_RESP_FLG_NONE, HTTPresponse_200_head, NULL },
   // успешный запрос. Если клиентом были запрошены какие-либо данные, то они находятся в заголовке и/или теле сообщения.
   { 302, HTTP_RESP_FLG_NONE | HTTP_RESP_FLG_REDIRECT, HTTPresponse_302_head, NULL },
   // "HTTP/1.1 302 Found\r\nConnection: close\r\nLocation: ",
   // 302 Found, 302 Moved Temporarily - запрошенный документ временно
   // доступен по другому URI, указанному в заголовке в поле Location.
   // Этот код может быть использован, например, при управляемом сервером
   // согласовании содержимого. Некоторые клиенты некорректно ведут себя
   // при обработке данного кода.
   { 304, HTTP_RESP_FLG_NONE, HTTPresponse_304_head, NULL },
   ///"304 Redirect: ",   // If-Modified-Since If-None-Match
   // сервер возвращает такой код, если клиент запросил документ методом GET,
   // использовал заголовок If-Modified-Since или If-None-Match и документ
   // не изменился с указанного момента. При этом сообщение сервера не должно содержать тела.
   { 400, HTTP_RESP_FLG_FINDFILE, HTTPresponse_400_head, NULL} ,
   // сервер обнаружил в запросе клиента синтаксическую ошибку.
   { 401, HTTP_RESP_FLG_FINDFILE, HTTPresponse_401_head, HTTPresponse_401_content },
   // для доступа к запрашиваемому ресурсу требуется аутентификация.
   // В заголовке ответ должен содержать поле WWW-Authenticate с перечнем
   // условий аутентификации. Клиент может повторить запрос,
   // включив в заголовок сообщения поле Authorization с требуемыми для аутентификации данными.
   //"HTTP/1.1 403 Forbidden\r\nConnection: close\r\n\r\n403 Forbidden: SSL Required - use HTTPS\r\n"
   { 404, HTTP_RESP_FLG_FINDFILE, HTTPresponse_404_head, HTTPresponse_404_content },
   // Сервер понял запрос, но не нашёл соответствующего ресурса по указанному URI.
   { 411, HTTP_RESP_FLG_FINDFILE, HTTPresponse_411_head, HTTPresponse_411_content },
   // для указанного ресурса клиент должен указать Content-Length в заголовке запроса.
   // Без указания этого поля не стоит делать повторную попытку запроса к серверу по данному URI.
   // Такой ответ естественен для запросов типа POST и PUT.
   // Например, если по указанному URI производится загрузка файлов, а на сервере стоит
   // ограничение на их объём. Тогда разумней будет проверить в самом начале заголовок
   // Content-Length и сразу отказать в загрузке, чем провоцировать бессмысленную нагрузку,
   // разрывая соединение, когда клиент действительно пришлёт слишком объёмное сообщение.
   { 413, HTTP_RESP_FLG_FINDFILE, HTTPresponse_413_head, HTTPresponse_413_content },
   // возвращается в случае, если сервер отказывается обработать запрос
   // по причине слишком большого размера тела запроса. Сервер может закрыть соединение,
   // чтобы прекратить дальнейшую передачу запроса.
   { 414, HTTP_RESP_FLG_FINDFILE, HTTPresponse_414_head, HTTPresponse_414_content },
   // сервер не может обработать запрос из-за слишком длинного указанного URL.
   // Такую ошибку можно спровоцировать, например, когда клиент пытается передать длинные
   // параметры через метод GET, а не POST.
   { 429, HTTP_RESP_FLG_NONE, HTTPresponse_429_head, NULL },
   // клиент попытался отправить слишком много запросов за короткое время, что может указывать,
   // например, на попытку DoS-атаки. Может сопровождаться заголовком Retry-After, указывающим,
   // через какое время можно повторить запрос.
   { 501, HTTP_RESP_FLG_FINDFILE, HTTPresponse_501_head, HTTPresponse_501_content },
   // сервер не поддерживает возможностей, необходимых для обработки запроса.
   // Типичный ответ для случаев, когда сервер не понимает указанный в запросе метод. + см 405
   { 418, HTTP_RESP_FLG_FINDFILE, HTTPresponse_418_head, HTTPresponse_418_content },
   // http://en.wikipedia.org/wiki/Hyper_Text_Coffee_Pot_Control_Protocol
   { 500, HTTP_RESP_FLG_END, HTTPresponse_500_head, HTTPresponse_500_content }
   // любая внутренняя ошибка сервера, которая не входит в рамки остальных ошибок класса.
};
const char HTTPfsupload[] = "<html><body style='margin:100px'><form method='post' action='/fsupload' enctype='multipart/form-data'><b>File Upload</b><p><input type='file' name='file' size=40> <input type='submit' value='Upload'></form></body></html>";
#define sizeHTTPfsupload 220
const char HTTPdefault[] = "<html><h3>RTL871X Built-in Web server <sup><i>&copy</i></sup></h3></html>";
#define sizeHTTPdefault 73
const char HTTPfserror[] = "<html><h3>Web-disk error. Upload the WEBFiles.bin!</h3></html>";
#define sizeHTTPfserror 62

const char HTTPAccessControlAllowOrigin[] = "Access-Control-Allow-Origin: *\r\n";
const char *HTTPContentLength = "Content-Length:";
#define sizeHTTPContentLength 15
const char *HTTPContentType = "Content-Type:";
#define sizeHTTPContentType 13
const char *HTTPmultipartformdata = "multipart/form-data";
#define sizeHTTPmultipartformdata 19
const char *HTTPboundary = "boundary=";
#define sizeHTTPboundary 9
const char *HTTPAuthorization = "Authorization:";
#define sizeHTTPAuthorization 14
const char *HTTPCookie = "Cookie:";
#define sizeHTTPCookie 7

/******************************************************************************
 * FunctionName : Close_web_conn
 * Description  : Free  ts_conn
 * Parameters   : struct TCP_SERV_CONN
 * Returns      : none
*******************************************************************************/
static void Close_web_conn(TCP_SERV_CONN *ts_conn)
{
   WEB_SRV_CONN *web_conn = (WEB_SRV_CONN *)ts_conn->linkd;
   int i = 0;

   do
   {
      if(web_conn->bffiles[i] != WEBFS_INVALID_HANDLE)
      {
#if WEBSERVER_DEBUG_EN > 1
         os_printf("cf%d ", web_conn->bffiles[i]);
#endif
         if(web_conn->bffiles[i] <= WEBFS_MAX_HANDLE)
            webfs_Close(web_conn->bffiles[i]);
         web_conn->bffiles[i] = WEBFS_INVALID_HANDLE;
      };
      i++;
   } while(i < 4);
   ClrSCB(SCB_FOPEN | SCB_FGZIP | SCB_FCALBACK);
}

/******************************************************************************
 * FunctionName : ReNew_web_conn
 * Description  :
 * Parameters   : struct TCP_SERV_CONN
 * Returns      : none
*******************************************************************************/
static WEB_SRV_CONN * ReNew_web_conn(TCP_SERV_CONN *ts_conn)
{
   WEB_SRV_CONN *web_conn = (WEB_SRV_CONN *)ts_conn->linkd;
   if(web_conn == NULL)
   {
      web_conn = (WEB_SRV_CONN *)zalloc(sizeof(WEB_SRV_CONN));
      if(web_conn != NULL)
      {
         web_conn->bffiles[0] = WEBFS_INVALID_HANDLE;
         web_conn->bffiles[1] = WEBFS_INVALID_HANDLE;
         web_conn->bffiles[2] = WEBFS_INVALID_HANDLE;
         web_conn->bffiles[3] = WEBFS_INVALID_HANDLE;
         //  web_conn->webflag = 0; //zalloc
         //  web_conn->func_web_cb = NULL; //zalloc
         OpenSCB(); // сбросить флаги
         ts_conn->linkd = (void *)web_conn;
      }
   }
   return web_conn;
}

//=============================================================================
// Authorization: Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ==\n"
// The resulting string is then encoded using the RFC2045-MIME variant of Base64,
// except not limited to 76 uint8_t/line
// /ssl/crypto/ssl_crypto_misc.c:
// EXP_FUNC int STDCALL base64_decode(const uint8_t *in,  int len, uint8_t *out, int *outlen);
// Username and password are combined into a string "username:password"
static uint8_t CheckAuthorization(uint8_t* base64str)
{
   uint8_t *pcmp = base64str;
   uint8_t pbuf[77];
   int declen = 76;
   int len = 0;
   while(*pcmp++ >= '+') len++;

   if((len >= 4) && (len <= 128) &&(base64decode(base64str, len, pbuf, &declen)))
   {
      pbuf[declen]='\0';
#if WEBSERVER_DEBUG_EN > 1
      os_printf("'%s' ", pbuf);
#endif
      return web_auth(pbuf, declen);
   };
   return 0;
}

//=============================================================================

#define web_parse_cookie(CurHTTP, ts_conn) web_parse_vars(ts_conn, (CurHTTP)->pcookie, (CurHTTP)->cookie_len, '\0', ';')
#define web_parse_uri_vars(CurHTTP, ts_conn) web_parse_vars(ts_conn, (CurHTTP)->puri, (CurHTTP)->uri_len, '?', '&')
#define web_parse_content(CurHTTP, ts_conn) web_parse_vars(ts_conn, (CurHTTP)->pcontent, (CurHTTP)->content_len, '\0', '&')

static void web_parse_vars(TCP_SERV_CONN *ts_conn, uint8_t *vars, uint32_t vars_len, uint8_t start_char, uint8_t end_char)
{
   if(vars == NULL || vars_len == 0) return;
   uint8_t *pcmp;

   if(start_char)
   {
      pcmp = cmpcpystr(NULL, vars, '\0', start_char, vars_len); // find start_char if available
      start_char = '\0';
   }
   else
   {
      pcmp = vars - 1;
   }


   while(pcmp != NULL)
   {
      uint16_t len = vars_len - (pcmp - vars);
      uint8_t *pcmd = pcmp;
      pcmp = cmpcpystr(pcmp, pcmp + 1, start_char, '=', len); // skip spaces before variable name
      if(pcmp == NULL) break;
      urldecode(pcmd, pcmd, len, len);
      len = vars_len - (pcmp - vars);
      uint8_t *pvar = pcmp;
      pcmp = cmpcpystr(pcmp, pcmp + 1, '\0', end_char, len);
      if(pcmd[0] != '\0')
      {
         urldecode(pvar, pvar, len, len);
         web_int_vars(ts_conn, pcmd, pvar);
      }
   }
}
//=============================================================================
// Разбор имени файла и перевод в вид относительного URI.
// (выкидывание HTTP://Host)
// Проверка на обращение в папку или имя файла требующее пароль
//=============================================================================
static void web_parse_fname(HTTP_CONN *CurHTTP, TCP_SERV_CONN *ts_conn)
{
   if(CurHTTP->puri == NULL) return;
   if(CurHTTP->uri_len < 2) // = "/"?
   {
      CurHTTP->pFilename[0] = CurHTTP->puri[0];
      return;
   }

   uint8_t cbuf[FileNameSize+16];
   uint8_t *pcbuf = cbuf;
   urldecode(pcbuf, CurHTTP->puri, sizeof(cbuf) - 1, CurHTTP->uri_len);
   if(rom_xstrcmp((char *)pcbuf, "HTTP://")||(rom_xstrcmp((char *)pcbuf, "http://")))
   {
      pcbuf += 7;
      uint8_t *pcmp = strchr((char *)pcbuf, '/');
      if(pcmp != NULL) pcbuf = pcmp;
   }
   cmpcpystr(CurHTTP->pFilename, pcbuf, '\0', '?', FileNameSize);

   // Проверка на обращение в папку или имя файла требующее пароль
   uint8_t *pcmp = web_strnstr(CurHTTP->pFilename, ProtectedFilesName, strlen(CurHTTP->pFilename));
   if(pcmp != NULL)
   {
      WEB_SRV_CONN *web_conn = (WEB_SRV_CONN *)ts_conn->linkd;
#if USE_WEB_AUTH_LEVEL
      pcmp += sizeof(ProtectedFilesName) - 1;
      web_conn->auth_realm = atoi(pcmp) + 1;
      printf("[%s] ar%d ", pcmp, web_conn->auth_realm);
      //web_conn->auth_realm = WEB_AUTH_LEVEL_USER;
#endif
      SetSCB(SCB_AUTH);
   }
}
//=============================================================================
//=============================================================================
uint8_t * head_find_ctr(HTTP_CONN *CurHTTP, const uint8_t * c, int clen, int dlen)
{
   if(CurHTTP->head_len < clen + dlen + 2) return NULL; // + "\r\n"
   uint8_t * pstr = web_strnstr((char *)CurHTTP->phead, c, CurHTTP->head_len);
   if(pstr != NULL)
   {
      pstr += clen;
      uint8_t *pend = web_strnstr(pstr, CRLF, CurHTTP->phead + CurHTTP->head_len - pstr);
      if(pend == NULL)
      {
         CurHTTP->httpStatus = 400;
         return NULL;
      }
      while(*pstr == ' ' && pstr < pend) pstr++;
      if(pend - pstr < dlen)
      {
         CurHTTP->httpStatus = 400;
         return NULL;
      }
      }
   return pstr;
}
//=============================================================================
// Func: parse_header
// Разбирает докачан или нет заголовок HTTP, что там принято, GET или POST,
// открывает файл и проверяет content, если это POST и не прием файла.
//=============================================================================
static bool parse_header(HTTP_CONN *CurHTTP, TCP_SERV_CONN *ts_conn)
{
   WEB_SRV_CONN *web_conn = (WEB_SRV_CONN *)ts_conn->linkd;
   CurHTTP->httpStatus = 501; // 501 Not Implemented (not a GET or POST command)

   uint8_t *pstr = ts_conn->pbufi;
   uint8_t *pend = &ts_conn->pbufi[ts_conn->sizei];
   CurHTTP->pcontent = pend;

   if(pstr == NULL)
   {
      CurHTTP->httpStatus =  500; // 500 Internal Server Error
      return FALSE;
   }
   if(ts_conn->sizei < MIN_REQ_LEN) return FALSE; // 501 Not Implemented (not a GET or POST command)
   uint8_t *pnext = web_strnstr(pstr, CRLF, ts_conn->sizei); // "\r\n"
   if(pnext == NULL)
   {
      CurHTTP->httpStatus = 400; // 400 Bad Request
      return FALSE;
   }
   pnext += 2;
   if(pnext - pstr < MIN_REQ_LEN) return FALSE; // 501 размер строки запроса менее "GET /"
   if(strncmp(pstr, "GET ", 4) == 0)
   {
      SetSCB(SCB_GET);
      CurHTTP->httpStatus = 200;
      pstr += 4;
   }
   else if(strncmp(pstr, "POST ", 5) == 0)
   {
      SetSCB(SCB_POST);
      CurHTTP->httpStatus = 200;
      pstr += 5;
   }
   else return FALSE;  // 501 Not Implemented (not a GET or POST command)
   CurHTTP->puri = pstr;
   CurHTTP->uri_len = pnext - pstr;

   if(CurHTTP->uri_len > 10) // "/ HTTP/1.0\r\n"
   {
      pstr = web_strnstr(CurHTTP->puri, " HTTP/", CurHTTP->uri_len);
      if(pstr != NULL)
      {
         if((pstr[7] == '.')&&(pstr[6] <= '9')&&(pstr[6] >= '0')&&(pstr[8] >= '0')&&(pstr[8] <= '9'))
         CurHTTP->httpver = ((pstr[6]-'0')<<4) + pstr[8]-'0';
      }
   }
#if WEBSERVER_DEBUG_EN > 3
   os_printf("http_ver=%02x ", CurHTTP->httpver);
#endif
   if(CurHTTP->httpver < 0x10) // HTTP/0.9 ?
   {
      if(CheckSCB(SCB_POST))
      {
         CurHTTP->httpStatus = 400; // 400 HTTP/0.9 does not support POST
         return FALSE;
      }
   }
	// здесь уже надо глядеть - следует или нет докачивать данные
	pstr = web_strnstr(pnext-2, CRLF CRLF, pend - pnext + 2 ); // find "\r\n\r\n"
	if(pstr == NULL) return TRUE; // докачивать!
	// разбираем дальше Header, раз уже скачан
	pstr += 2;
	if(pstr != pnext) { // есть Headers
		CurHTTP->phead = pnext;
		CurHTTP->head_len = pstr - pnext;
		if(CheckSCB(SCB_POST)){
			pstr += 2;
			CurHTTP->pcontent = pstr;
			CurHTTP->content_len = pend - pstr;
		};
	};
	if(!CheckSCB(SCB_FOPEN)) { // файл уже открыт? нет
		web_parse_fname(CurHTTP, ts_conn);
		if(!webserver_open_file(CurHTTP, ts_conn)) {
			CurHTTP->httpStatus = 404; // "404: File not found"
			return FALSE; //
		};
	};
	if((CurHTTP->phead == NULL)||(CurHTTP->head_len == 0)) {
		// если требуется авторизация, но нет передачи пароля...
		if(CheckSCB(SCB_AUTH)) CurHTTP->httpStatus = 401; // 401 Unauthorized
		return FALSE; // нет Header
	};
	if(CheckSCB(SCB_POST)) {
    	pstr = head_find_ctr(CurHTTP, HTTPContentLength, sizeHTTPContentLength, 1);
    	if(pstr == NULL || CurHTTP->httpStatus != 200) {
    		CurHTTP->httpStatus = 411; // no "Content Length:", 411 Length Required
    		return FALSE;
    	}
        uint32_t cnlen = atoi(pstr);
#if WEBSERVER_DEBUG_EN > 1
        os_printf("content_len = %d of %d ", cnlen, CurHTTP->content_len);
#endif
        if(cnlen) {
    		web_conn->content_len = cnlen; // запомнить размер, для приема файла
        	if(!CheckSCB(SCB_BNDR) && (CurHTTP->head_len > sizeHTTPContentType + sizeHTTPmultipartformdata + sizeHTTPboundary + 2 + 2)) { //"x\r\n"
            	pstr = head_find_ctr(CurHTTP, HTTPContentType, sizeHTTPContentType, sizeHTTPmultipartformdata + sizeHTTPboundary + 2);
            	if(CurHTTP->httpStatus != 200) return FALSE;
            	if(pstr != NULL) {
            		pend = web_strnstr(pstr, CRLF, CurHTTP->phead + CurHTTP->head_len - pstr);
                    pstr = web_strnstr(pstr, HTTPmultipartformdata, pend - pstr);
                    if(pstr != NULL) {
                    	pstr += sizeHTTPmultipartformdata;
                        pstr = web_strnstr(pstr, HTTPboundary, pend - pstr);
                        if(pstr != NULL) {
                        	// сохраним этот "мультипаспорт" (с) 5-ый элемент :)
                   			pstr += sizeHTTPboundary;
                   			HTTP_UPLOAD *pupload = (HTTP_UPLOAD *)zalloc(sizeof(HTTP_UPLOAD));
                   			if(pupload == NULL) {
                   				CurHTTP->httpStatus =  500; // 500 Internal Server Error
                   				return FALSE;
                   			}
                        	uint8_t x = *pend;
                        	*pend = '\0';
#if WEBSERVER_DEBUG_EN > 4
                        	os_printf("[%s] ", pstr);
#endif
                   			rtl_memcpy(pupload->boundary, pstr, MAXLENBOUNDARY);
                        	*pend = x;
                   			pupload->sizeboundary = strlen(pupload->boundary);
                   			ts_conn->pbufo = (uint8_t *)pupload;
                   			SetSCB(SCB_BNDR);
//                          if(cnlen > ((pupload->sizeboundary * 2) + 18)) {
                        		SetSCB(SCB_RXDATA);
//                          }
                        };
                    };
            	};
            };
        	if((!CheckSCB(SCB_BNDR)) && cnlen > CurHTTP->content_len) { // обычный контент и недокачан заголовок? да.
        		CurHTTP->content_len = cnlen;
#if WEBSERVER_DEBUG_EN > 2
            	os_printf("wait content ");
#endif
        		CurHTTP->httpStatus = 413; // 413 Request Entity Too Large // пока так
        		return TRUE; // докачивать
           	};
        }
        else CurHTTP->content_len = cnlen; // уточнить, что Content Length = 0
    };
    if(CheckSCB(SCB_AUTH)) {
    	pstr = head_find_ctr(CurHTTP, HTTPAuthorization, sizeHTTPAuthorization, 5 + 3); // "Authorization: Basic 1234\r\n"
    	if(pstr == NULL || CurHTTP->httpStatus != 200) {
    		CurHTTP->httpStatus = 401; // 401 Unauthorized
    		return FALSE;
    	}
        if(strncmp(pstr, "Basic", 5) == 0) { // The authorization method and a space i.e. "Basic" is then put before the encoded string.
        	pstr += 5;
        	while(*pstr == ' ') pstr++;
#if USE_WEB_AUTH_LEVEL
        	web_conn->auth_level = CheckAuthorization(pstr);
#if WEBSERVER_DEBUG_EN > 1
        	os_printf("%u?%u ", web_conn->auth_level, web_conn->auth_realm);
#endif
        	if(web_conn->auth_level >= web_conn->auth_realm)
        		ClrSCB(SCB_AUTH);
#else
        	if(CheckAuthorization(pstr))
        		ClrSCB(SCB_AUTH);
#endif
            else {
    	   		CurHTTP->httpStatus = 401; // 401 Unauthorized
    	   		return FALSE;
    		};
        }
        else {
	   		CurHTTP->httpStatus = 401; // 401 Unauthorized
	   		return FALSE;
		};
    };

    if(CurHTTP->head_len > sizeHTTPCookie + 4) { // "Cookie: a=\r\n"
    	pstr = head_find_ctr(CurHTTP, HTTPCookie, sizeHTTPCookie, 2);
        if(pstr != NULL) {
    		pend = web_strnstr(pstr, CRLF, CurHTTP->phead + CurHTTP->head_len - pstr);
        	if(pend != NULL) {
        		CurHTTP->pcookie = pstr;
        		CurHTTP->cookie_len = pend - pstr;
#if WEBSERVER_DEBUG_EN > 3
        		*pend = '\0';
				os_printf("cookie:[%s] ", pstr);
        		*pend = '\r';
#endif
        	}
#if WEBSERVER_DEBUG_EN > 3
           	else os_printf("cookie not crlf! ");
#endif
        };
    };
#ifdef WEBSOCKET_ENA
    if(CheckSCB(SCB_GET) && web_conn->bffiles[0] == WEBFS_WEBCGI_HANDLE) {
#if WEBSERVER_DEBUG_EN > 3
      	os_printf("hdlen=%d ", CurHTTP->head_len);
#endif
    	if(CurHTTP->head_len > sizeHTTPUpgrade +  sizeHTTPwebsocket + 2 + sizeHTTPSecWebSocketKey + minsizeWebSocketKey + 2) { // + "\r\n"
        	pstr = head_find_ctr(CurHTTP, HTTPUpgrade, sizeHTTPUpgrade, sizeHTTPwebsocket);
        	if(CurHTTP->httpStatus != 200) return FALSE;
        	if(pstr != NULL) {
            	if(!rom_xstrcmp(word_to_lower_case(pstr), HTTPwebsocket)) {
                    CurHTTP->httpStatus = 400; // 400 Bad Request
                    return FALSE;
            	}
            	pstr = head_find_ctr(CurHTTP, HTTPSecWebSocketKey, sizeHTTPSecWebSocketKey, minsizeWebSocketKey);
            	if(pstr == NULL || CurHTTP->httpStatus != 200) return FALSE;
            	{
            		if(WebSocketAcceptKey(CurHTTP->pFilename, pstr)) SetSCB(SCB_WEBSOC);
            	}
        	}
        }
    }
#endif
    return FALSE;
}
/******************************************************************************
 * FunctionName : web_inc_fp
 * Parameters   : fp
*******************************************************************************/
static void web_inc_fp(WEB_SRV_CONN *web_conn, webfs_handle_t fp)
{
	if(web_conn->bffiles[3] != WEBFS_INVALID_HANDLE) {
	#if WEBSERVER_DEBUG_EN > 1
		os_printf("cf%d ", web_conn->bffiles[3]);
	#endif
		if(web_conn->bffiles[3] <= WEBFS_MAX_HANDLE) {
			web_conn->content_len -= webfs_GetBytesRem(web_conn->bffiles[3]);
			webfs_Close(web_conn->bffiles[3]);
		}
	};
    web_conn->bffiles[3] = web_conn->bffiles[2];
    web_conn->bffiles[2] = web_conn->bffiles[1];
    web_conn->bffiles[1] = web_conn->bffiles[0];
	web_conn->bffiles[0] = fp;
	SetSCB(SCB_FOPEN); // файл открыт
}
/******************************************************************************
 * FunctionName : web_inc_fopen
 * Description  : web include file open
 * Parameters   : struct
 * Returns      : TRUE - open OK
*******************************************************************************/
bool web_inc_fopen(TCP_SERV_CONN *ts_conn, uint8_t *cFile)
{
	WEB_SRV_CONN *web_conn = (WEB_SRV_CONN *)ts_conn->linkd;
	if(CheckSCB(SCB_FOPEN) && (!CheckSCB(SCB_FCALBACK))) { // файл уже открыт и он не парсится?
	    return FALSE; // такое не поддерживается в "~inc:filename~"
	};
	webfs_handle_t fp = webfs_Open(cFile);
#if WEBSERVER_DEBUG_EN > 1
	os_printf("of%d[%s] ", fp, cFile);
#endif
	if(fp != WEBFS_INVALID_HANDLE)
   {
		if(fatCache.flags & WEBFS_FLAG_HASINDEX) SetSCB(SCB_FCALBACK); // файл надо парсить
		web_conn->content_len += webfs_GetBytesRem(fp); // указать размер файла для вывода
		if(fatCache.flags & WEBFS_FLAG_ISZIPPED) {
			if(CheckSCB(SCB_FOPEN)) { // файл уже открыт и "~inc:filename~" не поддерживает GZIP!
				webfs_Close(fp);
#if WEBSERVER_DEBUG_EN > 1
				os_printf("Not inc GZIP! ");
#endif
				return FALSE;
			};
			SetSCB(SCB_FGZIP); // файл сжат GZIP
		}
	}
	else
   { // File not found
	    return FALSE;
	};
	web_inc_fp(web_conn, fp);
	return TRUE;
};
/******************************************************************************
 * FunctionName : web_inc_file
 * Description  : web include file close
 * Parameters   : struct
 * Returns      : TRUE - все файлы закрыты
*******************************************************************************/
bool web_inc_fclose(WEB_SRV_CONN *web_conn)
{
	if(web_conn->bffiles[0] != WEBFS_INVALID_HANDLE)
   {
#if WEBSERVER_DEBUG_EN > 1
		os_printf("cf%d ", web_conn->bffiles[0]);
#endif
		if(web_conn->bffiles[0] <= WEBFS_MAX_HANDLE)
      {
			webfs_Close(web_conn->bffiles[0]);
			ClrSCB(SCB_FGZIP);
		}
        web_conn->bffiles[0] = web_conn->bffiles[1];
        web_conn->bffiles[1] = web_conn->bffiles[2];
        web_conn->bffiles[2] = web_conn->bffiles[3];
        web_conn->bffiles[3] = WEBFS_INVALID_HANDLE;
   		if(web_conn->bffiles[0] != WEBFS_INVALID_HANDLE) return FALSE;
	};
	ClrSCB(SCB_FOPEN | SCB_FGZIP | SCB_FCALBACK);
	return TRUE; // больше нет файлов
};
/******************************************************************************
 * FunctionName : webserver_open_file
 * Description  : Compare to known extensions to determine Content-Type
 * Parameters   : filename -- file name
 * Returns      : 1 - open, 0 - no
*******************************************************************************/
static bool webserver_file_ext(HTTP_CONN *CurHTTP, uint8_t *pfname)
{
   uint8_t *pfext = NULL;
   while(*pfname >= ' ') if(*pfname++ == '.') pfext = pfname;
   if(pfext != NULL)
   {
      for(CurHTTP->fileType = HTTP_TXT; CurHTTP->fileType < HTTP_UNKNOWN; CurHTTP->fileType++)
         if(rom_xstrcmp(pfext, httpFileExtensions[CurHTTP->fileType]))
            return TRUE;
   };
   return FALSE;
}
/*----------------------------------------------------------------------*/
#ifdef USE_CAPTDNS
/* = flase, если включен redirect, и запрос от ip адреса из подсети AP,
 * и Host name не равен aesp8266 или ip AP. */
static bool web_cdns_no_redir(HTTP_CONN *CurHTTP, TCP_SERV_CONN *ts_conn)
{
	if(syscfg.cfg.b.cdns_ena
	  && pcb_cdns != NULL
	  &&((ts_conn->pcb->remote_ip.addr ^ info.ap_ip) & info.ap_mask) == 0
	  && CurHTTP->phead != NULL
	  && CurHTTP->head_len != 0) {
		uint8_t * ps = head_find_ctr(CurHTTP, HTTPHost, sizeHTTPHost, 7);
		if(ps != NULL) {
#if WEBSERVER_DEBUG_EN > 1
		os_printf("Host: '%s' ", ps);
#endif
		uint8_t strip[4*4];
		os_sprintf_fd(strip, IPSTR, IP2STR(&info.ap_ip));
		if((rom_xstrcmp(ps, HostNameLocal) == 0) && (rom_xstrcmp(ps, strip) == 0))   {
				rtl_sprintf(CurHTTP->pFilename, httpHostNameLocal, HostNameLocal); // "http://esp8266/"
				WEB_SRV_CONN *web_conn = (WEB_SRV_CONN *)ts_conn->linkd;
				SetSCB(SCB_REDIR);
				return FALSE;
			}
		}
	}
	return TRUE;
}
#endif
/******************************************************************************
 * FunctionName : webserver_open_file
 * Description  : Open file
 * Parameters   : filename -- file name
 * Returns      : 1 - open, 0 - no
*******************************************************************************/
static bool webserver_open_file(HTTP_CONN *CurHTTP, TCP_SERV_CONN *ts_conn)
{
	WEB_SRV_CONN *web_conn = (WEB_SRV_CONN *)ts_conn->linkd;
	uint8_t pbuf[WEBFS_FILENAME_LEN_MAX];
	uint8_t *pstr = pbuf;
	if(CurHTTP->pFilename[0] == '/') {
		if(CurHTTP->pFilename[1] == '\0') {
			if(isWEBFSLocked) {
				web_inc_fp(web_conn, WEBFS_NODISK_HANDLE); // желательно дописать ответ, что нет диска.
				web_conn->content_len = sizeHTTPfserror;
				CurHTTP->fileType = HTTP_HTML;
#if WEBSERVER_DEBUG_EN > 1
				os_printf("of%d[%s] ", web_conn->webfile, CurHTTP->pFilename);
#endif
				return TRUE;
			}
			else {
#ifdef USE_CAPTDNS
				if(web_cdns_no_redir(CurHTTP, ts_conn)) rom_xstrcpy(pstr, http_default_file);
				else return FALSE;
#else
				rom_xstrcpy(pstr, http_default_file);
#endif
			}
		}
		else
      {
			rtl_memcpy(pstr, &CurHTTP->pFilename[1], WEBFS_FILENAME_LEN_MAX - 1);
			if(rom_xstrcmp(pstr, web_cgi_fname)) {
				web_inc_fp(web_conn, WEBFS_WEBCGI_HANDLE);
				web_conn->content_len = sizeHTTPdefault;
#if USE_WEB_AUTH_LEVEL
//				web_conn->auth_realm = WEB_AUTH_LEVEL_USER;
//				SetSCB(SCB_AUTH);
#endif
				CurHTTP->fileType = HTTP_HTML;
#if WEBSERVER_DEBUG_EN > 1
				os_printf("of%d[%s] ", web_conn->webfile, CurHTTP->pFilename);
#endif
				return TRUE;
			}
			else if(rom_xstrcmp(pstr, fsupload_fname)) {
#if USE_WEB_AUTH_LEVEL
				web_conn->auth_realm = WEB_AUTH_LEVEL_WEBFS;
#endif
				SetSCB(SCB_AUTH);
				web_inc_fp(web_conn, WEBFS_UPLOAD_HANDLE);
				web_conn->content_len = sizeHTTPfsupload;
				CurHTTP->fileType = HTTP_HTML;
#if WEBSERVER_DEBUG_EN > 1
				os_printf("of%d[%s] ", web_conn->webfile, CurHTTP->pFilename);
#endif
				return TRUE;
			}
		}
		if(isWEBFSLocked) return FALSE;
		// поиск файла на диске
		if(!web_inc_fopen(ts_conn, pstr)) {
			uint32_t i = strlen(pbuf);
			if(i + sizeof(http_default_file) < WEBFS_FILENAME_LEN_MAX - 1) {
				// добавить к имени папки "/index.htm"
				pbuf[i] = '/';
				rom_xstrcpy(&pbuf[i+1], http_default_file);
				if(!web_inc_fopen(ts_conn, pstr)) {
#ifdef USE_CAPTDNS
					web_cdns_no_redir(CurHTTP, ts_conn);
#endif
					return FALSE;
				}
			};
		};
		// Compare to known extensions to determine Content-Type
		return webserver_file_ext(CurHTTP, pstr);
		//return TRUE;
	};
	return FALSE; // файл не открыт
}
/******************************************************************************
*******************************************************************************/
static void web_send_fnohanle(TCP_SERV_CONN *ts_conn)
{
	WEB_SRV_CONN *web_conn = (WEB_SRV_CONN *)ts_conn->linkd;
	uint32_t pdata = 0;
//	uint8_t pbuf[mMAX(mMAX(sizeHTTPdefault,sizeHTTPfserror), sizeHTTPfsupload)];
	uint32_t size = 0;
	switch(web_conn->webfile) {
	case WEBFS_WEBCGI_HANDLE:
		pdata = (uint32_t)((void *)HTTPdefault);
		size = sizeHTTPdefault;
		break;
	case WEBFS_UPLOAD_HANDLE:
		pdata = (uint32_t)((void *)HTTPfsupload);
		size = sizeHTTPfsupload;
		break;
	case WEBFS_NODISK_HANDLE:
		pdata = (uint32_t)((void *)HTTPfserror);
		size = sizeHTTPfserror;
		break;
	}
	if(pdata != 0 && size != 0) {
//		spi_flash_read(pdata & MASK_ADDR_FLASH_ICACHE_DATA, pbuf, size);
		tcpsrv_int_sent_data(ts_conn, (uint8_t *) pdata, size);
	}
#if WEBSERVER_DEBUG_EN > 1
	os_printf("%u ", size);
#endif
	SetSCB(SCB_FCLOSE|SCB_DISCONNECT);
}
/******************************************************************************
*******************************************************************************/
static int web_find_cbs(uint8_t * chrbuf, uint32_t len)
{
   for(uint32_t i = 0; i < len; i++)
      if(chrbuf[i] == '~')
         return i;
   return -1;
}
/******************************************************************************
 * FunctionName : webserver_send_fdata
 * Description  : Sent callback function to call for this espconn when data
 *                is successfully sent
 * Parameters   : arg -- Additional argument to pass to the callback function
 * Returns      : none
*******************************************************************************/
static void webserver_send_fdata(TCP_SERV_CONN *ts_conn)
{
	WEB_SRV_CONN *web_conn = (WEB_SRV_CONN *)ts_conn->linkd;
	if(web_conn->webfile == WEBFS_INVALID_HANDLE)
   {
		SetSCB(SCB_FCLOSE|SCB_DISCONNECT);
		return;
	}
#if WEBSERVER_DEBUG_EN > 1
	os_printf("send: ");
#endif
#ifdef SET_CPU_CLK_SPEED
//	set_cpu_clk();
#endif
	web_conn->msgbufsize = tcp_sndbuf(ts_conn->pcb);

#if WEBSERVER_DEBUG_EN > 5
	os_printf("sndbuf=%u ", web_conn->msgbufsize);
#endif

	if (web_conn->msgbufsize < MIN_SEND_SIZE) {
#if WEBSERVER_DEBUG_EN > 1
		os_printf("sndbuf=%u! ", web_conn->msgbufsize);
		if(ts_conn->flag.wait_sent) os_printf("wait_sent! "); // блок передан?
#endif
		ts_conn->pcb->flags &= ~TF_NODELAY;
		tcpsrv_int_sent_data(ts_conn, (uint8_t *)ts_conn, 0);
		return;
	}
	if((web_conn->webfile > WEBFS_MAX_HANDLE)&&(!CheckSCB(SCB_RETRYCB)))  {
		web_send_fnohanle(ts_conn);
		return;
	}
	web_conn->msgbufsize = mMIN(MAX_SEND_SIZE, web_conn->msgbufsize);
	uint8_t *pbuf = (uint8_t *) malloc(web_conn->msgbufsize);
	if (pbuf == NULL) {
#if WEBSERVER_DEBUG_EN > 0
		os_printf("out of memory - disconnect!\n");
#endif
		SetSCB(SCB_FCLOSE|SCB_DISCONNECT);
		return;
	};
	web_conn->msgbuf = pbuf;
	web_conn->msgbuflen = 0;
	if (CheckSCB(SCB_CHUNKED)) { // is chunked
		web_conn->msgbuf += RESCHKS_SEND_SIZE;
		web_conn->msgbufsize -= RESCHK_SEND_SIZE;
	};
	if(CheckSCB(SCB_FCALBACK) == 0) { // передача файла без парсинга
		// Get/put as many bytes as possible
		web_conn->msgbuflen = webfs_GetArray(web_conn->webfile, web_conn->msgbuf, web_conn->msgbufsize);
		if(web_conn->msgbuflen < web_conn->msgbufsize ) SetSCB(SCB_FCLOSE | SCB_DISCONNECT);
	}
	else { // парсинг потока передачи
		do { // начинаем с пустого буфера
			if(CheckSCB(SCB_RETRYCB)) { // повторный callback? да
#if WEBSERVER_DEBUG_EN > 2
				os_printf("rcb ");
#endif
				if(web_conn->func_web_cb != NULL) web_conn->func_web_cb(ts_conn);
				if(CheckSCB(SCB_RETRYCB)) break; // повторить ещё раз? да.
			}
			else {
				uint8_t *pstr = &web_conn->msgbuf[web_conn->msgbuflen]; // указатель в буфере
				// запомнить указатель в файле. ftell(fp)
				uint32_t max = mMIN(web_conn->msgbufsize - web_conn->msgbuflen, SCB_SEND_SIZE); // читаем по 128 байт ?
				uint32_t len = webfs_GetArray(web_conn->webfile, pstr, max);
				// прочитано len байт в буфер по указателю &sendbuf[msgbuflen]
				if(len) { // есть байты для передачи, ищем string "~calback~"
					int cmp = web_find_cbs(pstr, len);
					if(cmp >= 0) { // найден calback
						// откат файла
						WEBFSStubs[web_conn->webfile].addr -= len;
						WEBFSStubs[web_conn->webfile].bytesRem += len;
						// передвинуть указатель в файле на считанные байты с учетом маркера, без добавки длины для передачи
						WEBFSStubs[web_conn->webfile].addr += cmp+1;
						WEBFSStubs[web_conn->webfile].bytesRem -= cmp+1;
						// это второй маркер?
						if(CheckSCB(SCB_FINDCB)) { // в файле найден закрывающий маркер calback
							ClrSCB(SCB_FINDCB); // прочитали string calback-а
							if(cmp != 0) { // это дубль маркера ? нет.
								// запустить calback
								pstr[cmp] = '\0'; // закрыть string calback-а
								if(!memcmp((void*)pstr, "inc:", 4)) { // "inc:file_name"
									if(!web_inc_fopen(ts_conn, &pstr[4])) {
										tcp_strcpy_fd("file not found!");
									};
								}
								else web_int_callback(ts_conn, pstr);
							}
							else { // Дубль маркера.
								web_conn->msgbuflen++; // передать только маркер ('~')
							};
						}
						else {
							SetSCB(SCB_FINDCB); // в файле найден стартовый маркер calback
							web_conn->msgbuflen += cmp;  // передать до стартового маркера calback
						};
					}
					else {  // просто данные
						ClrSCB(SCB_FINDCB);
						if(len < max) {
							if(web_inc_fclose(web_conn)) SetSCB(SCB_FCLOSE | SCB_DISCONNECT); // файл(ы) закончилсь совсем? да.
						};
						web_conn->msgbuflen += len; // добавить кол-во считанных байт для передачи.
					};
				}
				else if(web_inc_fclose(web_conn)) SetSCB(SCB_FCLOSE | SCB_DISCONNECT); // файл(ы) закончилсь совсем? да.
			};  // not SCB_RETRYCB
		} // набираем буфер
		while((web_conn->msgbufsize - web_conn->msgbuflen >= SCB_SEND_SIZE)&&(!CheckSCB(SCB_FCLOSE | SCB_RETRYCB | SCB_DISCONNECT)));
	};
#if WEBSERVER_DEBUG_EN > 3
	os_printf("#%04x %d ", web_conn->webflag, web_conn->msgbuflen);
#elif WEBSERVER_DEBUG_EN > 1
	os_printf("%u ", web_conn->msgbuflen);
#endif
	if(web_conn->msgbuflen) {
		web_conn->content_len -= web_conn->msgbuflen; // пока только для инфы
		if(CheckSCB(SCB_CHUNKED)) { // greate chunked
			uint8_t cbuf[16];
			static const char chunks[] = "\r\n%X\r\n";
			unsigned int len = rtl_sprintf(cbuf, chunks, web_conn->msgbuflen);
			web_conn->msgbuf -= len;
			rtl_memcpy(web_conn->msgbuf, cbuf, len);
			web_conn->msgbuflen += len;
			if(CheckSCB(SCB_FCLOSE)) { // close file? -> add 'end chunked'
				tcp_strcpy_fd("\r\n0\r\n\r\n");
			};
		};
		ts_conn->pcb->flags |= TF_NODELAY;
		tcpsrv_int_sent_data(ts_conn, web_conn->msgbuf, web_conn->msgbuflen);
	};
	free(pbuf);
	web_conn->msgbuf = NULL;
}
/******************************************************************************
 * FunctionName : web_print_headers
 * Description  : Print HTTP Response Header
 * Parameters   : *
 * Returns      : none
*******************************************************************************/
static void web_print_headers(HTTP_CONN *CurHTTP, TCP_SERV_CONN *ts_conn)
{
   WEB_SRV_CONN *web_conn = (WEB_SRV_CONN *)ts_conn->linkd;
   HTTP_RESPONSE *CurResp = (HTTP_RESPONSE *)HTTPResponse;
#if WEBSERVER_DEBUG_EN > 3
   os_printf("prh#%04x,%d,%d ", web_conn->webflag, CurHTTP->httpStatus, CurHTTP->fileType);
#endif
   web_conn->msgbuf = (uint8_t *)malloc(HTTP_SEND_SIZE);
   if(web_conn->msgbuf == NULL)
   {
#if WEBSERVER_DEBUG_EN == 1
      os_printf("web: out of memory!\n");
#elif WEBSERVER_DEBUG_EN > 1
      os_printf("web: out of memory! ");
#endif
      SetSCB(SCB_FCLOSE | SCB_DISCONNECT);
      return;
   }

   web_conn->msgbufsize = HTTP_SEND_SIZE;
   web_conn->msgbuflen = 0;
   if(CheckSCB(SCB_REDIR))
      CurHTTP->httpStatus = 302; // редирект
#ifdef WEBSOCKET_ENA
   if(CheckSCB(SCB_WEBSOC) && CurHTTP->httpStatus == 200)
   {
#if WEBSERVER_DEBUG_EN > 1
   CurHTTP->httpStatus = 101;
#endif
   tcp_puts(WebSocketHTTPOkKey, CurHTTP->pFilename);
   }
   else
   {
#endif
   while(!(CurResp->flag & HTTP_RESP_FLG_END))
   {
      if(CurResp->status == CurHTTP->httpStatus)
         break;
      CurResp++;
   };

   tcp_puts_fd("HTTP/1.1 %u ", CurResp->status);
   if(CurResp->status == 401)
#ifdef USE_WEB_AUTH_LEVEL
      tcp_puts_fd(CurResp->headers, web_conn->auth_realm);
   else
#endif
      tcp_strcpy(CurResp->headers);

   tcp_strcpy_fd("\r\nServer: " WEB_NAME_VERSION "\r\nConnection: close\r\n");
   if(CheckSCB(SCB_REDIR))
   {
      tcp_puts_fd("Location: %s\r\n\r\n", CurHTTP->pFilename);
      ts_conn->flag.pcb_time_wait_free = 1; // закрыть соединение
      SetSCB(SCB_DISCONNECT);
   }
   else
   {
      if(CurResp->status != 200)
      {
         web_inc_fclose(web_conn);
         ClrSCB(SCB_FCALBACK | SCB_FGZIP | SCB_CHUNKED | SCB_RXDATA | SCB_FCLOSE);
         if(CurResp->flag & HTTP_RESP_FLG_FINDFILE)
         {
            sprintf(CurHTTP->pFilename, "/%u.htm", CurResp->status);
            webserver_open_file(CurHTTP, ts_conn);
            //CurHTTP->httpStatus = CurResp->status; // вернуть статус!
         }
      }

      if((!CheckSCB(SCB_FOPEN)) && (CurResp->default_content != NULL))
      {
         tcp_puts_fd("%s %u\r\n%s %s\r\n\r\n", HTTPContentLength, rtl_strlen(CurResp->default_content),
         HTTPContentType, httpContentTypes[HTTP_TXT]);
         tcp_strcpy(CurResp->default_content);
         SetSCB(SCB_DISCONNECT);
      }
      else if(CheckSCB(SCB_FOPEN))
      {
         if(web_conn->content_len > 0)
         {
            tcp_strcpy_fd("X-Content-Type-Options: nosniff\r\n");
            //tcp_strcpy_fd("Access-Control-Allow-Origin: *\r\n");
            
            if(CurHTTP->fileType != HTTP_UNKNOWN)
            {
               if(web_conn->bffiles[0] == WEBFS_WEBCGI_HANDLE && CheckSCB(SCB_FCALBACK))
                  CurHTTP->fileType = web_conn->fileType;
               tcp_puts_fd("Content-Type: %s\r\n", httpContentTypes[CurHTTP->fileType]);
            }

            if(CheckSCB(SCB_FCALBACK)) // длина неизветсна
            {
               tcp_strcpy_fd("Cache-Control: no-cache\r\n"); // max-age=0, must-revalidate, no-store,
               if(CurHTTP->httpver >= 0x11)
                  SetSCB(SCB_CHUNKED);
            }
            else
            {
               tcp_puts_fd("%s %d\r\n", HTTPContentLength, web_conn->content_len);
               /*
               if(CurResp->status == 200 && (!isWEBFSLocked) && web_conn->bffiles[0] != WEBFS_WEBCGI_HANDLE)
               {
                  tcp_puts_fd("Cache-Control: max-age=%d\r\n", FILE_CACHE_MAX_AGE_SEC);
               }
               else
               {*/
                  tcp_strcpy_fd("Cache-Control: no-cache\r\n");
               //}
            }

            if(CheckSCB(SCB_FGZIP))
            {
               tcp_strcpy_fd("Content-Encoding: gzip\r\n");
            }
            else if(CheckSCB(SCB_CHUNKED))
            {
               tcp_strcpy_fd("Transfer-Encoding: chunked\r\n");
            }

            if(!CheckSCB(SCB_CHUNKED))
            {
               tcp_strcpy_fd(CRLF);
            }
         }
         else
         {
            tcp_puts_fd("%s 0\r\n\r\n", HTTPContentLength);
            SetSCB(SCB_FCLOSE|SCB_DISCONNECT);
         }
      }
      else
      {
         SetSCB(SCB_DISCONNECT);
      }
   }
#ifdef WEBSOCKET_ENA
   }
#endif

#if WEBSERVER_DEBUG_EN > 3
   os_printf("#%04x (%d) %d ", web_conn->webflag, web_conn->msgbuflen, CurHTTP->httpStatus);
   web_conn->msgbuf[web_conn->msgbuflen] = 0;
   os_printf("\n2?%d[%s]\n", web_conn->msgbuflen, web_conn->msgbuf);
#elif WEBSERVER_DEBUG_EN > 1
   os_printf("head[%d]:%d ", web_conn->msgbuflen, CurHTTP->httpStatus);
#endif

   if(web_conn->msgbuflen)
   {
      if(CheckSCB(SCB_DISCONNECT)) SetSCB(SCB_CLOSED);
      tcpsrv_int_sent_data(ts_conn, web_conn->msgbuf, web_conn->msgbuflen);
#ifdef USE_WEB_NAGLE
      ts_conn->flag.nagle_disabled = 1;
#endif
   }
   free(web_conn->msgbuf);
   web_conn->msgbuf = NULL;
}
/******************************************************************************/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// поиск boundary
// 0 - разделитель (boundary) не найден, докачивать или ...
// 1 - boundary найден
// 200 - найден завершаюший boundary
// 400 - неверный формат
// ...
//-----------------------------------------------------------------------------
/* Пример M-Explorer: Load blk len: 399
-----------------------------7df22f37711be\r\n
Content-Disposition: form-data; name="test"; filename="readme.txt"\r\n
Content-Type: text/plain\r\n\r\n
1234567890\r\n
-----------------------------7df22f37711be\r\n
Content-Disposition: form-data; name="start"\r\n\r\n
0x1B000\r\n
-----------------------------7df22f37711be\r\n
Content-Disposition: form-data; name="stop"\r\n\r\n
0x1B000\r\n
-----------------------------7df22f37711be--\r\n */
/* Пример Google Chrome: Load blk len: 391
------WebKitFormBoundaryugGNBVFOk6qxfe22\r\n
Content-Disposition: form-data; name="test"; filename="readme.txt"\r\n
Content-Type: text/plain\r\n\r\n
1234567890\r\n
------WebKitFormBoundaryugGNBVFOk6qxfe22\r\n
Content-Disposition: form-data; name="start"\r\n\r\n
0x1B000\r\n
------WebKitFormBoundaryugGNBVFOk6qxfe22\r\n
Content-Disposition: form-data; name="stop"\r\n\r\n
0x1B000\r\n
------WebKitFormBoundaryugGNBVFOk6qxfe22--\r\n */
//-----------------------------------------------------------------------------
const char crlf_end_boundary[] = "--" CRLF;
static int find_boundary(HTTP_UPLOAD *pupload, uint8_t *pstr, uint32_t len)
{
	int x = len - 6 - pupload->sizeboundary;
	if(x <= 0) return 0; // разделитель (boundary) не найден - докачивать буфер
	int i;
	uint8_t *pcmp;
	for(i = 0; i <= x; i++) {
		if(pstr[i] == '-' && pstr[i+1] == '-') {
			pcmp = pstr + i;
//			if((pstr + len - pcmp) < pupload->sizeboundary + 6) return 0; // разделитель (boundary) не найден - докачивать буфер
			pupload->pbndr = pcmp; // указатель на заголовок boundary (конец блока данных);
			pcmp += 2;
			if(memcmp(pcmp, pupload->boundary, pupload->sizeboundary)) return 0; // разделитель (boundary) не найден
			pcmp += pupload->sizeboundary;
			if(rom_xstrcmp(pcmp, crlf_end_boundary)) {
				pcmp += 4;
				pupload->pnext = pcmp; // указатель в заголовке boundary (описание новых данных);
				return 200; // найден завершающий разделитель
			}
			if(pcmp[0] != '\r' || pcmp[1] != '\n') return 400; // неверный формат
			pcmp += 2;
			pupload->pnext = pcmp; // указатель в заголовке boundary (описание новых данных);
			return 1;
		};
	};
	return 0; // разделитель (boundary) не найден - докачивать буфер
}
//-----------------------------------------------------------------------------
// Function: cmp_next_boundary
// return:
// 0 - разделитель (boundary) не найден, докачивать
// 1 - далее обработка данных
// 200 - найден завершающий разделитель: "\r\n--boundary--"
// 400 - неизвестный формат content-а
//-----------------------------------------------------------------------------
const char disk_ok_filename[] = "/disk_ok.htm";
const char disk_err1_filename[] = "/disk_er1.htm";
const char disk_err2_filename[] = "/disk_er2.htm";
const char disk_err3_filename[] = "/disk_er3.htm";
const char sysconst_filename[] = "sysconst";
#ifdef USE_OVERLAY
const char overlay_filename[] = "overlay";
#endif
const char sector_filename[] = "fsec_";
#define sector_filename_size 5
const char file_label[] = "file";

static int upload_boundary(TCP_SERV_CONN *ts_conn) // HTTP_UPLOAD pupload, uint8_t pstr, uint16_t len)
{
	HTTP_UPLOAD *pupload = (HTTP_UPLOAD *)ts_conn->pbufo;
	WEB_SRV_CONN *web_conn = (WEB_SRV_CONN *)ts_conn->linkd;
	if(pupload == NULL) return 500; // ошибка сервера
	uint32_t ret;
	uint32_t len;
	uint8_t *pnext;
	uint8_t *pstr;
	while(web_conn->content_len && ts_conn->pbufi != NULL) {
		pstr = ts_conn->pbufi;
		len = ts_conn->sizei;
#if WEBSERVER_DEBUG_EN > 4
		os_printf("bufi[%u]%u, cont:%u ", ts_conn->sizei, ts_conn->cntri, web_conn->content_len);
#endif
		if(len < (8 + pupload->sizeboundary)) return 0; // разделитель (boundary) не влезет - докачивать буфер
		switch(pupload->status) {
			case 0: // поиск boundary
			{
#if WEBSERVER_DEBUG_EN > 4
				os_printf("find_bndr ");
#endif
				pnext = web_strnstr(pstr, CRLF CRLF , len);
				if(pnext == NULL) return 0; // докачивать
				len = pnext - pstr;
				ret = find_boundary(pupload, pstr, len);
#if WEBSERVER_DEBUG_EN > 4
				os_printf("len=%u,ret=%u ", len, ret );
#endif
				if(ret != 1) return ret;
				pstr = pupload->pnext; // +"\r\n" адрес за заголовком boundary
				pupload->name[0] = '\0';
				pupload->filename[0] = '\0';
				pstr = web_strnstr(pstr, "name=", pnext - pstr);
				if(pstr == NULL) return 400; // неизвестный формат content-а
				pstr += 5;
				if(pstr >= pnext) return 400; // неизвестный формат content-а
				uint8_t *pcmp = cmpcpystr(pupload->name, pstr, '"', '"', VarNameSize);
				if(pcmp == NULL) {
				 	pcmp = cmpcpystr(pupload->name, pstr, 0x22, 0x22, VarNameSize);
				 	if(pcmp == NULL) return 400; // неизвестный формат content-а
				};
				pstr = pcmp;
#if WEBSERVER_DEBUG_EN > 4
				os_printf("name:'%s' ", pupload->name);
#endif
				if(pstr >= pnext) return 400; // неизвестный формат content-а
				pcmp = web_strnstr(pstr, "filename=", pnext - pstr);
				if(pcmp != NULL) {
					pcmp += 9;
					if(pcmp < pnext) {
						if(cmpcpystr(pupload->filename, pcmp, '"', '"', VarNameSize) == NULL)
							cmpcpystr(pupload->filename, pcmp, 0x22, 0x22, VarNameSize);
					};
#if WEBSERVER_DEBUG_EN > 1
					if(pupload->filename[0]!= '\0') os_printf("filename:'%s' ", pupload->filename);
#endif
				};
				len += 4;
				pupload->status++;
#if WEBSERVER_DEBUG_EN > 4
					os_printf("trim#%u\n", len );
#endif
				ts_conn->cntri += len; // далее идут данные
				if(!web_trim_bufi(ts_conn, &ts_conn->pbufi[len], ts_conn->sizei - len)) return 500;
				web_conn->content_len -= len;
				break;
			}
			case 1: // прием данных, первый заход, проверка форматов и т.д.
			{
#if WEBSERVER_DEBUG_EN > 4
				os_printf("tst,fn='%s' ", pupload->filename);
#endif
				if(pupload->filename[0]!='\0') { // загрузка файла?
					if(rom_xstrcmp(pupload->name, file_label)) { // !os_memcmp((void*)pupload->name, "file", 4)
						if(len < sizeof(webfs_disk_header_t)) return 0; // докачивать
						webfs_disk_header_t *dhead = (webfs_disk_header_t *)pstr;
						if(dhead->id != WEBFS_DISK_ID || dhead->ver != WEBFS_DISK_VER
								|| (web_conn->content_len - pupload->sizeboundary - 8 < dhead->disksize)) {
							if(isWEBFSLocked)
                        return 400;
							SetSCB(SCB_REDIR);
							rom_xstrcpy(pupload->filename, disk_err1_filename); // rtl_memcpy(pupload->filename,"/disk_er1.htm\0",14); // неверный формат
							return 200;
						};
						if(dhead->disksize > webfs_MaxSize()) {
							if(isWEBFSLocked) return 400;
							SetSCB(SCB_REDIR);
							rom_xstrcpy(pupload->filename, disk_err2_filename); // rtl_memcpy(pupload->filename,"/disk_er2.htm\0",14); // не влезет
							return 200;
						};
						pupload->fsize = dhead->disksize;
						pupload->faddr = webfs_BaseAddr();
#if WEBSERVER_DEBUG_EN > 4
						os_printf("updisk[%u]=ok ", dhead->disksize);
#endif
						pupload->status = 3; // = 3 загрузка WebFileSystem во flash
						isWEBFSLocked = TRUE;
						break;
					}
#ifdef USE_OVERLAY
					else if(rom_xstrcmp(pupload->name, overlay_filename)) {
						if(len < sizeof(struct SPIFlashHeader)) return 0; // докачивать
						struct SPIFlashHeader *fhead = (struct SPIFlashHeader *)pstr;
						if(web_conn->content_len - pupload->sizeboundary < sizeof(fhead)
						|| fhead->head.id != LOADER_HEAD_ID) {
							if(isWEBFSLocked) return 400;
							SetSCB(SCB_REDIR);
							rom_xstrcpy(pupload->filename, disk_err1_filename); // rtl_memcpy(pupload->filename,"/disk_er1.htm\0",14); // неверный формат
							return 200;
						};
						if(fhead->entry_point >= IRAM_BASE && ovl_call != NULL) {
							ovl_call(0); // close прошлый оверлей
							ovl_call = NULL;
						}
						pupload->start = fhead->entry_point;
						pupload->segs = fhead->head.number_segs;
						if(pupload->segs) {
							pupload->fsize = sizeof(struct SPIFlashHeadSegment);
							pupload->status = 5; // = 5 загрузка файла оверлея, начать с загрузки заголовка сегмента
						}
						else {
							pupload->fsize = 0;
							pupload->status = 4; // = 4 загрузка файла оверлея, запуск entry_point
						}
						//
						len = sizeof(struct SPIFlashHeader);
						ts_conn->cntri += len;
						if(!web_trim_bufi(ts_conn, &ts_conn->pbufi[len], ts_conn->sizei - len)) return 500;
						web_conn->content_len -= len;
						//
						break;
					}
#endif
					else if(rom_xstrcmp(pupload->name, sysconst_filename)) {
						pupload->fsize = FLASH_SECTOR_SIZE;
						pupload->faddr = FLASH_RESERVED_DATA_BASE; // FLASH_SYSTEM_DATA_ADDR
						pupload->status = 2; // = 2 загрузка файла во flash
						break;
					}
					else if(rom_xstrcmp(pupload->name, sector_filename)) {
						pupload->fsize = FLASH_SECTOR_SIZE;
						pupload->faddr = ahextoul(&pupload->name[sector_filename_size]) << 12;
						pupload->status = 2; // = 2 загрузка файла сектора во flash
						break;
					};
					if(isWEBFSLocked) return 400;
					SetSCB(SCB_REDIR);
					rom_xstrcpy(pupload->filename, disk_err3_filename); // rtl_memcpy(pupload->filename,"/disk_er3.htm\0",14); // неизвестный тип
					return 200;
				}
				else {
					uint8_t *pcmp = web_strnstr(pstr, CRLF, len);
					if(pcmp == NULL) return 0; // докачивать
					ret = find_boundary(pupload, pstr, len);
#if WEBSERVER_DEBUG_EN > 4
					os_printf("ret=%u ", ret );
#endif
					if((ret != 1 && ret != 200)) { // не найден конец или новый boundary?
						return ret; // догружать
					}
					*pcmp = '\0';
					web_int_vars(ts_conn, pupload->name, pstr);
					if(ret == 200) return ret;
					// найден следующий boundary
					len = pupload->pbndr - ts_conn->pbufi;
					pupload->status = 0; // = 0 найден следующий boundary
#if WEBSERVER_DEBUG_EN > 4
					os_printf("trim#%u\n", len );
#endif
					ts_conn->cntri += len; // далее идут данные
					if(!web_trim_bufi(ts_conn, &ts_conn->pbufi[len], ts_conn->sizei - len)) return 500;
					web_conn->content_len -= len;
					break;
				}
//				return 400;
			}
//			default:
			case 2: // загрузка файла во flash
			case 3: // загрузка WebFileSystem во flash (скорость записи W25Q128 ~175 килобайт в сек, полный диск на 15,5МБ пишется 90..100 сек )
			{
#if WEBSERVER_DEBUG_EN > 4
				os_printf("fdata ");
#endif
				uint32_t block_size = mMIN(max_len_buf_write_flash + 8 + pupload->sizeboundary, web_conn->content_len);
				if(ts_conn->sizei < block_size) return 0; // докачивать
				ret = find_boundary(pupload, pstr, block_size);
#if WEBSERVER_DEBUG_EN > 4
				os_printf("ret=%u ", ret);
#endif
				if((ret == 1 || ret == 200)) { // найден конец или новый boundary?
					len = mMIN(block_size, pupload->pbndr - 2 - ts_conn->pbufi);
				}
				else {
					len = mMIN(max_len_buf_write_flash, web_conn->content_len - 8 - pupload->sizeboundary);
				}
#if WEBSERVER_DEBUG_EN > 4
				os_printf("\nlen=%d, block_size=%d, content_len=%d, sizeboundary= %d, ret=%d, data = %d, load=%d", len, block_size, web_conn->content_len, pupload->sizeboundary, ret, pupload->pbndr - ts_conn->pbufi, ts_conn->sizei);
#endif
				if(pupload->fsize < len) block_size = pupload->fsize;
				else block_size = len;
				if(block_size) { // идут данные файла
//					tcpsrv_unrecved_win(ts_conn); // для ускорения, пока стрирается-пишется уже обновит окно (включено в web_rx_buf)

					device_mutex_lock(RT_DEV_LOCK_FLASH);
					if(pupload->faddr >= flash_get_size() && pupload->status == 3) {
						if((pupload->faddr & 0x0000FFFF)==0) {

#if WEBSERVER_DEBUG_EN > 2
							os_printf("Clear flash page addr %p... ", pupload->faddr);
#endif
							flash_erase_block(&flashobj, pupload->faddr);
						}
					}
					else if((pupload->faddr & 0x00000FFF) == 0) {
#if WEBSERVER_DEBUG_EN > 2
						os_printf("Clear flash sector addr %p... ", pupload->faddr);
#endif
						flash_erase_sector(&flashobj, pupload->faddr);
					}
#if WEBSERVER_DEBUG_EN > 2
					os_printf("Write flash addr:%p[0x%04x]\n", pupload->faddr, block_size);
#endif
					flash_stream_write(&flashobj, pupload->faddr, (block_size + 3)&(~3), (uint8_t *)pstr);

					device_mutex_unlock(RT_DEV_LOCK_FLASH);

					pupload->fsize -= block_size;
					pupload->faddr += block_size;
				}
#if WEBSERVER_DEBUG_EN > 4
				os_printf("trim#%u\n", len);
#endif
				if(len) {
					ts_conn->cntri += len;
					if(!web_trim_bufi(ts_conn, &ts_conn->pbufi[len], ts_conn->sizei - len)) return 500;
					web_conn->content_len -= len;
				}
#ifdef SET_CPU_CLK_SPEED
//				if(syscfg.cfg.b.hi_speed_enable) set_cpu_clk();
#endif
				if((ret == 1 || ret == 200)) { // найден конец или новый boundary?
					if(pupload->status == 3) webfs_Init();
					if(pupload->fsize != 0) {
						if(!isWEBFSLocked) {
							SetSCB(SCB_REDIR);
							rom_xstrcpy(pupload->filename, disk_err1_filename); // rtl_memcpy(pupload->filename,"/disk_er1.htm\0",14); // не всё передано или неверный формат
							return 200;
						}
						return 400; //  не всё передано или неверный формат
					}
					else {
						if(!isWEBFSLocked) {
							SetSCB(SCB_REDIR);
							rom_xstrcpy(pupload->filename, disk_ok_filename); // rtl_memcpy(pupload->filename,"/disk_ok.htm\0",13);
						};
					};
					if(ret == 1) pupload->status = 0; // = 0 найден следующий boundary
					if(ret == 200)	return ret;
				}
				break;
			}
#ifdef USE_OVERLAY
			case 4: // загрузка данных/кода оверлея
			case 5: // загрузка заголовка данных оверлея
			{
				uint32_t block_size = mMIN(max_len_buf_write_flash + 8 + pupload->sizeboundary, web_conn->content_len);
				if(ts_conn->sizei < block_size) return 0; // докачивать
				ret = find_boundary(pupload, pstr, block_size);
				if((ret == 1 || ret == 200)) { // найден конец или новый boundary?
					len = mMIN(block_size, pupload->pbndr - 2 - ts_conn->pbufi);
				}
				else {
					len = mMIN(max_len_buf_write_flash, web_conn->content_len - 8 - pupload->sizeboundary);
				}
				block_size = len;
				while(block_size) {
#if WEBSERVER_DEBUG_EN > 5
					os_printf("blk:%d,st:%d,fs:%d,%d  ", block_size, pupload->status, pupload->fsize, pupload->segs);
#endif
					if(pupload->status == 5) {
						if(block_size >= sizeof(struct SPIFlashHeadSegment)) { // размер данных
							if(pupload->segs) { //
								rtl_memcpy(&pupload->faddr, pstr, 4);
								rtl_memcpy(&pupload->fsize, &pstr[4], 4);
#if WEBSERVER_DEBUG_EN > 4
								os_printf("New seg ovl addr:%p[%p] ", pupload->faddr, pupload->fsize);
#endif
							}
						}
						else if(ret != 1 && ret != 200) { // не найден конец или boundary?
							return 0; // докачивать
						}
						else {
#if WEBSERVER_DEBUG_EN > 5
							os_printf("err_load_fseg ");
#endif
//						if(block_size < sizeof(struct SPIFlashHeadSegment)
//						|| pupload->segs == 0 //
//						|| pupload->fsize > USE_OVERLAY) {
							if(!isWEBFSLocked) {
								SetSCB(SCB_REDIR);
								rom_xstrcpy(pupload->filename, disk_err1_filename); // rtl_memcpy(pupload->filename,"/disk_er1.htm\0",14); // не всё передано или неверный формат
								return 200;
							}
							return 400; //  не всё передано или неверный формат
						}
						pupload->segs--; // счет сегментов
						pupload->status = 4; // загрузка данных/кода оверлея
						pstr += sizeof(struct SPIFlashHeadSegment);
						block_size -= sizeof(struct SPIFlashHeadSegment);
					};
					uint32_t i = mMIN(pupload->fsize, block_size);
					if(i) {
#if WEBSERVER_DEBUG_EN > 1
						os_printf("Wr:%p[%p] ", pupload->faddr, i);
#endif
						copy_s1d4((void *)pupload->faddr, pstr, i);
						block_size -= i;
						pupload->faddr += i;
						pstr += i;
						pupload->fsize -= i;
					};
					if(pupload->fsize == 0) {
						if(pupload->segs) { // все сегменты загружены?
							pupload->status = 5; // загрузка заголовка данных оверлея
						}
						else { // все сегменты загружены
							block_size = 0;
							break; // break while(block_size)
						}
					};
				}; // while(block_size)
				if(len) {
					ts_conn->cntri += len;
					if(!web_trim_bufi(ts_conn, &ts_conn->pbufi[len], ts_conn->sizei - len)) return 500;
					web_conn->content_len -= len;
				};
				if((ret == 1 || ret == 200)) { // найден конец или новый boundary?
#if WEBSERVER_DEBUG_EN > 5
					os_printf("fs:%d,%d ", pupload->fsize, pupload->segs);
#endif
					if(pupload->fsize != 0 || pupload->segs != 0) { //
						if(!isWEBFSLocked) {
							SetSCB(SCB_REDIR);
							rom_xstrcpy(pupload->filename, disk_err1_filename); // rtl_memcpy(pupload->filename,"/disk_er1.htm\0",14); // не всё передано или неверный формат
							return 200;
						}
						return 400; //  не всё передано или неверный формат
					}
					else {
#if WEBSERVER_DEBUG_EN > 1
						os_printf("Run%p ", pupload->start);
#endif
						if(pupload->start >= IRAM_BASE) {
							ovl_call = (tovl_call *)pupload->start;
							web_conn->web_disc_cb = (web_func_disc_cb)pupload->start; // адрес старта оверлея
							web_conn->web_disc_par = 1; // параметр функции - инициализация
						}
						if(!isWEBFSLocked) {
							SetSCB(SCB_REDIR);
							rom_xstrcpy(pupload->filename, disk_ok_filename); // rtl_memcpy(pupload->filename,"/disk_ok.htm\0",13);
						};
					};
					if(ret == 1) pupload->status = 0; // = 0 найден следующий boundary
					if(ret == 200)	return ret;
				};
				break;
			};
#endif
		};
	};
	return 0; //
}
//-----------------------------------------------------------------------------
// web_rx_buf
//
//-----------------------------------------------------------------------------
static bool web_rx_buf(HTTP_CONN *CurHTTP, TCP_SERV_CONN *ts_conn)
{
	WEB_SRV_CONN *web_conn = (WEB_SRV_CONN *)ts_conn->linkd;
	ts_conn->flag.rx_buf = 1; // указать, что всегда в режиме докачивать
//	CurHTTP->fileType = HTTP_UNKNOWN;
//	ts_conn->pbufi, ts_conn->cntri;
#if WEBSERVER_DEBUG_EN > 3
	os_printf("rx:%u[%u] ", web_conn->content_len, ts_conn->sizei);
#endif
	if(ts_conn->sizei == 0) return TRUE; // докачивать
	tcpsrv_unrecved_win(ts_conn);
	int ret = upload_boundary(ts_conn);
	if(ret > 1) {
		CurHTTP->httpStatus = ret;
		web_conn->content_len = 0;
		if(ret == 200) {
			if(CheckSCB(SCB_REDIR)) {
				HTTP_UPLOAD *pupload = (HTTP_UPLOAD *)ts_conn->pbufo;
				if(pupload != NULL) {
					rtl_memcpy(CurHTTP->pFilename, pupload->filename, VarNameSize);
//					SetSCB(SCB_DISCONNECT);
				}
			}
			else if((!isWEBFSLocked) && CheckSCB(SCB_FOPEN)
					&& web_conn->webfile <= WEBFS_MAX_HANDLE
					&& webfs_GetFilename(web_conn->webfile, CurHTTP->pFilename, FileNameSize)) {
					SetSCB(SCB_REDIR);
//					web_conn->content_len = webfs_GetBytesRem(web_conn->webfile); // webfs_GetSize(web_conn->webfile);
//					webserver_file_ext(CurHTTP, CurHTTP->pFilename);
//					return FALSE; // ok 200 + file
			}
		}
		SetSCB(SCB_DISCONNECT);
		return FALSE; // неизвестный content или end
	}
	else {
#if WEBSERVER_DEBUG_EN > 2
		os_printf("no boundary ");
#endif
		if(ts_conn->sizei > MAX_NO_DATA_BUF_SIZE) {
			CurHTTP->httpStatus = 418; // 418: Out of Coffee
			SetSCB(SCB_DISCONNECT);
			return FALSE; // неизвестный content или end
		}
	};
	if(web_conn->content_len > ts_conn->cntri) return TRUE; // докачивать
	CurHTTP->httpStatus = 400;
	SetSCB(SCB_DISCONNECT);
	web_conn->content_len = 0;
	return FALSE; // неизвестный content
}
//-----------------------------------------------------------------------------
//--- web_trim_bufi -----------------------------------------------------------
//-----------------------------------------------------------------------------
bool web_trim_bufi(TCP_SERV_CONN *ts_conn, uint8_t *pdata, uint32_t data_len)
{
    if(pdata != NULL && data_len != 0 && ts_conn->sizei > data_len) {
        	rtl_memcpy(ts_conn->pbufi, pdata, data_len); // переместим кусок в начало буфера
        	ts_conn->pbufi = (uint8_t *)realloc(ts_conn->pbufi, data_len + 1); // mem_trim(ts_conn->pbufi, data_len + 1);
        	if(ts_conn->pbufi != NULL) {
            	ts_conn->sizei = data_len; // размер куска
            	ts_conn->cntri = 0;
        	}
        	else return FALSE; // CurHTTP.httpStatus = 500; // 500 Internal Server Error
    }
    else if(ts_conn->pbufi != NULL) {
		free(ts_conn->pbufi);
		ts_conn->pbufi = NULL;
		ts_conn->sizei = 0;
		ts_conn->cntri = 0;
	};
    return TRUE;
}
/******************************************************************************
 * web_feee_bufi
 *  освободить приемный буфер
*******************************************************************************/
bool web_feee_bufi(TCP_SERV_CONN *ts_conn)
{
	if(ts_conn->pbufi != NULL) {
		free(ts_conn->pbufi);
		ts_conn->pbufi = NULL;
		ts_conn->sizei = 0;
		ts_conn->cntri = 0;
		return TRUE;
	}
	return FALSE;
}
/******************************************************************************
 * FunctionName : webserver_recv
 * Description  : Processing the received data from the server
 * Parameters   : arg -- Additional argument to pass to the callback function
 *                pusrdata -- The received data (or NULL when the connection has been closed!)
 *                length -- The length of received data
 * Returns      : none
 *
 * For HTTP 1.0, this should normally only happen once (if the request fits in one packet).
 *
*******************************************************************************/
static err_t webserver_received_data(TCP_SERV_CONN *ts_conn)
{
#if WEBSERVER_DEBUG_EN > 1
    tcpsrv_print_remote_info(ts_conn);
    os_printf("read: %d ", ts_conn->sizei);
#endif
    HTTP_CONN CurHTTP;     // Current HTTP connection state
    WEB_SRV_CONN *web_conn = ReNew_web_conn(ts_conn);
    if(web_conn == NULL) {
#if WEBSERVER_DEBUG_EN > 1
        os_printf("err mem!\n");
#endif
    	return ERR_MEM;
    }
    if(CheckSCB(SCB_CLOSED | SCB_DISCONNECT | SCB_FCLOSE )) // обрабатывать нечего
    	return ERR_OK;
    if(!CheckSCB(SCB_WEBSOC)) {
    	web_conn->udata_start = 0;
    	web_conn->udata_stop = 0;
    }
    memset(&CurHTTP, 0, sizeof(CurHTTP));
    CurHTTP.httpStatus = 200; // OK
    CurHTTP.fileType = HTTP_UNKNOWN;
    // прием и обработка заголовка HHTP
    if(!CheckSCB(SCB_HEAD_OK)) { // заголовок уже принят и обработан? нет
		ts_conn->flag.rx_buf = 1; // докачивать буфер
		tcpsrv_unrecved_win(ts_conn);
		// разбираем докачан или нет заголовок HTTP, что там принято GET или POST и открываем файл и прием content, если это POST и не прием файла.
    	if(parse_header(&CurHTTP, ts_conn)) { // заголовок полный? нет
    		if(ts_conn->sizei < MAX_HTTP_HEAD_BUF) {
#if WEBSERVER_DEBUG_EN > 4
				os_printf("buf");
#endif
#if WEBSERVER_DEBUG_EN > 1
				os_printf("...\n");
#endif
				return ERR_OK; // будем принимать ещё.
    		};
    	   	CurHTTP.httpStatus = 413; // 413 Request Entity Too Large // пока так
		};
    	// разбор заголовка
#if WEBSERVER_DEBUG_EN > 1
#ifdef WEBSOCKET_ENA
		os_printf("%s f[%s] ", (CheckSCB(SCB_POST))? "POST" : (CheckSCB(SCB_WEBSOC))? "WEBSOC" : "GET", CurHTTP.pFilename);
#else
		os_printf("%s f[%s] ", (CheckSCB(SCB_POST))? "POST" : "GET", CurHTTP.pFilename);
#endif
#endif
#if WEBSERVER_DEBUG_EN > 3
		os_printf("hcn:%p[%d],wcn:%d ", CurHTTP.pcontent, CurHTTP.content_len, web_conn->content_len);
#endif
	    if(CurHTTP.httpStatus == 200) { // && CheckSCB(SCB_FOPEN)) { // если файл открыт и всё OK
			if(CurHTTP.cookie_len != 0) web_parse_cookie(&CurHTTP, ts_conn);
			web_parse_uri_vars(&CurHTTP, ts_conn);
			if(CurHTTP.pcontent != NULL) {
				if(CheckSCB(SCB_RXDATA)) {
					if(web_conn->content_len) { // с заголовком приняли кусок данных файла?
#if WEBSERVER_DEBUG_EN > 3
						os_printf("trim:%u[%u] ", web_conn->content_len, CurHTTP.content_len);
#endif
						if(!web_trim_bufi(ts_conn, CurHTTP.pcontent, CurHTTP.content_len)) {
#if WEBSERVER_DEBUG_EN > 1
							os_printf("trim error!\n");
#endif
							CurHTTP.httpStatus = 500;
						};
					};
				}
				else {
					if(CurHTTP.content_len != 0) web_parse_content(&CurHTTP, ts_conn);
				};
			};
		};
    	SetSCB(SCB_HEAD_OK); // заголовок принят и обработан
	};
#if WEBSERVER_DEBUG_EN > 3
   	os_printf("tst_rx: %u, %u, %u ", CurHTTP.httpStatus, (CheckSCB(SCB_RXDATA) != 0), web_conn->content_len );
#endif
   	// проверка на прием данных (content)
    if(CurHTTP.httpStatus == 200 && CheckSCB(SCB_RXDATA) && (web_conn->content_len) && web_rx_buf(&CurHTTP, ts_conn)) {
#if WEBSERVER_DEBUG_EN > 1
    	os_printf("...\n");
#endif
    	return ERR_OK; // докачивать content
    };
#ifdef WEBSOCKET_ENA
	if(CheckSCB(SCB_WEBSOC) && CurHTTP.httpStatus == 200 && (!CheckSCB(SCB_REDIR))) {
		if(!CheckSCB(SCB_WSDATA)) {
			// создание и вывод заголовка ответа websock
			ClrSCB(SCB_RXDATA);
			Close_web_conn(ts_conn); // закрыть все файлы
			web_print_headers(&CurHTTP, ts_conn);
			if(CheckSCB(SCB_DISCONNECT)) {
			    ts_conn->flag.rx_null = 1; // всё - больше не принимаем!
				ts_conn->flag.rx_buf = 0; // не докачивать буфер
				if(web_feee_bufi(ts_conn)) tcpsrv_unrecved_win(ts_conn); // уничтожим буфер
			}
			else {
				SetSCB(SCB_WSDATA);
				ts_conn->flag.rx_buf = 1; // указать, что всегда в режиме докачивать
				tcpsrv_unrecved_win(ts_conn);
				tcp_output(ts_conn->pcb);

				if(web_feee_bufi(ts_conn)) tcpsrv_unrecved_win(ts_conn); // уничтожим буфер
/*
				if(ts_conn->pbufi != NULL && ts_conn->sizei != 0) { // что-то ещё есть в буфере?
#if WEBSERVER_DEBUG_EN > 1
					os_printf("ws_rx[%u]? ", ts_conn->sizei);
#endif
					websock_rx_data(ts_conn);
				}
*/
			}
		}
		else {
			websock_rx_data(ts_conn);
		}
	}
	else
#endif
	{
	    ts_conn->flag.rx_null = 1; // всё - больше не принимаем!
		ts_conn->flag.rx_buf = 0; // не докачивать буфер
		if(web_feee_bufi(ts_conn)) tcpsrv_unrecved_win(ts_conn); // уничтожим буфер
	    if(tcp_sndbuf(ts_conn->pcb) >= HTTP_SEND_SIZE) { // возможна втавка ответа?
			// создание и вывод заголовка ответа.
			web_print_headers(&CurHTTP, ts_conn);
	        // начало предачи файла, если есть
	        if((!CheckSCB(SCB_CLOSED | SCB_DISCONNECT | SCB_FCLOSE))&&CheckSCB(SCB_FOPEN)) webserver_send_fdata(ts_conn);
	    }
	    else {
#if WEBSERVER_DEBUG_EN > 1
	    	os_printf("sndbuf=%u! ", tcp_sndbuf(ts_conn->pcb));
#endif
	    	SetSCB(SCB_FCLOSE | SCB_DISCONNECT);
	    };
	}
    if(CheckSCB(SCB_FCLOSE))  {
        tcp_output(ts_conn->pcb);
    	Close_web_conn(ts_conn);
    	SetSCB(SCB_DISCONNECT);
    }
    if(CheckSCB(SCB_DISCONNECT)) web_int_disconnect(ts_conn);
#if WEBSERVER_DEBUG_EN > 1
    else  os_printf("...\n");
#endif
    return ERR_OK;
}
/******************************************************************************
 * web_int_disconnect
*******************************************************************************/
static void web_int_disconnect(TCP_SERV_CONN *ts_conn)
{
#if WEBSERVER_DEBUG_EN > 1
    os_printf("dis\n");
#endif
	WEB_SRV_CONN *web_conn = (WEB_SRV_CONN *)ts_conn->linkd;
	ts_conn->flag.tx_null = 1;
	ts_conn->flag.rx_null = 1;
	tcpsrv_unrecved_win(ts_conn);
	if(ts_conn->flag.pcb_time_wait_free) tcpsrv_disconnect(ts_conn);
	SetSCB(SCB_CLOSED);
}
/******************************************************************************
 * FunctionName : webserver_sent_cb
 * Description  : Sent callback function to call for this espconn when data
 *                is successfully sent
 * Parameters   : arg -- Additional argument to pass to the callback function
 * Returns      : none
*******************************************************************************/
static err_t webserver_sent_callback(TCP_SERV_CONN *ts_conn)
{
#if WEBSERVER_DEBUG_EN > 1
    tcpsrv_print_remote_info(ts_conn);
#endif
	WEB_SRV_CONN *web_conn = (WEB_SRV_CONN *)ts_conn->linkd;
	if(web_conn == NULL) return ERR_ARG;
    if(CheckSCB(SCB_CLOSED) == 0) { // No SCB_CLOSED
    	if(!CheckSCB(SCB_DISCONNECT)) {
#ifdef WEBSOCKET_ENA
        	if(CheckSCB(SCB_WSDATA)) {
        		websock_rx_data(ts_conn);
        	}
        	else
#endif
        		if((!CheckSCB(SCB_FCLOSE))&&CheckSCB(SCB_FOPEN)) webserver_send_fdata(ts_conn);
    	}
        if(CheckSCB(SCB_FCLOSE))  {
        	Close_web_conn(ts_conn);
        	SetSCB(SCB_DISCONNECT);
        }
        if(CheckSCB(SCB_DISCONNECT))  web_int_disconnect(ts_conn);
    #if WEBSERVER_DEBUG_EN > 1
        else  os_printf("...\n");
    #endif
    }
    else { //  SCB_CLOSED
#if WEBSERVER_DEBUG_EN > 1
      os_printf("#%04x ?\n", web_conn->webflag);
#endif
      ts_conn->flag.tx_null = 1;
      ts_conn->flag.rx_null = 1;
    };
    return ERR_OK;
}
/******************************************************************************
 * FunctionName : webserver_disconnect
 * Description  : calback disconnect
 * Parameters   : arg -- Additional argument to pass to the callback function
 * Returns      : none
*******************************************************************************/
static void webserver_disconnect(TCP_SERV_CONN *ts_conn)
{
#if WEBSERVER_DEBUG_EN > 1
   tcpsrv_disconnect_calback_default(ts_conn);
#endif
   WEB_SRV_CONN *web_conn = (WEB_SRV_CONN *)ts_conn->linkd;
   if(web_conn == NULL)
      return;

   Close_web_conn(ts_conn);
   if(CheckSCB(SCB_SYSSAVE))
   {
      ClrSCB(SCB_SYSSAVE);
      //sys_write_cfg();
   };
}

/******************************************************************************
******************************************************************************/
BaseType_t webserver_qfn(web_ex_func_cb fnc, void * param, uint16_t pause_ms)
{
   WEB_SRV_QFNK qfn;
   qfn.fnc = fnc;
   qfn.param = param;
   qfn.pause_ms = pause_ms;
   return xQueueSendToBack(xQueueWebSrv, &qfn, 0);
}
/******************************************************************************
 * todo: временная затычка, необходимо переделать...
******************************************************************************/
void qfnk_task(void * par)
{
	(void) par;
	WEB_SRV_QFNK qfn;
	WEB_SRV_QFNK qfnt;
	TickType_t timetick;
	qfnt.fnc = NULL;
	qfnt.pause_ms = 0;
	while(1) {
		if(xQueueReceive(xQueueWebSrv, &qfn, 5) == pdPASS) { // portMAX_DELAY
			if(qfn.fnc) {
#if WEBSERVER_DEBUG_EN > 2
				os_printf("qfn: %p(%p),%d\n", qfn.fnc, qfn.param, qfn.pause_ms);
#endif
				if(qfn.pause_ms) {
					timetick = xTaskGetTickCount();
					qfnt = qfn;
				}
				else qfn.fnc((uint32_t) qfn.param);
			}
		}
		else if(qfnt.fnc) {
			if(xTaskGetTickCount() - timetick > qfnt.pause_ms) {
#if WEBSERVER_DEBUG_EN > 3
				os_printf("qfnt: %p(%p),%d\n", qfnt.fnc, qfnt.param, qfnt.pause_ms);
#endif
				qfnt.fnc((uint32_t) qfnt.param);
				qfnt.fnc = NULL;
			}
		}
	}
}
/******************************************************************************
 * FunctionName : webserver_init
 * Description  : Открытие сервера
 * Parameters   : arg -- port N
 * Returns      : none
*******************************************************************************/
err_t webserver_init(uint16_t portn)
{
   // WEBFSInit();
	err_t err = ERR_MEM;
	xQueueWebSrv = xQueueCreate(5, sizeof( WEB_SRV_QFNK ));
	if(xQueueWebSrv)
   {
		if(xTaskCreate(qfnk_task, "web_qfn", 1024, NULL, tskIDLE_PRIORITY + 1 + PRIORITIE_OFFSET, NULL) == pdPASS)
		{
			TCP_SERV_CFG *p = tcpsrv_init(portn);
			if (p != NULL)
         {
				// изменим конфиг на наше усмотрение:
				//if(syscfg.cfg.b.web_time_wait_delete)
            if(WEBSERVER_PCB_WAIT == 1)
               p->flag.pcb_time_wait_free = 1; // пусть убивает, для теста и проксей
				p->max_conn = 99; // сработает по heap_size
#if WEBSERVER_DEBUG_EN > 3
				os_printf("Max connection %d, time waits %d & %d, min heap size %d\n",
						p->max_conn, p->time_wait_rec, p->time_wait_cls, p->min_heap);
#endif
				//p->time_wait_rec = syscfg.web_twrec; // if =0 -> вечное ожидание
				//p->time_wait_cls = syscfg.web_twcls; // if =0 -> вечное ожидание
            p->time_wait_rec = WEBSERVER_WAIT_REC; // if =0 -> вечное ожидание
				p->time_wait_cls = WEBSERVER_WAIT_CLS; // if =0 -> вечное ожидание
				// слинкуем с желаемыми процедурами:
			 	p->func_discon_cb = webserver_disconnect;
		//	 	p->func_listen = webserver_listen; // не требуется
			 	p->func_sent_cb = webserver_sent_callback;
				p->func_recv = webserver_received_data;
				err = tcpsrv_start(p);
				if (err != ERR_OK)
            {
					tcpsrv_close(p);
					p = NULL;
				}
				else
            {
#if WEBSERVER_DEBUG_EN > 1
					os_printf("WEB: init port %u\n", portn);
#endif
				}
			}
		}
	}
	return err;
}
/******************************************************************************
 * FunctionName : webserver_close
 * Description  : закрытие сервера
 * Parameters   : arg -- port N
 * Returns      : none
*******************************************************************************/
err_t webserver_close(uint16_t portn)
{
	err_t err = ERR_ARG;
	if(portn != 0) err = tcpsrv_close(tcpsrv_server_port2pcfg(portn));
#if WEBSERVER_DEBUG_EN > 1
	if(err == ERR_OK) os_printf("WEB: close\n");
#endif
	if(xQueueWebSrv) {
		WEB_SRV_QFNK qfn;
		qfn.fnc = (web_ex_func_cb) vTaskDelete;
		qfn.param = NULL;
		qfn.pause_ms = 0;
		if(xQueueSendToBack(xQueueWebSrv, &qfn, 1000) == pdPASS) {
			while(uxQueueMessagesWaiting(xQueueWebSrv)) {
				vTaskDelay(10);
			};
		}
		xQueueWebSrv = NULL;
	};
	return err;
}
/******************************************************************************
 * FunctionName : webserver_reinit
 * Description  : закрытие сервера и открытие нового
 * Parameters   : arg -- port N открытого порта
 * Returns      : none
*******************************************************************************/
err_t webserver_reinit(uint16_t portn)
{
   err_t err = ERR_OK;
   //if(portn == syscfg.web_port)
   //   return err;
   if(portn)
      err = tcpsrv_close(tcpsrv_server_port2pcfg(portn)); // закрыть старый порт
   //if(syscfg.web_port)
   //   err = webserver_init(syscfg.web_port); // открыть новый
   err = webserver_init(WEBSERVER_PORT);
   return err;
}

#endif // WEBSERVER_PORT
