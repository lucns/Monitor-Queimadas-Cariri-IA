// Developed by @lucns

#include "JsonObject.h"

JsonObject::JsonObject() : JsonParser() {
  isArray = false;
}

JsonObject::JsonObject(String content) : JsonParser(content) {
  isArray = false;
}

void JsonObject::put(String key, JsonObject value) {
  putInternal("\"" + key + "\"" + ":" + value.toString());
}

void JsonObject::put(String key, JsonArray value) {
  putInternal("\"" + key + "\"" + ":" +  value.toString());
}

void JsonObject::put(String key, String value) {
  putInternal("\"" + key + "\"" + ":\"" + value + "\"");
}

void JsonObject::put(String key, char* value) {
  String s = String(value);
  putInternal("\"" + key + "\"" + ":\"" + s + "\"");
}

void JsonObject::put(String key, int value) {
  putInternal("\"" + key + "\"" + ":" + String(value));
}

void JsonObject::put(String key, unsigned long value) {
  putInternal("\"" + key + "\"" + ":" + String(value));
}

void JsonObject::put(String key, double value) {
  putInternal("\"" + key + "\"" + ":" + String(value));
}

void JsonObject::put(String key, float value) {
  putInternal("\"" + key + "\"" + ":" + String(value));
}

void JsonObject::put(String key, bool value) {
  putInternal("\"" + key + "\"" + ":" + (value ? "true" : "false"));
}

JsonObject JsonObject::getJsonObject(String key) {
  JsonObject a(getValue(key));
  return a;
}

JsonArray JsonObject::getJsonArray(String key) {
  JsonArray a(getValue(key));
  return a;
}

String JsonObject::getString(String key) {
  String s = getValue(key);
  return s.substring(1, s.length() - 1);
}

int JsonObject::getInt(String key) {
  String s = getValue(key);
  return s.toInt();
}

unsigned long JsonObject::getLong(String key) {
  String s = getValue(key);
  return atol(s.c_str());
}

double JsonObject::getDouble(String key) {
  String s = getValue(key);
  return s.toDouble();
}

float JsonObject::getFloat(String key) {
  String s = getValue(key);
  return s.toFloat();
}

bool JsonObject::getBoolean(String key) {
  String a = getValue(key);
  return a == "true";
}

bool JsonObject::contains(String key) {
  String item;
  uint8_t indexOf;
  uint16_t index = 0;
  while (index < length()) {
    item = getItem(index);
    indexOf = item.indexOf(":");
    if (key == item.substring(1, indexOf - 1)) {
      return true;
    }
    index++;
  }
  return false;
}

String JsonObject::getValue(String key) {
  String item;
  uint8_t indexOf;
  uint16_t index = 0;
  while (index < length()) {
    item = getItem(index);
    indexOf = item.indexOf(":");
    if (key == item.substring(1, indexOf - 1)) {
      return item.substring(indexOf + 1);
    }
    index++;
  }
  return "";
}
