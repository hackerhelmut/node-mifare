// Copyright 2013, Rolf Meyer
// See LICENCE for more information

#include "reader.h"
#include "desfire.h"

void reader_timer_callback(uv_timer_t *handle, int timer_status) {
  HandleScope scope;
  reader_data *data = static_cast<reader_data *>(handle->data);
  LONG res;
  DWORD event;
  Local<String> status;
  Local<Object> error = Object::New();
  Local<Object> reader = Object::New();

  res = SCardGetStatusChange(data->context->context, 0, &data->state, 1);
  if(res == SCARD_S_SUCCESS) {
    event = data->state.dwEventState;
    if(event & SCARD_STATE_CHANGED) {
      data->state.dwCurrentState = event;
      if(event & SCARD_STATE_IGNORE) {
        status = v8::String::New("SCARD_STATE_IGNORE");
      } else if(event & SCARD_STATE_UNKNOWN) {
        status = v8::String::New("SCARD_STATE_UNKNOWN");
      } else if(event & SCARD_STATE_UNAVAILABLE) {
        status = v8::String::New("SCARD_STATE_UNAVAILABLE");
      } else if(event & SCARD_STATE_EMPTY) {
        status = v8::String::New("SCARD_STATE_EMPTY");
      } else if(event & SCARD_STATE_PRESENT) {
        status = v8::String::New("SCARD_STATE_PRESENT");
      } else if(event & SCARD_STATE_ATRMATCH) {
        status = v8::String::New("SCARD_STATE_ATRMATCH");
      } else if(event & SCARD_STATE_EXCLUSIVE) {
        status = v8::String::New("SCARD_STATE_EXCLUSIVE");
      } else if(event & SCARD_STATE_INUSE) {
        status = v8::String::New("SCARD_STATE_INUSE");
      } else if(event & SCARD_STATE_MUTE) {
        status = v8::String::New("SCARD_STATE_MUTE");
      }

      // Prepare readerObject event
      reader->Set(String::NewSymbol("name"), String::New(data->state.szReader));
      reader->Set(String::NewSymbol("status"), v8::Local<v8::Value>::New(status));

      // Card object, will be eventually filled lateron
      if(event & SCARD_STATE_PRESENT) {
        if(data->state.cbAtr>0) {
        }

        // Establishes a connection to a smart card contained by a specific reader.
        MifareTag *tags = freefare_get_tags_pcsc(data->context, data->state.szReader);
        for(int i = 0; (!res) && tags[i]; i++) {
          if(freefare_get_tag_type(tags[i])==DESFIRE) {

            //res = mifare_desfire_connect(tags[i]);
            //if(res) {
            //  error->Set(String::NewSymbol("msg"), String::New("Can't conntect to Mifare DESFire target."));
            //  error->Set(String::NewSymbol("code"), Integer::New(res));
            //  continue;
            //}
            card_data *cardData = new card_data(data);
            cardData->tag = tags[i];
            Local<Object> card = Object::New();
            card->Set(String::NewSymbol("type"), String::New("DESFIRE"));
            card->SetHiddenValue(String::NewSymbol("data"), External::Wrap(cardData));

            card->Set(String::NewSymbol("info"), FunctionTemplate::New(CardInfo)->GetFunction());
            card->Set(String::NewSymbol("masterKeyInfo"), FunctionTemplate::New(CardMasterKeyInfo)->GetFunction());
            card->Set(String::NewSymbol("keyVersion"), FunctionTemplate::New(CardKeyVersion)->GetFunction());
            card->Set(String::NewSymbol("freeMemory"), FunctionTemplate::New(CardFreeMemory)->GetFunction());
            card->Set(String::NewSymbol("setKey"), FunctionTemplate::New(CardSetKey)->GetFunction());
            card->Set(String::NewSymbol("setAid"), FunctionTemplate::New(CardSetAid)->GetFunction());
            card->Set(String::NewSymbol("format"), FunctionTemplate::New(CardFormat)->GetFunction());
            card->Set(String::NewSymbol("readNdef"), FunctionTemplate::New(CardReadNdef)->GetFunction());
            card->Set(String::NewSymbol("writeNdef"), FunctionTemplate::New(CardWriteNdef)->GetFunction());

            const unsigned argc = 3;
            Local<Value> argv[argc] = {
              Local<Value>::New(error),
              Local<Value>::New(reader),
              Local<Value>::New(card)
            };
            data->callback->Call(Context::GetCurrent()->Global(), argc, argv);

            delete cardData;
            //mifare_desfire_disconnect(tags[i]);
            error = Object::New();
          }
        }
        freefare_free_tags(tags);
      }
    }
  } else if(static_cast<unsigned int>(res) == SCARD_E_TIMEOUT) {
  }
  if(error->Has(String::NewSymbol("msg"))) {
    const unsigned argc = 3;
    Local<Value> argv[argc] = {
      Local<Value>::New(error),
      Local<Value>::New(Undefined()),
      Local<Value>::New(Undefined())
    };
    data->callback->Call(Context::GetCurrent()->Global(), argc, argv);
  }
}

Handle<Value> ReaderListen(const Arguments& args) {
  HandleScope scope;
  Local<Object> self = args.This();
  reader_data *data = static_cast<reader_data *>(External::Unwrap(self->GetHiddenValue(String::NewSymbol("data"))));
  if(args.Length()!=1 || !args[0]->IsFunction()) {
    ThrowException(Exception::TypeError(String::New("The only argument to listen has to be a callback function")));
    return scope.Close(Undefined());
  }

  data->callback = Persistent<Function>::New(Local<Function>::Cast(args[0]));
  data->self = Persistent<Object>::New(self);

  uv_timer_init(uv_default_loop(), &data->timer);
  uv_timer_start(&data->timer, reader_timer_callback, 500, 250);
  return scope.Close(args.This()); 
}

