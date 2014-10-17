// Copyright 2013, Rolf Meyer
// See LICENCE for more information

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

#include "reader.h"

using namespace v8;
using namespace node;

/**
 * plugin global secure card context
 **/
static pcsc_context *context;
static std::vector<reader_data *> readers_data;

/**
 * Get Names of the Readers connected to the computer
 * @param hContext The SCard Context used to search
 * @return An Array of Strings with reader names
 **/
Handle<Value> getReader(const Arguments& args) {
  LONG res;
  char *reader_names;
  char *reader_iter = NULL;
  int   reader_count = 0;

  HandleScope scope;
  Persistent<Object> readers;

  if(args.Length() > 0){
    ThrowException(Exception::TypeError(String::New("This function does not take any arguments")));
    return scope.Close(Undefined());
  }

  // Allocate buffer. We assume autoallocate is not present (on Mac OS X anyway)
  res = pcsc_list_devices(context, &reader_names);
  if(res != SCARD_S_SUCCESS || reader_names[0] == '\0') {
    //readers = Persistent<Array>::New(Array::New(0));
    readers = Persistent<Object>::New(Object::New());
    //delete [] reader_names;
    ThrowException(Exception::TypeError(String::New("Unable to list readers")));
    return scope.Close(Undefined());
  }

  // Clean before use
  for(std::vector<reader_data *>::iterator iter;iter!=readers_data.end();iter++) {
    reader_data *data = *iter;
    delete [] data->state.szReader;
    delete data;
  }
  readers_data.clear();
  readers = Persistent<Object>::New(Object::New());

  // Get number of readers from null separated string
  reader_iter = reader_names;
  while(*reader_iter != '\0') {
    // Node Object:
    readers_data.push_back(new reader_data(reader_iter, context));
    Local<External> data = Local<External>::New(External::New(readers_data.back()));
    Local<Object> reader = Local<Object>::New(Object::New());
    reader->Set(String::NewSymbol("name"), String::New(reader_iter));
    reader->Set(String::NewSymbol("listen"), FunctionTemplate::New(ReaderListen)->GetFunction());
    reader->Set(String::NewSymbol("setLed"), FunctionTemplate::New(ReaderSetLed)->GetFunction());
    reader->SetHiddenValue(String::NewSymbol("data"), data);
    readers->Set(String::NewSymbol(reader_iter), reader);
    reader_iter += strlen(reader_iter)+1;
    reader_count++;
  }
  //delete [] reader_names;

  return scope.Close(readers); 
}

/**
 * Node.js initialization function
 * @param exports The Commonjs module exports object
 **/
void init(Handle<Object> exports) {
  pcsc_init(&context);
  if(!context) {
    ThrowException(Exception::TypeError(String::New("Cannot establish context")));
    return; 
  }

  exports->Set(String::NewSymbol("getReader"),
      FunctionTemplate::New(getReader)->GetFunction());
	// AtExit(&PCSC::close);
}

NODE_MODULE(node_mifare, init)
