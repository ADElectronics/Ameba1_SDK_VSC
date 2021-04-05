#include "lwip/netif.h"
#include "lwip_netconf.h"

#if CONFIG_EXAMPLE_LWIP_APP_MQTT
#include "apps/mqtt.h"

#define CLIENT_PIN_EXAMPLE		1
#define CLIENT_RESEND_EXAMPLE	1
#define CLIENT_ID_NAME			"lwip_test"
#define CLIENT_USER				"usr"//NULL if not used
#define CLIENT_PASSWORD 		"pwd"//NULL if not used
#define CLIENT_SUBSCRIBE_TOPIC	"cmd/test"

#if (CLIENT_RESEND_EXAMPLE == 1)
#define CLIENT_PUBLISH_TOPIC	"sts/test"

static char str_pub[20];
#endif

#if (CLIENT_PIN_EXAMPLE == 1)
#include "port_api.h"
#include "PortNames.h"

#define CLIENT_GPIO_PIN 		PA_3
#define CLIENT_GPIO_STATE_ON	"ON"
#define CLIENT_GPIO_STATE_OFF	"OFF"

static gpio_t gpio_led;
#endif

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status);
static void mqtt_sub_request_cb(void *arg, err_t result);
static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len);
static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags);
static void mqtt_pub_request_cb(void *arg, err_t result);

static mqtt_client_t *mqtt_client;
static ip_addr_t broker_addr;
static int inpub_id;

static void mqtt_client_try_connect(mqtt_client_t *client)
{
	struct mqtt_connect_client_info_t ci;
	err_t err;

	memset(&ci, 0, sizeof(ci));

	ci.client_id = CLIENT_ID_NAME;
	ci.client_user = CLIENT_USER;
	ci.client_pass = CLIENT_PASSWORD;
	err = mqtt_client_connect(client, &broker_addr, MQTT_PORT, mqtt_connection_cb, 0, &ci);

	if(err != ERR_OK)
		printf("mqtt_connect return %d\n", err);
}

static void mqtt_sub_request_cb(void *arg, err_t result)
{
	printf("Subscribe result: %d\n", result);
}

static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len)
{
	printf("Incoming publish at topic %s with total length %d\n", topic, tot_len);
	/* ... */
}

static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
	printf("Incoming publish payload with length %d, flags %d\n", len, flags);

#if (CLIENT_PIN_EXAMPLE == 1)
	if(strncmp(data, CLIENT_GPIO_STATE_ON, len) == 0)
		gpio_direct_write(&gpio_led, 1);
	else if(strncmp(data, CLIENT_GPIO_STATE_OFF, len) == 0)
		gpio_direct_write(&gpio_led, 0);
#else

#endif

#if (CLIENT_RESEND_EXAMPLE == 1)
	strncpy(str_pub, data, len);
	mqtt_publish(mqtt_client, CLIENT_PUBLISH_TOPIC, &str_pub, len, 0, 0, mqtt_pub_request_cb, NULL);
#endif
}

static void mqtt_pub_request_cb(void *arg, err_t result)
{
	//if(result != ERR_OK)
	//{
		printf("Publish result: %d\n", result);
	//}
}

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
	err_t err;
	if(status == MQTT_CONNECT_ACCEPTED)
	{
		printf("mqtt_connection_cb: Successfully connected\n");
		mqtt_set_inpub_callback(client, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, arg);
		err = mqtt_subscribe(client, CLIENT_SUBSCRIBE_TOPIC, 1, mqtt_sub_request_cb, arg);

		if(err != ERR_OK)
		{
			printf("mqtt_subscribe return: %d\n", err);
		}
	}
	else
	{
		printf("mqtt_connection_cb: Disconnected, reason: %d\n", status);
		mqtt_client_try_connect(client);
	}
}

void lwip_app_mqtt(void *param)
{
	vTaskDelay(6000);
	printf("\r\nMQTT Init...\r\n");
	IP4_ADDR(&broker_addr, 192, 168, 1, 110);

#if (CLIENT_PIN_EXAMPLE == 1)
	gpio_init(&gpio_led, CLIENT_GPIO_PIN);
	gpio_dir(&gpio_led, PIN_OUTPUT);
	gpio_mode(&gpio_led, PullNone);
#endif

	mqtt_client = mqtt_client_new();
	if(mqtt_client != NULL)
	{
		mqtt_client_try_connect(mqtt_client);
	}

	for(;;)
	{
		vTaskDelay(10);
	}
	vTaskDelete(NULL);
}

void example_lwip_app_mqtt(void)
{
	if(xTaskCreate(lwip_app_mqtt, (char const *)"mqtt task", 1024, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
	{
		printf("\n\r[%s] Create update task failed", __FUNCTION__);
	}
}

#endif
