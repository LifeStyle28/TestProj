#ifndef INC_UDP_HANDLER_H_
#define INC_UDP_HANDLER_H_

#include "ip4_addr.h"
#include "err.h"

#include "udp.h"

err_t udp_send_msg(struct udp_pcb* upcb, const char* data); ///< функция отправки сообщения

struct udp_pcb* udp_create_s(struct udp_pcb* upcb, const ip4_addr_t ip_addr, const u16_t port, udp_recv_fn recv, void *recv_arg);

err_t udp_create_socket_atalon(); ///< функция создания сокета
err_t udp_send_msg_atalon(); ///< функция отправки сообщения


#endif
