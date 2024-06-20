#include "uds_server.h"
#include "tp/mock_tp.h"

#include <stdio.h>

// Soft reset ecu
resp_code_t ECU_reset_Handler() {
  printf("handling ecu reset\n");
  return POSITIVE_RESPONE;
}

// To jump to programing or application
resp_code_t DiagSess_Ctl_Handler(diag_sess_t session) {
  printf("handling session control %u\n", session);
  return POSITIVE_RESPONE;
}

// Preparation for download new SW
resp_code_t Request_Download_Handler(uint32_t mem_addr, uint32_t mem_size, uint32_t* block_len) {
  printf("handling request download 0x%x 0x%x\n", mem_addr, mem_size);
  *block_len = 128;
  return POSITIVE_RESPONE;
}

// Transfer data
resp_code_t Transfer_Data_Handler(uint8_t seq_cnt, const uint8_t* data, uint32_t block_size) {
  printf("handling transfer data seq_cnt %u block size %u\n", seq_cnt, block_size);
  return POSITIVE_RESPONE;
}

// Complete transfer
resp_code_t Transfer_Exit_Handler() {
  printf("handling transfer exit\n");
  return POSITIVE_RESPONE;
}

// Check memory integrity
resp_code_t Check_Memory_Handler(uint32_t* crc) {
  printf("handling check mem\n");
  *crc = 0x7777;
  return POSITIVE_RESPONE;
}

int main() {
  tp_init();
  while(1) {

  }
  return 0;
}