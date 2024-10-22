#pragma once
#include "../common/io.h"
#include <condition_variable>
#include <mutex>
#include <string>

extern int requst_id;
extern std::map<int, std::string> messages;
extern std::map<int, std::condition_variable> conditions;
extern std::map<int, std::mutex> mutexes;

int connect(std::string host, int port);

int recv_thread(int sock);

template <typename T> int request_response(int sock, google::protobuf::Message &request, T *response) {
    int err = send_message(sock, ++requst_id, Type::INIT_REQUEST, &request);
    if (err < 0) {
        return -1;
    }
    std::unique_lock<std::mutex> lock(mutexes[requst_id]);
    conditions[requst_id].wait(lock);
    response->ParseFromString(messages[requst_id]);
    return 0;
}