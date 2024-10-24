#include "io.h"
#include "../proto/packets.pb.h"
#include "header.h"
#include "log.h"
#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <google/protobuf/message.h>

int send_packet(int sock, int id, Type type, google::protobuf::Message *message) {
    Header header;
    header.size = message->ByteSizeLong();
    header.id = id;
    header.type = type;
    char *header_buffer = serialize(&header);

    char packet_buffer[sizeof(Header) + message->ByteSizeLong()];
    memcpy(packet_buffer, header_buffer, sizeof(Header));
    delete[] header_buffer;

    char *message_buffer = new char[message->ByteSizeLong()];
    bool err = message->SerializeToArray(message_buffer, message->ByteSizeLong());
    if (!err) {
        log(DEBUG, sock, "(%d) Serialize message failed", id);
        return -1;
    }
    memcpy(packet_buffer + sizeof(Header), message_buffer, message->ByteSizeLong());
    delete[] message_buffer;
    int len = send(sock, message_buffer, message->ByteSizeLong(), 0);
    if (len < 0) {
        log(DEBUG, sock, "(%d) Send message failed: %s", id, strerror(errno));
        return -1;
    }
    std::string debug = message->DebugString();
    debug.pop_back();
    log(DEBUG, sock, "(%d) Send message success: %s - %d bytes", id, debug.c_str(), len);
    return len;
}

int full_read(int fd, char *buf, int size) {
    int recived = 0;
    while (recived < size) {
        int len = recv(fd, buf + recived, size - recived, 0);
        if (len == 0) {
            log(DEBUG, fd, "EOF: %s", strerror(errno));
            return 0;
        }
        if (len < 0) {
            log(DEBUG, fd, "Full read failed: %s", strerror(errno));
            return -1;
        }
        recived += len;
    }
    return recived;
}

// Handle recv packets, 1 on success, 0 on EOF, -1 on error
int handle_recv(int sock, recv_handlers &handlers) {
    Header *header;
    char buffer[sizeof(Header)];
    int recived = full_read(sock, buffer, sizeof(buffer));
    if (recived == 0) {
        return 0;
    }
    if (recived < (int)sizeof(buffer)) {
        log(DEBUG, sock, "Full header read failed");
        return -1;
    }
    header = deserialize(buffer);
    log(DEBUG, sock, "Received header: size %d id %d type %d %d bytes", header->size, header->id, header->type, recived);
    char *recv_buffer = new char[header->size];
    recived = full_read(sock, recv_buffer, header->size);
    if (recived == 0 && header->size != 0) {
        return 0;
    }
    if (recived < header->size) {
        return -1;
    }
    int ret = -2;
    std::string message = "";
    switch (header->type) {
    case Type::INIT_REQUEST: {
        InitRequest request;
        request.ParseFromArray(recv_buffer, header->size);
        message = request.DebugString();
        ret = handlers.init_request(sock, header->id, &request);
        break;
    }
    default:
        log(DEBUG, sock, "(%d) Unknown packet type: %d", header->id, header->type);
        break;
    }
    if (message != "") {
        log(DEBUG, sock, message.c_str());
    }
    if (ret < 0) {
        log(DEBUG, sock, "Handler failed: %d", ret);
    } else {
        log(DEBUG, sock, "Handler success: %d", ret);
    }
    delete header;
    delete[] recv_buffer;
    return ret;
};
