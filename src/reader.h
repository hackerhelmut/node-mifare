// Copyright 2013, Rolf Meyer
// See LICENCE for more information
#ifndef READER_H
#define READER_H

#include <node.h>
#include <v8.h>
#include <node_buffer.h>
#include <vector>
#include <iostream>
#include <cstring>

#ifdef __APPLE__
#include <PCSC/winscard.h>
#include <PCSC/wintypes.h>
#else
#include <winscard.h>
#endif
#include <freefare_pcsc.h>
#include <cstdlib>


using namespace v8;
using namespace node;

struct reader_data {
  /**
   * Create a new reader status instance
   * @param name The name of the reader
   * @return The SCARD_READERSTATE object representating this object
   */
  reader_data(const char* name, pcsc_context *hContext) {
    this->name = std::string(name);
    this->state.szReader = this->name.c_str();
    this->state.dwCurrentState = SCARD_STATE_UNAWARE;
    this->state.pvUserData = this;
    this->timer.data = this;
    this->context = hContext;
  }

  std::string name;
  uv_timer_t timer;
  SCARD_READERSTATE state;
  pcsc_context *context;
  Persistent<Function> callback;
  Persistent<Object> self;
};

void reader_timer_callback(uv_timer_t *handle, int timer_status);
Handle<Value> ReaderListen(const Arguments& args);
Handle<Value> ReaderSetLed(const Arguments& args);

#endif // READER_H
