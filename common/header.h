#pragma once
#include <cstddef>
#include <cstdint>
#include <arpa/inet.h>
#include <cstring>

struct Header{
  int32_t size;
  int32_t id;
  int32_t type;
};

char* serialize(Header* header);

Header* deserialize(char* buffer);
