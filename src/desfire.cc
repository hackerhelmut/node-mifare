// Copyright 2013, Rolf Meyer
// See LICENCE for more information

#include "desfire.h"
#include "utils.h"

Handle<Value> CardInfo(const Arguments& args) {
  int res;
  HandleScope scope;
  Local<Object> self = args.This();
  Local<Object> card = Object::New();
  card_data *data = static_cast<card_data *>(External::Unwrap(self->GetHiddenValue(String::NewSymbol("data"))));
  if(args.Length()!=0) {
    return scope.Close(errorResult(0x12321, "This function takes no arguments"));
  }
  res = mifare_desfire_connect(data->tag);
  if(res) {
    return scope.Close(errorResult(res, "Can't conntect to Mifare DESFire target."));
  }

  struct mifare_desfire_version_info info;
  res = mifare_desfire_get_version(data->tag, &info);
  if(res) {
    return scope.Close(errorResult(res, freefare_strerror(data->tag)));
  }
  mifare_desfire_disconnect(data->tag);

  Local<Array> uid = Local<Array>::New(Array::New(7));
  for(unsigned int j=0; j<7; j++) {
    uid->Set(j, Integer::New(info.uid[j]));
  }
  card->Set(String::NewSymbol("uid"), uid);
   
  Local<Array> bno = Local<Array>::New(Array::New(5));
  for(unsigned int j=0; j<5; j++) {
    bno->Set(j, Integer::New(info.batch_number[j]));
  }
  card->Set(String::NewSymbol("batchNumber"), bno);

  Local<Object> pdate = Object::New();
  pdate->Set(String::NewSymbol("week"), Number::New(info.production_week));
  pdate->Set(String::NewSymbol("year"), Number::New(info.production_year));
  card->Set(String::NewSymbol("production"), pdate);

  Local<Object> hardware = Object::New();
  hardware->Set(String::NewSymbol("vendorId"), Number::New(info.hardware.vendor_id));
  hardware->Set(String::NewSymbol("type"), Number::New(info.hardware.type));
  hardware->Set(String::NewSymbol("subtype"), Number::New(info.hardware.subtype));

  Local<Object> hw_version = Object::New();
  hw_version->Set(String::NewSymbol("major"), Number::New(info.hardware.version_major));
  hw_version->Set(String::NewSymbol("minor"), Number::New(info.hardware.version_minor));
  hardware->Set(String::NewSymbol("version"), hw_version);

  hardware->Set(String::NewSymbol("storageSize"), Number::New(info.hardware.storage_size));
  hardware->Set(String::NewSymbol("protocol"), Number::New(info.hardware.protocol));
  card->Set(String::NewSymbol("hardware"), hardware);

  Local<Object> software = Object::New();
  software->Set(String::NewSymbol("vendorId"), Number::New(info.software.vendor_id));
  software->Set(String::NewSymbol("type"), Number::New(info.software.type));
  software->Set(String::NewSymbol("subtype"), Number::New(info.software.subtype));

  Local<Object> sw_version = Object::New();
  sw_version->Set(String::NewSymbol("major"), Number::New(info.software.version_major));
  sw_version->Set(String::NewSymbol("minor"), Number::New(info.software.version_minor));
  software->Set(String::NewSymbol("version"), sw_version);

  software->Set(String::NewSymbol("storageSize"), Number::New(info.software.storage_size));
  software->Set(String::NewSymbol("protocol"), Number::New(info.software.protocol));
  card->Set(String::NewSymbol("software"), software);
  
  return scope.Close(card);
}

Handle<Value> CardMasterKeyInfo(const Arguments& args) {
  LONG res;
  uint8_t settings;
  uint8_t max_keys;
  HandleScope scope;
  Local<Object> self = args.This();
  card_data *data = static_cast<card_data *>(External::Unwrap(self->GetHiddenValue(String::NewSymbol("data"))));
  if(args.Length()!=0) {
    return scope.Close(errorResult(0x12321, "This function takes no arguments"));
  }
  res = mifare_desfire_connect(data->tag);
  if(res < 0) {
    return scope.Close(errorResult(res, "Can't conntect to Mifare DESFire target."));
  }

  res = mifare_desfire_get_key_settings(data->tag, &settings, &max_keys);
  if(!res) {
    Local<Object> key = Object::New();
    key->Set(String::NewSymbol("configChangable"), Boolean::New((settings & 0x08)));
    key->Set(String::NewSymbol("freeCreateDelete"), Boolean::New((settings & 0x04)));
    key->Set(String::NewSymbol("freeDirectoryList"), Boolean::New((settings & 0x02)));
    key->Set(String::NewSymbol("keyChangable"), Boolean::New((settings & 0x01)));
    key->Set(String::NewSymbol("maxKeys"), Integer::New((max_keys)));
  
    mifare_desfire_disconnect(data->tag);
    return scope.Close(key); 
  } else if (AUTHENTICATION_ERROR == mifare_desfire_last_picc_error(data->tag)) {
    Local<String> key = String::New("LOCKED");
  
    mifare_desfire_disconnect(data->tag);
    return scope.Close(key); 
  } else {
  
    mifare_desfire_disconnect(data->tag);
    return scope.Close(errorResult(0x12322, freefare_strerror(data->tag)));
  }
}

Handle<Value> CardName(const Arguments& args) {
  HandleScope scope;
  Local<Object> self = args.This();
  card_data *data = static_cast<card_data *>(External::Unwrap(self->GetHiddenValue(String::NewSymbol("data"))));
  int res;
  if(args.Length()!=0) {
    return scope.Close(errorResult(0x12321, "This function takes no arguments"));
  }

  res = mifare_desfire_connect(data->tag);
  if(res) {
    return scope.Close(errorResult(res, "Can't conntect to Mifare DESFire target."));
  }

  Local<String> name = String::New(freefare_get_tag_friendly_name(data->tag));
  mifare_desfire_disconnect(data->tag);
  return scope.Close(name);
}

Handle<Value> CardKeyVersion(const Arguments& args) {
  int res;
  uint8_t version;
  HandleScope scope;
  Local<Object> self = args.This();
  card_data *data = static_cast<card_data *>(External::Unwrap(self->GetHiddenValue(String::NewSymbol("data"))));
  if(args.Length()!=1 || !args[0]->IsNumber()) {
    return scope.Close(errorResult(0x12321, "This function takes a key number as arguments"));
  }
  res = mifare_desfire_connect(data->tag);
  if(res) {
    return scope.Close(errorResult(res, "Can't conntect to Mifare DESFire target."));
  }

  res = mifare_desfire_get_key_version(data->tag, args[0]->ToUint32()->Value(), &version);
  if(res) {
    return scope.Close(errorResult(res, freefare_strerror(data->tag)));
  }
  return scope.Close(Local<Number>::New(Number::New(version)));
}

Handle<Value> CardFreeMemory(const Arguments& args) {
  LONG res;
  uint32_t size;
  HandleScope scope;
  Local<Object> self = args.This();
  card_data *data = static_cast<card_data *>(External::Unwrap(self->GetHiddenValue(String::NewSymbol("data"))));
  if(args.Length()!=0) {
    return scope.Close(errorResult(0x12321, "This function takes no arguments"));
  }

  res = mifare_desfire_connect(data->tag);
  if(res) {
    return scope.Close(errorResult(res, "Can't conntect to Mifare DESFire target."));
  }

  res = mifare_desfire_free_mem(data->tag, &size);
  mifare_desfire_disconnect(data->tag);
  if(res) {
    return scope.Close(errorResult(res, freefare_strerror(data->tag)));
  }
  return scope.Close(Local<Number>::New(Number::New(size)));
}

Handle<Value> CardSetAid(const Arguments& args) {
  HandleScope scope;
  Local<Object> self = args.This();
  card_data *data = static_cast<card_data *>(External::Unwrap(self->GetHiddenValue(String::NewSymbol("data"))));
  if(args.Length()!=1 || !args[0]->IsNumber() || args[0]->ToUint32()->Value() > 0xFFFFFF) {
    return scope.Close(errorResult(0x12321, "This function takes the aid as argument a number smaller than 0x1000000"));
  }
  if(data->aid) {
    free(data->aid);
  }
  data->aid = mifare_desfire_aid_new(args[0]->ToUint32()->Value());
  return scope.Close(validTrue());
}

Handle<Value> CardSetKey(const Arguments & args) {
  typedef MifareDESFireKey (*callback_t)(uint8_t *);
  callback_t callbacks[4][2] = {
    {mifare_desfire_des_key_new, mifare_desfire_des_key_new_with_version},
    {mifare_desfire_3des_key_new, mifare_desfire_3des_key_new_with_version},
    {mifare_desfire_3k3des_key_new, mifare_desfire_3k3des_key_new_with_version},
    {mifare_desfire_aes_key_new, NULL}
  };
  HandleScope scope;
  Local<Object> self = args.This();
  card_data *data = static_cast<card_data *>(External::Unwrap(self->GetHiddenValue(String::NewSymbol("data"))));
  uint8_t key_val[24];
  bool version = false;
  uint8_t aes_ver = 0;
  int type = 0;
  uint32_t max_len = 8;
  std::string error = (
    "This function takes up to four arguments. "
    "The first is the key an must be an array of 8, 16 or 24 bytes, depending on the type. "
    "The second is the type a string out of (des|3des|3k3des|aes) which defines also the length of the key. "
    "If you choose \"des\" the key is 8 numbers long. If you choose 3des or aes it is 16 numbers long. "
    "For 3k3des it has to be 24 numbers long. it will default to \"des\"."
    "The third is version a boolean describing wether the key is versioned. It will default to false"
    "If you choose to use an \"aes\" key the fourth argument is the aes version a number smaller 255"
  );
  if(args.Length()==0 || args.Length()>4 || !args[0]->IsArray() ||
      (args.Length()>1 && !args[1]->IsString()) ||
      (args.Length()>2 && !args[2]->IsBoolean()) ||
      (args.Length()>3 && !args[3]->IsUint32()) || 
      (args.Length()>3 && args[3]->ToUint32()->Value()>255) 
    ) {
    return scope.Close(errorResult(0x12321, error));
  }

  if(args.Length()>2) {
    version = args[2]->BooleanValue();
  }

  if(args.Length()>1) {
    Local<String> t = Local<String>::Cast(args[1]);
    if(t->Equals(String::New("aes"))) {
      type = 3;
      max_len = 16;
    } else if(t->Equals(String::New("3k3des"))) {
      type = 2;
      max_len = 24;
    } else if(t->Equals(String::New("3des"))) {
      type = 1;
      max_len = 16;
    }
  }

  if(type == 3 && args.Length() > 3) {
    aes_ver = (uint8_t)(args[3]->ToUint32()->Value()&0xFF);
  }

  if(args[0]->IsArray()) {
    Local<Array> key = Local<Array>::Cast(args[0]);
    if(key->Length()!=max_len) {
      return scope.Close(errorResult(0x12321, error));
    }
    for(uint32_t i=0; i<max_len; i++) {
      Local<Value> k = key->Get(i);
      if(!k->IsInt32()) {
        return scope.Close(errorResult(0x12321, error));
      }
      if(((int32_t)k->ToInt32()->Value())>255) {
        return scope.Close(errorResult(0x12321, error));
      }
      key_val[i] = (uint8_t)(((uint32_t)k->ToUint32()->Value()) & 0xFF);
    }
    if(data->key) {
      mifare_desfire_key_free(data->key);
    }
    callback_t cb = callbacks[type][version];
    if(cb) {
      data->key = cb(key_val);
    } else {
      mifare_desfire_aes_key_new_with_version(key_val, 0);
    }
  }
  for(uint32_t i = 0; i < max_len; i++) {
    key_val[i] = 0;
  }

  return scope.Close(args.This()); 
}

Handle<Value> CardFormat(const Arguments& args) {
  HandleScope scope;
  Local<Object> self = args.This();
  card_data *data = static_cast<card_data *>(External::Unwrap(self->GetHiddenValue(String::NewSymbol("data"))));
  uint8_t key_data_picc[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  if(args.Length()>1 || (args.Length()==1 && !args[0]->IsObject())) {
    return scope.Close(errorResult(0x12321, "The only argument to listen has to be a callback function"));
  }
  // Send Mifare DESFire ChangeKeySetting to change the PICC master key settings into :
  // bit7-bit4 equal to 0000b
  // bit3 equal to 1b, the configuration of the PICC master key MAY be changeable or frozen
  // bit2 equal to 1b, CreateApplication and DeleteApplication commands are allowed without PICC master key authentication
  // bit1 equal to 1b, GetApplicationIDs, and GetKeySettings are allowed without PICC master key authentication
  // bit0 equal to 1b, PICC masterkey MAY be frozen or changeable
  Local<Object> options = args.Length()==1? Local<Object>::Cast(args[0]) : Object::New();
  bool configChangable = options->Has(String::NewSymbol("configChangeable")) ?
    options->Get(String::NewSymbol("configChangable"))->IsTrue() :
    true;
  bool freeCreateDelete = options->Has(String::NewSymbol("freeCreateDelete")) ?
    options->Get(String::NewSymbol("freeCreateDelete"))->IsTrue() :
    true;
  bool freeDirectoryList = options->Has(String::NewSymbol("freeDirectoryList")) ?
    options->Get(String::NewSymbol("freeDirectoryList"))->IsTrue() :
    true;
  bool keyChangable = options->Has(String::NewSymbol("keyChangable")) ?
    options->Get(String::NewSymbol("keyChangable"))->IsTrue() :
    true;
  uint8_t flags = (configChangable << 3) | (freeCreateDelete << 2) | (freeDirectoryList << 1) | (keyChangable << 0);
  int res;

  res = mifare_desfire_connect(data->tag);
  if(res < 0) {
    return scope.Close(errorResult(res, "Can't connect to Mifare DESFire target."));
  }

  MifareDESFireKey key_picc = mifare_desfire_des_key_new_with_version(key_data_picc);
  res = mifare_desfire_authenticate(data->tag, 0, key_picc);
  if(res < 0) {
    mifare_desfire_disconnect(data->tag);
    return scope.Close(errorResult(res, "Can't authenticate on Mifare DESFire target."));
  }
  mifare_desfire_key_free(key_picc);

  res = mifare_desfire_change_key_settings(data->tag, flags);
  if(res < 0) {
    mifare_desfire_disconnect(data->tag);
    return scope.Close(errorResult(res, "ChangeKeySettings failed"));
  }
  res = mifare_desfire_format_picc(data->tag);
  if(res < 0) {
    mifare_desfire_disconnect(data->tag);
    return scope.Close(errorResult(res, "Can't format PICC."));
  }

  mifare_desfire_disconnect(data->tag);

  return scope.Close(validTrue()); 
}

Handle<Value> CardCreateNdef(const Arguments& args) {
  HandleScope scope;
  Local<Object> self = args.This();
  int res =0;
  uint8_t file_no = 0;
  //uint16_t ndef_max_len = 0;
  char *ndef_msg = NULL;
  uint16_t ndef_msg_len = 0;

  uint8_t ndef_read_key[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  uint8_t *key_data_picc = ndef_read_key;
  uint8_t *key_data_app = ndef_read_key;

  card_data *data = static_cast<card_data *>(External::Unwrap(self->GetHiddenValue(String::NewSymbol("data"))));
  if(args.Length()!=1 || !Buffer::HasInstance(args[0])) {
    return scope.Close(errorResult(0x12321, "This function takes a buffer to write to a tag"));
  }

  res = mifare_desfire_connect(data->tag);
  if(res) {
    return scope.Close(errorResult(res, "Can't conntect to Mifare DESFire target."));
  }

  struct mifare_desfire_version_info info;
  res = mifare_desfire_get_version(data->tag, &info);
  if(res < 0) {
    mifare_desfire_disconnect(data->tag);
    return errorResult(res, freefare_strerror(data->tag));
  }

  int ndef_mapping;
  switch(info.software.version_major) {
  case 0: {
      ndef_mapping = 1;
    } break;
  case 1: 
  default: // newer version? let's assume it supports latest mapping too
      ndef_mapping = 2;
  }

  /* Initialised Formatting Procedure. See section 6.5.1 and 8.1 of Mifare DESFire as Type 4 Tag document*/
  // Send Mifare DESFire Select Application with AID equal to 000000h to select the PICC level
  res = mifare_desfire_select_application(data->tag, NULL);
  if(res < 0) {
    mifare_desfire_disconnect(data->tag);
    return scope.Close(errorResult(res, "Application selection failed"));
  }

  MifareDESFireKey key_picc;
  MifareDESFireKey key_app;
  key_picc = mifare_desfire_des_key_new_with_version(key_data_picc);
  key_app = mifare_desfire_des_key_new_with_version(key_data_app);

  // Authentication with PICC master key MAY be needed to issue ChangeKeySettings command
  res = mifare_desfire_authenticate(data->tag, 0, key_picc);
  if(res < 0) {
  mifare_desfire_disconnect(data->tag);
    return scope.Close(errorResult(res, "Authentication with PICC master key failed"));
  }

  MifareDESFireAID aid;
  if(ndef_mapping == 1) {
    uint8_t key_settings;
    uint8_t max_keys;
    mifare_desfire_get_key_settings(data->tag, &key_settings, &max_keys);
    if((key_settings & 0x08) == 0x08) {

      // Send Mifare DESFire ChangeKeySetting to change the PICC master key settings into :
      // bit7-bit4 equal to 0000b
      // bit3 equal to Xb, the configuration of the PICC master key MAY be changeable or frozen
      // bit2 equal to 0b, CreateApplication and DeleteApplication commands are allowed with PICC master key authentication
      // bit1 equal to 0b, GetApplicationIDs, and GetKeySettings are allowed with PICC master key authentication
      // bit0 equal to Xb, PICC masterkey MAY be frozen or changeable
      res = mifare_desfire_change_key_settings(data->tag, 0x09);
      if(res < 0) {
        mifare_desfire_disconnect(data->tag);
        return scope.Close(errorResult(res, "ChangeKeySettings failed"));
      }
    }

    // Mifare DESFire Create Application with AID equal to EEEE10h, key settings equal to 0x09, NumOfKeys equal to 01h
    aid = mifare_desfire_aid_new(0xEEEE10);
    res = mifare_desfire_create_application(data->tag, aid, 0x09, 1);
    if(res < 0) {
      mifare_desfire_disconnect(data->tag);
      return scope.Close(errorResult(res, "Application creation failed. Try format before running create."));
    }

    // Mifare DESFire SelectApplication (Select previously creates application)
    res = mifare_desfire_select_application(data->tag, aid);
    if(res < 0) {
      mifare_desfire_disconnect(data->tag);
      return scope.Close(errorResult(res, "Application selection failed"));
    }
    free(aid);

    // Authentication with NDEF Tag Application master key (Authentication with key 0)
    res = mifare_desfire_authenticate(data->tag, 0, key_app);
    if(res < 0) {
      mifare_desfire_disconnect(data->tag);
      return scope.Close(errorResult(res, "Authentication with NDEF Tag Application master key failed"));
    }

    // Mifare DESFire ChangeKeySetting with key settings equal to 00001001b
    res = mifare_desfire_change_key_settings(data->tag, 0x09);
    if(res < 0) {
      mifare_desfire_disconnect(data->tag);
      return scope.Close(errorResult(res, "ChangeKeySettings failed"));
    }

    // Mifare DESFire CreateStdDataFile with FileNo equal to 03h (CC File DESFire FID), ComSet equal to 00h,
    // AccesRights equal to E000h, File Size bigger equal to 00000Fh
    res = mifare_desfire_create_std_data_file(data->tag, 0x03, MDCM_PLAIN, 0xE000, 0x00000F);
    if(res < 0) {
      mifare_desfire_disconnect(data->tag);
      return scope.Close(errorResult(res, "CreateStdDataFile failed"));
    }

    // Mifare DESFire WriteData to write the content of the CC File with CClEN equal to 000Fh,
    // Mapping Version equal to 10h,MLe equal to 003Bh, MLc equal to 0034h, and NDEF File Control TLV
    // equal to T =04h, L=06h, V=E1 04 (NDEF ISO FID=E104h) 0E E0 (NDEF File size =3808 Bytes) 00 (free read access)
    // 00 free write access
    uint8_t capability_container_file_content[15] = {
      0x00, 0x0F,     // CCLEN: Size of this capability container.CCLEN values are between 000Fh and FFFEh
      0x10,           // Mapping version
      0x00, 0x3B,     // MLe: Maximum data size that can be read using a single ReadBinary command. MLe = 000Fh-FFFFh
      0x00, 0x34,     // MLc: Maximum data size that can be sent using a single UpdateBinary command. MLc = 0001h-FFFFh
      0x04, 0x06,     // T & L of NDEF File Control TLV, followed by 6 bytes of V:
      0xE1, 0x04,     //   File Identifier of NDEF File
      0x0E, 0xE0,     //   Maximum NDEF File size of 3808 bytes
      0x00,           //   free read access
      0x00            //   free write acces
    };

    res = mifare_desfire_write_data(data->tag, 0x03, 0, sizeof(capability_container_file_content), capability_container_file_content);
    if(res < 0) {
      mifare_desfire_disconnect(data->tag);
      return scope.Close(errorResult(res, "Write CC file content failed"));
    }

    // Mifare DESFire CreateStdDataFile with FileNo equal to 04h (NDEF FileDESFire FID), CmmSet equal to 00h, AccessRigths
    // equal to EEE0h, FileSize equal to 000EE0h (3808 Bytes)
    res = mifare_desfire_create_std_data_file(data->tag, 0x04, MDCM_PLAIN, 0xEEE0, 0x000EE0);
    if(res < 0) {
      mifare_desfire_disconnect(data->tag);
      return scope.Close(errorResult(res, "CreateStdDataFile failed"));
    }

  } else if(ndef_mapping == 2) {
    // Mifare DESFire Create Application with AID equal to 000001h, key settings equal to 0x0F, NumOfKeys equal to 01h,
    // 2 bytes File Identifiers supported, File-ID equal to E110h
    aid = mifare_desfire_aid_new(0x000001);
    uint8_t app[] = { 0xd2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x01 };
    res = mifare_desfire_create_application_iso(data->tag, aid, 0x0F, 0x21, 0, 0xE110, app, sizeof(app));
    if(res < 0) {
      mifare_desfire_disconnect(data->tag);
      return scope.Close(errorResult(res, "Application creation failed. Try format before running."));
    }

    // Mifare DESFire SelectApplication (Select previously creates application)
    res = mifare_desfire_select_application(data->tag, aid);
    if(res < 0) {
      mifare_desfire_disconnect(data->tag);
      return scope.Close(errorResult(res, "Application selection failed"));
    }
    free(aid);

    // Authentication with NDEF Tag Application master key (Authentication with key 0)
    res = mifare_desfire_authenticate(data->tag, 0, key_app);
    if(res < 0) {
      mifare_desfire_disconnect(data->tag);
      return scope.Close(errorResult(res, "Authentication with NDEF Tag Application master key failed"));
    }

    // Mifare DESFire CreateStdDataFile with FileNo equal to 01h (DESFire FID), ComSet equal to 00h,
    // AccesRights equal to E000h, File Size bigger equal to 00000Fh, ISO File ID equal to E103h
    res = mifare_desfire_create_std_data_file_iso(data->tag, 0x01, MDCM_PLAIN, 0xE000, 0x00000F, 0xE103);
    if(res < 0) {
      mifare_desfire_disconnect(data->tag);
      return scope.Close(errorResult(res, "CreateStdDataFileIso failed"));
    }

    // Mifare DESFire WriteData to write the content of the CC File with CClEN equal to 000Fh,
    // Mapping Version equal to 20h,MLe equal to 003Bh, MLc equal to 0034h, and NDEF File Control TLV
    // equal to T =04h, L=06h, V=E1 04 (NDEF ISO FID=E104h) 0xNNNN (NDEF File size = 0x0800/0x1000/0x1E00 bytes)
    // 00 (free read access) 00 free write access
    uint8_t capability_container_file_content[15] = {
      0x00, 0x0F,     // CCLEN: Size of this capability container.CCLEN values are between 000Fh and FFFEh
      0x20,           // Mapping version
      0x00, 0x3B,     // MLe: Maximum data size that can be read using a single ReadBinary command. MLe = 000Fh-FFFFh
      0x00, 0x34,     // MLc: Maximum data size that can be sent using a single UpdateBinary command. MLc = 0001h-FFFFh
      0x04, 0x06,     // T & L of NDEF File Control TLV, followed by 6 bytes of V:
      0xE1, 0x04,     //   File Identifier of NDEF File
      0x04, 0x00,     //   Maximum NDEF File size of 1024 bytes
      0x00,           //   free read access
      0x00            //   free write acces
    };
    
    uint16_t ndef_max_size = 0x0800;
    uint16_t announcedsize = 1 << (info.software.storage_size >> 1);
    if(announcedsize >= 0x1000) {
      ndef_max_size = 0x1000;
    }
    if(announcedsize >= 0x1E00) {
      ndef_max_size = 0x1E00;
    }
    capability_container_file_content[11] = ndef_max_size >> 8;
    capability_container_file_content[12] = ndef_max_size & 0xFF;
    res = mifare_desfire_write_data(data->tag, 0x01, 0, sizeof(capability_container_file_content), capability_container_file_content);
    if(res < 0) {
      mifare_desfire_disconnect(data->tag);
      return scope.Close(errorResult(res, "Write CC file content failed"));
    }

    // Mifare DESFire CreateStdDataFile with FileNo equal to 02h (DESFire FID), CmmSet equal to 00h, AccessRigths
    // equal to EEE0h, FileSize equal to ndefmaxsize (0x000800, 0x001000 or 0x001E00)
    res = mifare_desfire_create_std_data_file_iso(data->tag, 0x02, MDCM_PLAIN, 0xEEE0, ndef_max_size, 0xE104);
    if(res < 0) {
      mifare_desfire_disconnect(data->tag);
      return scope.Close(errorResult(res, "CreateStdDataFileIso failed"));
    }
  }
  mifare_desfire_key_free(key_picc);
  mifare_desfire_key_free(key_app);



  res = mifare_desfire_write_data(data->tag, file_no, 2, ndef_msg_len, (uint8_t*)ndef_msg);
  if(res < 0) {
    mifare_desfire_disconnect(data->tag);
    return scope.Close(errorResult(res, "Writing ndef message faild"));
  }
  mifare_desfire_disconnect(data->tag);

  return scope.Close(validTrue()); 
}



Local<Object> CardReadNdefTVL(card_data *data, int &res, uint8_t &file_no, uint16_t &ndef_max_len, MifareDESFireKey key_app) {
  int version;
  uint8_t *cc_data;
  // #### Get Version
  // We've to track DESFire version as NDEF mapping is different
  struct mifare_desfire_version_info info;
  res = mifare_desfire_get_version(data->tag, &info);
  if(res < 0) {
    return errorResult(res, freefare_strerror(data->tag));
  }
  version = info.software.version_major;

  // ### Select app
  // Mifare DESFire SelectApplication (Select application)
  MifareDESFireAID aid;
  if(version == 0) {
      aid = mifare_desfire_aid_new(0xEEEE10);
  } else {
      // There is no more relationship between DESFire AID and ISO AID...
      // Let's assume it's in AID 000001h as proposed in the spec
      aid = mifare_desfire_aid_new(0x000001);
  }
  res = mifare_desfire_select_application(data->tag, aid);
  if(res < 0) {
    return errorResult(res, "Application selection failed. No NDEF application found.");
  }
  free(aid);

  // ### Authentication
  // NDEF Tag Application master key (Authentication with key 0)
  res = mifare_desfire_authenticate(data->tag, 0, key_app);
  if(res < 0) {
    return errorResult(res, "Authentication with NDEF Tag Application master key failed");
  }

  // ### Read index
  // Read Capability Container file E103
  uint8_t lendata[20]; // cf FIXME in mifare_desfire.c read_data()
  if(version == 0) {
      res = mifare_desfire_read_data(data->tag, 0x03, 0, 2, lendata);
  } else {
      // There is no more relationship between DESFire FID and ISO FileID...
      // Let's assume it's in FID 01h as proposed in the spec
      res = mifare_desfire_read_data(data->tag, 0x01, 0, 2, lendata);
  }

  if(res < 0) {
    return errorResult(res, "Reading the ndef capability container file (E103) failed");
  }
  uint16_t cclen = (((uint16_t)lendata[0]) << 8) + ((uint16_t)lendata[1]);
  if(cclen < 15) {
    res = -12315;
    return errorResult(0x12315, "The read ndef capability container file (E103) is to short");
  }
  if(!(cc_data = new uint8_t[cclen + 20])) { // cf FIXME in mifare_desfire.c read_data()
    res = -12342;
    return errorResult(0x12342, "Allocation of ndef capability container file (E103) failed");
  }
  if(version == 0) {
      res = mifare_desfire_read_data(data->tag, 0x03, 0, cclen, cc_data);
  } else {
      res = mifare_desfire_read_data(data->tag, 0x01, 0, cclen, cc_data);
  }
  if(res < 0) {
    return errorResult(res, "Reading the ndef capability container file data (E103) failed");
  }

  // Search NDEF File Control TLV
  uint8_t off = 7;
  while(((off + 7) < cclen) && (cc_data[off] != 0x04)) {
      off += cc_data[off + 1] + 2; // Skip TLV entry
  }
  
  if(off + 7 >= cclen) {
    res = -12343;
    return errorResult(0x12343, "We've reached the end of the ndef capability container file (E103) and did not find the ndef TLV");
  }
  if(cc_data[off + 2] != 0xE1) {
    res = -12344;
    return errorResult(0x12344, "Found unknown ndef file reference");
  }

  // ### Get file
  if(version == 0) {
      file_no = cc_data[off + 3];
  } else{
      // There is no more relationship between DESFire FID and ISO FileID...
      // Let's assume it's in FID 02h as proposed in the spec
      file_no = 2;
  }
  // Swap endianess
  ndef_max_len = (((uint16_t)cc_data[off + 4]) << 8) + ((uint16_t)cc_data[off + 5]);

  delete [] cc_data;
  res = 0;
  return Local<Object>::New(Object::New());
}

Handle<Value> CardReadNdef(const Arguments& args) {
  HandleScope scope;
  Local<Object> self = args.This();
  int res;
  uint8_t file_no;
  uint16_t ndef_max_len;
  uint8_t *ndef_msg;
  uint16_t ndef_msg_len;

  uint8_t ndef_read_key[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  card_data *data = static_cast<card_data *>(External::Unwrap(self->GetHiddenValue(String::NewSymbol("data"))));
  if(args.Length()!=0) {
    return scope.Close(errorResult(0x12321, "This function does not take any arguments"));
  }

  res = mifare_desfire_connect(data->tag);
  if(res) {
    return scope.Close(errorResult(res, "Can't conntect to Mifare DESFire target."));
  }

  MifareDESFireKey key_app;
  key_app = mifare_desfire_des_key_new_with_version(ndef_read_key);
  Local<Object> errorresult = CardReadNdefTVL(data, res, file_no, ndef_max_len, key_app);
  mifare_desfire_key_free(key_app);
  if(res < 0) {
    mifare_desfire_disconnect(data->tag);
    return scope.Close(errorresult);
  }

  if(!(ndef_msg = new uint8_t[ndef_max_len + 20])) { // cf FIXME in mifare_desfire.c read_data()
    mifare_desfire_disconnect(data->tag);
    return scope.Close(errorResult(0x12345, "Allocation of ndef file failed"));
  }

  uint8_t lendata[20]; // cf FIXME in mifare_desfire.c read_data()
  res = mifare_desfire_read_data(data->tag, file_no, 0, 2, lendata);
  if(res < 0) {
    mifare_desfire_disconnect(data->tag);
    return scope.Close(errorResult(res, "Reading of ndef file failed"));
  }
  ndef_msg_len = (((uint16_t)lendata[0]) << 8) + ((uint16_t)lendata[1]);
  if(ndef_msg_len + 2 > ndef_max_len) {
    mifare_desfire_disconnect(data->tag);
      return scope.Close(errorResult(0x12346, "Declared ndef size larger than max ndef size"));
  }
  res = mifare_desfire_read_data(data->tag, file_no, 2, ndef_msg_len, ndef_msg);
  if(res < 0) {
    mifare_desfire_disconnect(data->tag);
    return scope.Close(errorResult(res, "Reading ndef message faild"));
  }
  Buffer *slowBuffer = Buffer::New(ndef_msg_len);
  memcpy(Buffer::Data(slowBuffer), ndef_msg, ndef_msg_len);

  Local<Object> result = buffer(ndef_msg, ndef_msg_len);
  result->Set(String::NewSymbol("maxLength"), Integer::New(ndef_max_len));
  if(ndef_msg) {
    delete [] ndef_msg;
  }
  mifare_desfire_disconnect(data->tag);
  return scope.Close(validResult(result));
}

Handle<Value> CardWriteNdef(const Arguments& args) {
  HandleScope scope;
  Local<Object> self = args.This();
  Local<Object> result = Object::New();
  int res;
  uint8_t file_no;
  uint16_t ndef_max_len;
  char *ndef_msg;
  uint16_t ndef_msg_len;

  uint8_t ndef_read_key[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  card_data *data = static_cast<card_data *>(External::Unwrap(self->GetHiddenValue(String::NewSymbol("data"))));
  if(args.Length()!=1 || !Buffer::HasInstance(args[0])) {
    return scope.Close(errorResult(0x12321, "This function takes a buffer to write to a tag"));
  }

  ndef_msg_len = Buffer::Length(args[0]);
  ndef_msg = Buffer::Data(args[0]);

  res = mifare_desfire_connect(data->tag);
  if(res) {
    return scope.Close(errorResult(res, "Can't conntect to Mifare DESFire target."));
  }

  MifareDESFireKey key_app;
  key_app = mifare_desfire_des_key_new_with_version(ndef_read_key);

  Local<Object> errorresult = CardReadNdefTVL(data, res, file_no, ndef_max_len, key_app);
  if(res < 0) {
    mifare_desfire_disconnect(data->tag);
    return scope.Close(errorresult);
  }

  result->Set(String::NewSymbol("maxLength"), Integer::New(ndef_max_len));

  if(ndef_msg_len > ndef_max_len) {
    mifare_desfire_disconnect(data->tag);
    return scope.Close(errorResult(0x12346, "Supplied NDEF larger than max NDEF size"));
  }

  uint8_t ndef_msg_b[2];
  ndef_msg_b[0] = (uint8_t)((ndef_msg_len) >> 8);
  ndef_msg_b[1] = (uint8_t)(ndef_msg_len);
  uint32_t zero = 0;
  //Mifare DESFire WriteData to write the content of the NDEF File with NLEN equal to NDEF Message length and NDEF Message
  res = mifare_desfire_write_data(data->tag, file_no, 0, 2, (uint8_t*)&zero);
  if(res < 0) {
    mifare_desfire_disconnect(data->tag);
    return scope.Close(errorResult(res, "Writing ndef message size pre faild"));
  }

  res = mifare_desfire_write_data(data->tag, file_no, 2, ndef_msg_len, (uint8_t*)ndef_msg);
  if(res != ndef_msg_len){
    printf("790, data write res %d %d\n", res, ndef_msg_len);
    mifare_desfire_disconnect(data->tag);
    return scope.Close(errorResult(res, "Writing full ndef message failed"));
  }
  if(res < 0) {
    mifare_desfire_disconnect(data->tag);
    return scope.Close(errorResult(res, "Writing ndef message failed"));
  }
  
  res = mifare_desfire_write_data(data->tag, file_no, 0, 2, ndef_msg_b);
  if(res < 0) {
    mifare_desfire_disconnect(data->tag);
    return scope.Close(errorResult(res, "Writing ndef message size post faild"));
  }


  res = mifare_desfire_disconnect(data->tag);
  return scope.Close(validTrue()); 
}

