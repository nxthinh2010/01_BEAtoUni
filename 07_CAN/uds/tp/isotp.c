#include <string.h>
#include "isotp.h"

// This can be extended to have more than one link i.e,. func_addr and rx_link
static isotp_link_t* link;

void set_isotp_link(isotp_link_t* tplink) {
  link = tplink;
  link->total_byte = 0;
}

// Function to send ISO-TP single frame
void send_single_frame(uint32_t id, const uint8_t *data, uint32_t len)
{
  uint8_t payload[CAN_FRAME_SIZE];
  payload[0] = len; // DLC (Data Length Code)
  memcpy(payload + 1, data, 7);
  can_frame_t frame = {id, payload, len + 1};
  can_send(&frame);
}

// Send first frame
void send_first_frame(uint32_t id, const uint8_t *data, uint32_t len)
{
  uint8_t payload[CAN_FRAME_SIZE];
  payload[0] = (0x10 | len >> 8); // should limit len <= FFF
  payload[1] = (len & 0xFF);

  memcpy(payload + 2, data, 6);
  can_frame_t frame = {id, payload, 8};
  can_send(&frame);
}

// Function to send ISO-TP multi frame
void send_multi_frame(uint32_t id, const uint8_t *data, uint32_t len)
{
  uint8_t payload[CAN_FRAME_SIZE];
  uint8_t seq_num = 1;
  uint16_t remaining_bytes = len;
  uint8_t offset = 0;

  while (remaining_bytes > 0)
  {
    uint8_t bytes_to_send = (remaining_bytes > CAN_FRAME_SIZE - 1) ? CAN_FRAME_SIZE - 1 : remaining_bytes;
    payload[0] = (0x20 | (seq_num & 0x0F));

    memcpy(payload + 1, data + offset, bytes_to_send);

    can_frame_t frame = {id, payload, bytes_to_send + 1};
    can_send(&frame);

    seq_num++;
    offset += bytes_to_send;
    remaining_bytes -= bytes_to_send;
  }
}

void send_isotp(uint32_t id, const uint8_t *data, uint32_t len)
{
  if (len <= 7)
  {
    send_single_frame(id, data, len);
  }
  else
  {
    send_first_frame(id, data, len);
    send_multi_frame(id, data + 6, len - 6);
  }
}

void handle_single_frame(const can_frame_t *frame) {
  link->recv_cb(frame->id, frame->data + 1, (frame->data[0] & 0x07));
}

//
void handle_first_frame(const can_frame_t *frame) {
  link->total_byte = ((uint32_t)(frame->data[0] & 0x0F) << 8) | frame->data[1];
  memcpy(link->rx_buff, frame->data + 2, frame->len - 2);
  link->recv_byte = 6;
  link->seq_num = 1;
  printf("expected %u\n", link->total_byte);
}

void handle_consec_frame(const can_frame_t *frame) {
  if (link->seq_num == (frame->data[0] & 0x0F)) {
    memcpy(link->rx_buff + link->recv_byte, frame->data + 1, frame->len - 1);
    link->recv_byte += frame->len - 1;
    link->seq_num++;
    link->seq_num &= 0x0F;
    if (link->recv_byte == link->total_byte) {
      link->recv_cb(link->rx_addr, link->rx_buff, link->total_byte);
      link->total_byte = 0;
      link->recv_byte = 0;
    }
  }
}

void isotp_can_recv(const can_frame_t *frame)
{
  if (link->rx_addr == frame->id)
  {
    if (frame->data[0] >> 4 == 0) // single frame
    {
      handle_single_frame(frame);
    }
    else if (frame->data[0] >> 4 == 1) // first frame
    {
      handle_first_frame(frame);
    }
    else if (frame->data[0] >> 4 == 2) // consecutive frame
    {
      handle_consec_frame(frame);
    }
  }
}