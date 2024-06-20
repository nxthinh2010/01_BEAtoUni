#include "uds_client.h"

#include "tp/mock_tp.h"

#include <stdio.h>
#include <stdlib.h>

#define HANDLE_RESP(x) { \
  if (POSITIVE_RESPONE != (x)) {\
    perror("ERROR\n");\
    exit(-1);\
  }\
}

int main() {
  tp_init();

  HANDLE_RESP(DiagSess_Ctl(DIAG_PROGRAM_SESS));
  HANDLE_RESP(ECU_reset());

  // wait short time for ECU reset

  uint32_t block_len = 0;
  uint32_t byte_to_send = 0xFA00;
  uint32_t offset = 0;
  uint8_t seq = 0;
  uint32_t start_addr = 0x800000;
  uint8_t hex_byte[0xFA00];
  uint32_t crc = 0;

  for (uint32_t i = 0; i < 0xFA00; ++i) {
    hex_byte[i] = i;
  }

  HANDLE_RESP(Request_Download(start_addr, byte_to_send, &block_len));
  while (offset < byte_to_send) {
    if (offset + block_len > byte_to_send) {
      block_len = byte_to_send - offset;
    }
    HANDLE_RESP(Transfer_Data(seq, hex_byte + offset, block_len));
    seq++;
    offset += block_len;
  }

  HANDLE_RESP(Transfer_Exit());
  HANDLE_RESP(Check_Memory(&crc));

  printf("my crc 0x%x\n", crc);

  // If CRC match then reset ECU again else try to transfer again
  HANDLE_RESP(DiagSess_Ctl(DIAG_DEFAULT_SESS));
  HANDLE_RESP(ECU_reset());

  return 0;
}