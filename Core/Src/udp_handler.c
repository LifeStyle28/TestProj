#include "udp_handler.h"
#include <string.h>


struct udp_pcb* udp_create_socket(const ip4_addr_t ip_addr, const u16_t port, udp_recv_fn recv, void *recv_arg)
{
	// создание сокета
	struct udp_pcb* upcb = udp_new();

	// если не удалось создать сокет, то возвращаем NULL
	if (upcb == NULL)
	{
		return NULL;
	}

	// привязка к локальному ИП и порту
	if (ERR_OK != udp_bind(upcb, &ip_addr, port))
	{
		udp_remove(upcb);
		return NULL;
	}

	// регистрируем колбэк на прием пакета
	udp_recv(upcb, recv, recv_arg);
	return upcb;
}

err_t udp_send_msg_to(struct udp_pcb* upcb, const char* data, const ip_addr_t *dst_ip, u16_t dst_port)
{
	// если сокет не создан, то на выход с ошибкой
	if (upcb == NULL)
	{
		return ERR_ABRT;
	}

	size_t data_size = strlen(data);

	// аллоцируем память под буфер с данными
	struct pbuf* p = pbuf_alloc(PBUF_TRANSPORT, data_size, PBUF_RAM);

	if (p == NULL)
	{
		return ERR_MEM;
	}

	err_t err_result;
	err_t err = pbuf_take(p, data, data_size);
	if (ERR_OK != err)
	{
		err_result = err;
	} else {
		// отсылаем пакет
		err_result = udp_sendto(upcb, p, dst_ip, dst_port);
	}
	// очищаем аллоцированную память
	pbuf_free(p);
	return err_result;
}
