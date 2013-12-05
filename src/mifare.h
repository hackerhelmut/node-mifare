// Copyright 2013, Rolf Meyer
// See LICENCE for more information

#ifndef MIFARE_H
#define MIFARE_H

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
#include <freefare.h>

/**
 * Get Names of the Readers connected to the computer
 * @param hContext The SCard Context used to search
 * @return An Array of Strings with reader names
 **/
Handle<Value> getReader(const Arguments& args);

/**
 * Node.js initialization function
 * @param exports The Commonjs module exports object
 **/
void init(Handle<Object> exports);

#endif // MIFARE_H
