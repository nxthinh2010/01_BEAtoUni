#ifndef __CAN_FRAME__H__
#define __CAN_FRAME__H__

#include <stdio.h>
#include <stdint.h>

#define CAN_FRAME_SIZE 8

typedef struct {
  uint32_t id;
  uint8_t* data;
  uint32_t len;
} can_frame_t;

void can_send(const can_frame_t* frame);
void can_poll(const can_frame_t* frame);

#endif  //!__CAN_FRAME__H__