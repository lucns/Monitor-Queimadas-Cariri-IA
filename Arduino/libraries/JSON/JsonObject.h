// Developed by @lucns

#ifndef JSONOBJECT_H
#define JSONOBJECT_H

#include "JsonParser.h"
#include "JsonArray.h"

class JsonArray;
class JsonObject : public JsonParser {

  private:
    String getValue(String key);
  public:
    JsonObject();
    JsonObject(String content);

    void put(String key, JsonObject value);
    void put(String key, JsonArray value);
    void put(String key, String value);
    void put(String key, char* value);
    void put(String key, int value);
    void put(String key, unsigned long value);
    void put(String key, double value);
    void put(String key, bool value);
    void put(String key, float value);

    JsonObject getJsonObject(String key);
    JsonArray getJsonArray(String key);
    String getString(String key);
    int getInt(String key);
    unsigned long getLong(String key);
    double getDouble(String key);
    float getFloat(String key);
    bool getBoolean(String key);
    bool contains(String key);
};
#endif
