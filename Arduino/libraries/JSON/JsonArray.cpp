// Developed by @lucns

#include "JsonArray.h"

JsonArray::JsonArray() : JsonParser() {
  isArray = true;
}

JsonArray::JsonArray(String content) : JsonParser(content) {
  isArray = true;
}

void JsonArray::put(JsonObject value) {
  putInternal(value.toString());
}

void JsonArray::put(JsonArray value) {
  putInternal(value.toString());
}

void JsonArray::put(String value) {
  putInternal("\"" + value +  "\"");
}

void JsonArray::put(int value) {
  putInternal(String(value));
}

void JsonArray::put(unsigned long value) {
  putInternal(String(value));
}

void JsonArray::put(double value) {
  putInternal(String(value));
}

void JsonArray::put(boolean value) {
  String v = value ? "true" : "false";
  putInternal(String(v));
}

void JsonArray::put(float value) {
  putInternal(String(value));
}

JsonObject JsonArray::getJsonObject(int index) {
  JsonObject a(getItem(index));
  return a;
}

JsonArray JsonArray::getJsonArray(int index) {
  JsonArray a(getItem(index));
  return a;
}

String JsonArray::getString(int index) {
  String s = getItem(index);
  return s.substring(1, s.length() - 1);
}

int JsonArray::getInt(int index) {
  String a = getString(index);
  return a.toInt();
}

unsigned long JsonArray::getLong(int index) {
  String a = getString(index);
  return atol(a.c_str());
}

double JsonArray::getDouble(int index) {
  String a = getString(index);
  return a.toDouble();
}

float JsonArray::getFloat(int index) {
  String a = getString(index);
  return a.toFloat();
}

bool JsonArray::getBoolean(int index) {
  String a = getString(index);
  return a == "true";
}
