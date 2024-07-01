#ifndef __CAN_FRAME_RECV_HANDLER__H__
#define __CAN_FRAME_RECV_HANDLER__H__

#include "isotp.h"

typedef void (*recv_handler_t)(const can_frame_t* frame);

static recv_handler_t recv_handler[] = {
  isotp_can_recv
};

#endif  //!__CAN_FRAME_RECV_HANDLER__H__