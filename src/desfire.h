// Copyright 2013, Rolf Meyer
// See LICENCE for more information
#ifndef DESFIRE_H
#define DESFIRE_H

#include <node.h>
#include <v8.h>
#include <vector>
#include <iostream>
#include <cstring>

#ifdef __APPLE__
#include <PCSC/winscard.h>
#include <PCSC/wintypes.h>
#else
#include <winscard.h>
#endif
#include <freefare.h>

#include "reader.h"
#include <cstdlib>

using namespace v8;
using namespace node;

struct card_data {
  card_data(reader_data *reader) : reader(reader) {
    uint8_t null[8] = {0,0,0,0,0,0,0,0};
    key = mifare_desfire_des_key_new(null);
    aid = mifare_desfire_aid_new(0x000001);
  }
  ~card_data() {
    if(key) {
      mifare_desfire_key_free(key);
      key = NULL;
    }
    if(aid) {
      free(aid);
      aid = NULL;
    }
  }
  reader_data *reader;
  MifareTag tag;
  MifareDESFireKey key;
  MifareDESFireAID aid;
};


Handle<Value> CardInfo(const Arguments& args);

Handle<Value> CardMasterKeyInfo(const Arguments& args);

Handle<Value> CardName(const Arguments& args);

Handle<Value> CardKeyVersion(const Arguments& args);

Handle<Value> CardFreeMemory(const Arguments& args);

Handle<Value> CardSetAid(const Arguments& args);

Handle<Value> CardSetKey(const Arguments & args);

Handle<Value> CardFormat(const Arguments& args);

Handle<Value> CardCreateNdef(const Arguments &args);

/**
 * Helper function to locate and read TVL of a desfire ndef sector
 * @return Might return a result object. This is only used when res is lesser 0 otherwise the object is empty.
 */
Local<Object> CardReadNdefTVL(card_data *data, int &res, uint8_t &file_no, uint16_t &ndefmaxlen, MifareDESFireKey key_app);

Handle<Value> CardReadNdef(const Arguments& args);

Handle<Value> CardWriteNdef(const Arguments& args);

Handle<Value> CardCreateNdef(const Arguments& args);

#endif // DESFIRE_H
