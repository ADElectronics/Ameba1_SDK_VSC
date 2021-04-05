#include "lwip/netif.h"
#include "lwip_netconf.h"

#if CONFIG_EXAMPLE_LWIP_APP_MDNS
#include "apps/mdns.h"

extern struct netif xnetif[NET_IF_NUM];

static service_get_txt_fn_t srv_txt(struct mdns_service *service, void *txt_userdata)
{
	uint8_t res = mdns_resp_add_service_txtitem(service, "A_D Electronics", 15);
	LWIP_ERROR("service add txt_userdata fail!", (res == ERR_OK), return NULL);
	return NULL;
}

static void mdns_report(struct netif* netif, u8_t result)
{
  printf(("mdns status[netif %d]: %d\n", netif->num, result));
}

void lwip_app_mdns(void *param)
{
	// Set NETIF_FLAG_IGMP flag for netif which should process IGMP messages
	xnetif[0].flags |= NETIF_FLAG_IGMP;

	vTaskDelay(2000);
	printf("\r\nmDNS Init...\r\n");

	mdns_resp_register_name_result_cb(mdns_report);
	mdns_resp_init();
	mdns_resp_add_netif(&xnetif[0], xnetif[0].hostname, 60);
	mdns_resp_add_service(&xnetif[0], "CV-Meter-Test", "_cvmeter", DNSSD_PROTO_TCP, 5000, 60, (service_get_txt_fn_t)srv_txt, NULL);
	//mdns_resp_netif_settings_changed(&xnetif[0]);
	mdns_resp_announce(&xnetif[0]);
	//printf("MTU: %u\r\n", xnetif[0].mtu);

	for(;;)
	{
		vTaskDelay(10);
	}
	vTaskDelete(NULL);
}

void example_lwip_app_mdns(void)
{
	if(xTaskCreate(lwip_app_mdns, (char const *)"mdns task", 1024, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
	{
		printf("\n\r[%s] Create update task failed", __FUNCTION__);
	}
}

#endif
