#include "can_frame.h"
#include "can_frame_def.h"

void can_send(const can_frame_t* frame) {
  printf("0x%x TX %u ", frame->id, frame->len);
  for (int i = 0; i < frame->len; ++i) {
    printf("%02X ", frame->data[i]);
  }
  printf("\n");
  can_poll(frame);
}

void can_poll(const can_frame_t* frame) {
  for (int i = 0; i < sizeof(recv_handler)/sizeof(recv_handler_t); ++i) {
    recv_handler[i](frame);
  }
}