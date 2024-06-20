#include "mock_tp.h"

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

static int fd_send;
static int fd_recv;

#if CLI==1
const char* send_pipe = "/home/nda9hc/learning/uds/cli_to_srv";
const char* recv_pipe = "/home/nda9hc/learning/uds/srv_to_cli";
#else
const char* send_pipe = "/home/nda9hc/learning/uds/srv_to_cli";
const char* recv_pipe = "/home/nda9hc/learning/uds/cli_to_srv";
#endif

pthread_t thread;

void *tp_poll(void* arg) {
  uint8_t buff[1024];
  fd_recv = open(recv_pipe, O_RDONLY);
  printf("fd_recv %d\n", fd_recv);
  while(1) {
    int nread = read(fd_recv, buff, 1024);
    if (nread > 0) {
      uint32_t id;
      memcpy(&id, buff, 4);
      tp_recv_handler(id, buff + 4, nread - 4);
    }
  }
}

void tp_init() {
  pthread_create(&thread, NULL, &tp_poll, NULL);
  fd_send = open(send_pipe, O_WRONLY);
  printf("fd_send %d\n", fd_send);
}

void send_tp(uint32_t id, uint8_t* data, uint16_t len) {
  printf("0x%x | TX | %u | ", id, len);
  for (uint16_t i = 0; i < len; ++i) {
    printf("%2x ", data[i]);
  }
  printf("\n");
  uint8_t buff[1024];
  memcpy(buff, &id, 4);
  memcpy(buff + 4, data, len);
  write(fd_send, buff, len + 4);
}