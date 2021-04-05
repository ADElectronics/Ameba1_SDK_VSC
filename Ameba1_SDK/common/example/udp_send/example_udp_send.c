#include "lwip/pbuf.h"
#include "lwip/udp.h"

#if CONFIG_EXAMPLE_UDP_SEND

#define UDP_PORT		55045
#define UDP_PBUF_LEN	10

void udp_send_task(void *param)
{
	struct udp_pcb *upcb;
	struct pbuf *pb;
	uint8_t buf[UDP_PBUF_LEN];
	ip_addr_t servIP;

	printf("\n\r Waiting for 10 sec...\n\r");
	vTaskDelay(10000);

	for(uint16_t i=0;i<UDP_PBUF_LEN;i++) //TODO: Переделать....
		buf[i] = i;

	upcb = udp_new();
	IP4_ADDR(&servIP, 192, 168, 1, 139);

	if (upcb)
	{
		for (;;)
		{
			vTaskDelay(1000);
			printf("Try send...\n\r");
			pb = pbuf_alloc(PBUF_TRANSPORT, UDP_PBUF_LEN, PBUF_RAM);
			memcpy(pb->payload, &buf, UDP_PBUF_LEN);
			udp_connect(upcb, &servIP, UDP_PORT);
			udp_send(upcb, pb);
			udp_disconnect(upcb);
			pbuf_free(pb);
		}
	}
	printf("Error, no free mem for udp_pcb...\n\r");
	vTaskDelete(NULL);
}

void example_udp_send(void)
{
	if(xTaskCreate(udp_send_task, (char const *)"udp_send_task", 1024, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
	{
		printf("\n\r[%s] Create update task failed", __FUNCTION__);
	}
}

#endif
