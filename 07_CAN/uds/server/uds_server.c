#include "uds_server.h"
#define UDS_TARGET_ADDR 0x719
#define UDS_SOURCE_ADDR 0x791
#include "conf/uds_conf.h"

#include "tp/mock_tp.h"

#include <string.h>
#include <stdbool.h>
#include <stdio.h>

static uint16_t resp_len;
static uint8_t resp_buff[512];

void tp_recv_handler(uint32_t id, const uint8_t *data, uint16_t len)
{
  printf("0x%x | RX | %u | ", id, len);
  for (uint16_t i = 0; i < len; ++i) {
    printf("%2x ", data[i]);
  }
  printf("\n");
  // TODO: check id before processing
  uint8_t ret;
  uint32_t resp;
  switch (data[0])
  {
  case ECU_RESET_SID:
    ret = ECU_reset_Handler();
    if (ret == POSITIVE_RESPONE) {
      resp_buff[0] = ECU_RESET_SID + 0x40;
      resp_buff[1] = 0x01;
      resp_len = 2;
    } else {
      resp_buff[0] = NEGATIVE_RESPONE;
      resp_len = 1;
    }
    break;
  case DIAG_SESS_CTL_SID:
    ret = DiagSess_Ctl_Handler(data[1]);
    if (ret == POSITIVE_RESPONE) {
      resp_buff[0] = DIAG_SESS_CTL_SID + 0x40;
      resp_buff[1] = data[1];
      resp_len = 2;
    } else {
      resp_buff[0] = NEGATIVE_RESPONE;
      resp_len = 1;
    }
    break;
  case ROUTINE_CTL_SID:
    if (data[1] == 0xFE && data[2] == 0xF2) {
      ret = Check_Memory_Handler(&resp);
      if (ret == POSITIVE_RESPONE) {
        resp_buff[0] = ROUTINE_CTL_SID + 0x40;
        resp_buff[1] = 0xFE;
        resp_buff[2] = 0xF2;
        resp_buff[3] = (resp >> 24) & 0xFF;
        resp_buff[4] = (resp >> 16) & 0xFF;
        resp_buff[5] = (resp >> 8) & 0xFF;
        resp_buff[6] = resp & 0xFF;
        resp_len = 7;
      } else {
        resp_buff[0] = NEGATIVE_RESPONE;
        resp_len = 1;
      }
    } else {
      resp_buff[0] = NEGATIVE_RESPONE;
      resp_len = 1;
    }
    break;
  case REQUEST_DOWNLOAD_SID:

    ret = Request_Download_Handler(
      ((uint32_t)data[3] << 24 | (uint32_t)data[4] << 16) | (uint32_t)data[5] << 8 | (uint32_t)data[6],
      ((uint32_t)data[7] << 24 | (uint32_t)data[8] << 16) | (uint32_t)data[9] << 8 | (uint32_t)data[10],
      &resp);
    if (ret == POSITIVE_RESPONE) {
      resp_buff[0] = REQUEST_DOWNLOAD_SID + 0x40;
      resp_buff[1] = (resp >> 8) & 0xFF;
      resp_buff[2] = resp & 0xFF;
      resp_len = 3;
    } else {
      resp_buff[0] = NEGATIVE_RESPONE;
      resp_len = 1;
    }

    break;
  case TRANSFER_DATA_SID:
    ret = Transfer_Data_Handler(data[1], data + 2, len - 2);
    if (ret == POSITIVE_RESPONE) {
      resp_buff[0] = TRANSFER_DATA_SID + 0x40;
      resp_buff[1] = data[1];
      resp_len = 2;
    } else {
      resp_buff[0] = NEGATIVE_RESPONE;
      resp_len = 1;
    }

    break;
  case TRANSFER_EXIT_SID:
    ret = Transfer_Exit_Handler();
    if (ret == POSITIVE_RESPONE) {
      resp_buff[0] = TRANSFER_EXIT_SID + 0x40;
      resp_len = 1;
    } else {
      resp_buff[0] = NEGATIVE_RESPONE;
      resp_len = 1;
    }
    break;
  default:
    resp_buff[0] = NEGATIVE_RESPONE;
    resp_len = 1;
  }
  send_tp(UDS_TARGET_ADDR, resp_buff, resp_len);
}
