#include "udp_handler.h"

#include "udp.h"

#include <string.h>

static void udp_receive_callback(
	void *arg,
	struct udp_pcb *pcb,
	struct pbuf *p,
	const ip_addr_t *addr,
	u16_t port) {

	/*                  */
	/* Обработка данных */
	/*                  */

	pbuf_free(p);

	/* Пришёл ответ ­— помигай! */
	/* LD2 */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_Delay(250u);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
}

static struct udp_pcb* make_sock(
	udp_recv_fn recv_cb,
	void* const recv_arg) {

	struct udp_pcb* upcb = udp_new();

	if (NULL == upcb) {
		return NULL;
	}

	udp_recv(upcb, recv_cb, recv_arg);
	return upcb;
}

static err_t set_remote(
	struct udp_pcb* const upcb,
	const ip4_addr_t addr,
	const u16_t port) {

	const err_t err = udp_connect(upcb, &addr, port);
	return err;
}

static err_t udp_send_msg(
	struct udp_pcb* const upcb,
	const char* const data)
{
	if (NULL == upcb || NULL == data) {
		return ERR_ABRT;
	}

	const size_t data_size = strlen(data);

	struct pbuf* const p = pbuf_alloc(
		PBUF_TRANSPORT,
		data_size,
		PBUF_RAM
	);

	if (NULL == p) {
		return ERR_ABRT;
	}

	err_t err = pbuf_take(p, data, data_size);

	if (ERR_OK != err) {
		pbuf_free(p);
		return err;
	}

	err = udp_send(upcb, p);
	if (ERR_OK != err) {
		pbuf_free(p);
		return err;
	}

	pbuf_free(p);
	return ERR_OK;
}

void fill_report(char* const storage) {
	static u8_t counter = 1u;
	sprintf(storage, "Hello world! x%d", counter);

	if (32u != counter) {
		++counter;
	} else {
		counter = 1u;
	}
}

void report(void) {
	static struct udp_pcb* upcb = NULL;

	if (NULL == upcb) {
		upcb = make_sock(udp_receive_callback, NULL);

		if (NULL == upcb) {
			return;
		}
	}

	static ip4_addr_t addr = {.addr = 0u};
	static const u16_t port = 3333u;

	if (!addr.addr) {
		IP4_ADDR(&addr, 192u, 168u, 0u, 11u);
	}

	if (ERR_OK != set_remote(upcb, addr, port)) {
		return;
	}

	char msg[32u] = {'\0'};

	/* Докладываем серверу положение дел (создаём отчёт) */
	fill_report(msg);
	udp_send_msg(upcb, msg);
}
