/*
 * web_auth.c
 *
 *  Created on: 23/04/2017.
 *      Author: pvvx
 * 2021.05 A_D fixes
 */

#include "autoconf.h"
#include "FreeRTOS.h"
#include "diag.h"
#include "web_utils.h"
#include "wifi_conf.h"
#include "web_srv.h"

uint8_t web_auth(uint8_t *pbuf, size_t declen)
{
   uint8_t * psw = strchr(pbuf, ':');

   if(psw != NULL)
   {
#if USE_WEB_AUTH_LEVEL
      if(strcmp(pbuf, "rtl871x:webfs_write") == 0)
      {
         return WEB_AUTH_LEVEL_WEBFS;
      }
      else if(strcmp(pbuf, "rtl871x:ota_write") == 0)
      {
         return WEB_AUTH_LEVEL_OTA;
      }
      else if(strcmp(pbuf, "rtl871x:supervisor") == 0)
      {
         return WEB_AUTH_LEVEL_SUPERVISOR;
      }
#endif
      *psw++ = 0;
      // TODO: переделать под стандартный API...
      /*
      if(rom_xstrcmp(wifi_ap_cfg.ssid, pbuf) && rom_xstrcmp( wifi_ap_cfg.password, psw))
      {
         return WEB_AUTH_LEVEL_USER;
      }
      else if(rom_xstrcmp(wifi_st_cfg.ssid, pbuf) && rom_xstrcmp( wifi_st_cfg.password, psw))
      {
         return WEB_AUTH_LEVEL_USER1;
      }*/
   }
   return WEB_AUTH_NONE;
}
