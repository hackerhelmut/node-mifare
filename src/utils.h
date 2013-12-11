#ifndef UTILS_H
#define UTILS_H

#include <node.h>
#include <v8.h>
#include <node_buffer.h>
#include <iostream>
#include <cstring>

using namespace v8;
using namespace node;

Local<Object> validResult(Local<Value> data);
Local<Object> validTrue();
Local<Object> errorResult(int no, const char *msg);
Local<Object> errorResult(int no, const std::string msg);
Local<Object> buffer(uint8_t *data, size_t len);

#endif // UTILS_H
