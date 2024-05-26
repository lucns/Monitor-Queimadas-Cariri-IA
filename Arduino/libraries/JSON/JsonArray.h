// Developed by @lucns

#ifndef JSONARRAY_H
#define JSONARRAY_H

#include "JsonParser.h"
#include "JsonObject.h"

class JsonObject;
class JsonArray : public JsonParser {

  private:
  public:
    JsonArray();
    JsonArray(String content);

    bool has(String item);
    void put(JsonObject value);
    void put(JsonArray value);
    void put(String value);
    void put(int value);
    void put(unsigned long value);
    void put(double value);
    void put(bool value);
    void put(float value);

    JsonObject getJsonObject(int index);
    JsonArray getJsonArray(int index);
    String getString(int index);
    int getInt(int index);
    unsigned long getLong(int index);
    double getDouble(int index);
    float getFloat(int index);
    bool getBoolean(int index);
};
#endif
