#pragma once
#include <cstddef>
#include <cstdint>
#include <arpa/inet.h>
#include <cstring>

const int HEADER_SIZE = 12;

struct Header{
  int32_t size;
  int32_t id;
  int32_t type;
};

char* serialize(Header* header);

Header* deserialize(char* buffer);
