#pragma once
#include "../proto/packets.pb.h"
#include <arpa/inet.h>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <google/protobuf/message.h>

int send_packet(int sock, int id, Type type, google::protobuf::Message *message);

struct recv_handlers {
    int (*init_request)(int sock, int id, google::protobuf::Message *message);
};

int handle_recv(int sock, recv_handlers &handlers);
