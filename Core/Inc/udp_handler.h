#ifndef INC_UDP_HANDLER_H_
#define INC_UDP_HANDLER_H_

#include "ip4_addr.h"
#include "err.h"

#include "udp.h"

//err_t udp_create_socket();
//err_t udp_send_msg();

//функция создания сокета
struct udp_pcb* udp_create_socket(const ip4_addr_t ip_addr, const u16_t port, udp_recv_fn recv, void *recv_arg);

// функция отправки сообщения
err_t udp_send_msg_to(struct udp_pcb* upcb, const char* data, const ip_addr_t *dst_ip, u16_t dst_port);

#endif
