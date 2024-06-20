#ifndef MOCK_TP_H
# define MOCK_TP_H

#include <stdint.h>


void tp_recv_handler(uint32_t id, const uint8_t* data, uint16_t len);
void send_tp(uint32_t id, uint8_t* data, uint16_t len);

void tp_init();

#endif