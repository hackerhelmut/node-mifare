#include "utils.h"

Local<Object> validResult(Local<Value> data) {
  Local<Object> result = Object::New();
  result->Set(String::NewSymbol("err"), Array::New());
  result->Set(String::NewSymbol("data"), data);
  return result;
}

Local<Object> validTrue() {
  return validResult(Local<Boolean>::New(Boolean::New(true)));
}

Local<Object> errorResult(int no, const std::string msg) {
  return errorResult(no, msg.c_str());
}

Local<Object> errorResult(int no, const char *msg) {
  Local<Object> result = Object::New();
  Local<Array> errors = Array::New();
  Local<Object> error = Object::New();
  
  error->Set(String::NewSymbol("code"), Integer::New(no));
  error->Set(String::NewSymbol("msg"), String::New(msg));
  errors->Set(0, error);
  result->Set(String::NewSymbol("err"), errors);
  result->Set(String::NewSymbol("data"), Undefined());
  return result;
}

Local<Object> buffer(uint8_t *data, size_t len) {
  Buffer *slowBuffer = Buffer::New(len);
  memcpy(Buffer::Data(slowBuffer), data, len);

  Local<Object> result = Object::New();
  Local<Object> globalObj = Context::GetCurrent()->Global();
  Local<Function> bufferConstructor = Local<Function>::Cast(globalObj->Get(String::New("Buffer")));
  Handle<Value> constructorArgs[3] = { slowBuffer->handle_, v8::Integer::New(len), v8::Integer::New(0) };
  Local<Object> ndef = bufferConstructor->NewInstance(3, constructorArgs);
  result->Set(String::NewSymbol("ndef"), ndef);
  return result;
}


