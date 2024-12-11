#pragma once
#include "../common/tcp.h"
#include "../common/log.h"
#include <condition_variable>
#include <mutex>
#include <string>

extern int requst_id;
extern std::map<int, std::string> messages;
extern std::map<int, std::condition_variable> conditions;
extern std::mutex condition_mutex;

int connect(std::string host, int port);

recv_handlers get_fs_handlers();

recv_handlers get_event_handlers();

template <typename T> int request_response(int sock, google::protobuf::Message &request, T *response, Type type) {
    int id = ++requst_id;
    int err = send_message(sock, id, type, &request);
    if (err < 0) {
        return -1;
    }
    std::unique_lock<std::mutex> lock(condition_mutex);
    if (conditions.find(id) == conditions.end()) {
        conditions[id].wait(lock);
    }
    response->ParseFromString(messages[id]);
    return 0;
}
