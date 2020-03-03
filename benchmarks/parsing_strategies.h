#pragma once

#include <simdjson/jsonparser.h>
#include <simdjson/jsonstream.h>
#include <rapidjson/document.h>
#include <sajson.h>

#include "utility.h"

using namespace simdjson;

//SIMDJson

bool simdjsonMBWRParse(const char* filename);

bool simdjsonMBRParse(const char *filename);

bool simdjsonPStrParse(const char *filename);

//Rapidjson

bool rapidjsonParseInsitu(const char *filename);

bool rapidjsonParse(const char *filename);

//Sajson

bool sajsonParseDynAlloc(const char *filename);

bool sajsonParseSngAlloc(const char *filename);

bool sajsonParseDynAllocMutBuff(const char *filename);

bool sajsonParseSngAllocMutBuff(const char *filename);