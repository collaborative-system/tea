CC := g++
C_FLAGS := -g -Wall -Wextra -pedantic -std=c++20 `pkg-config --cflags --libs protobuf` -pthread
FS_FLAGS := -lfuse3 -D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=30 -Wno-missing-field-initializers
SERVER_FLAGS := 
COMMON := common/log.cpp common/io.cpp common/header.cpp
SERVER_FILES := server/tcp.cpp server/fs.cpp
FS_FILES := filesystem/tcp.cpp filesystem/fs.cpp filesystem/log.cpp
PROTO := proto/messages.proto

all: build

.PHONY: build
build: filesystem server

.PHONY: clean
clean: filesystem-clean server-clean

.PHONY: clean-all
clean-all: clean proto-clean

.PHONY: filesystem-run
filesystem-run: filesystem 
	./filesystem/filesystem --host=127.0.0.1 -f tests/

.PHONY: filesystem 
filesystem: filesystem/filesystem

filesystem/filesystem: proto/proto.pb.o
	$(CC) $(C_FLAGS) $(FS_FLAGS) -o $@ filesystem/main.cpp $(COMMON) $(FS_FILES) proto/proto.pb.o

.PHONY: filesystem-clean
filesystem-clean:
	rm -f filesystem/filesystem

.PHONY: server-run
server-run: server
	./server/server ~/name

.PHONY: server
server: server/server

server/server: proto/proto.pb.o
	$(CC) $(C_FLAGS) $(SERVER_FLAGS) -o $@ server/main.cpp $(COMMON) $(SERVER_FILES) proto/proto.pb.o

.PHONY: server-clean
server-clean:
	rm -f server/server

.PHONY: proto
proto: proto/proto.pb.o

proto/proto.pb.o:
	protoc -I=proto/ --cpp_out=proto/ $(PROTO)
	$(CC) $(CFLAGS) -g `pkg-config --cflags --libs protobuf` -c proto/*.pb.cc -o proto/proto.pb.o

.PHONY: proto-clean
proto-clean:
	rm -rf proto/proto.pb.o proto/*.pb.*
