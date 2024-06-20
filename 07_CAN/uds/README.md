# Preparation

Create named pipe

```bash
mkfifo cli_to_srv
mkfifo srv_to_cli
```

Change path to fifo (absolute path) in `tp/mock_tp.c`

# How to build

```bash
cd <root>

#build Client
gcc -I. tp/mock_tp.c client/uds_client.c client/test_client.c -o test_cli -lpthread -DCLI=1 -DUDS_TARGET_ADDR=0x791 -DUDS_SOURCE_ADDR=0x719

#build Server
gcc -I. tp/mock_tp.c server/uds_server.c server/test_server.c -o test_srv -lpthread -DCLI=0 -DUDS_TARGET_ADDR=0x719 -DUDS_SOURCE_ADDR=0x791
```

# How to run

./test_cli

./test_srv