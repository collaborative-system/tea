#pragma once
#include "../common/tcp.h"
#include "../common/handlers.h"
#include <string>
#include <unistd.h>

int listen(int port, recv_handlers fs, recv_handlers event);

int close();
