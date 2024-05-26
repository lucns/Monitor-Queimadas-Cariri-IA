// Developed by @lucns

#ifndef JSONPARSER_H
#define JSONPARSER_H

#include "Arduino.h"

#define MAXIMUM_ARRAY_SIZE 32

class JsonParser {

  private:  
    String data[MAXIMUM_ARRAY_SIZE];
    uint16_t index;
	bool valid;
  public:
    JsonParser();
    JsonParser(String content);
    String toString();
	bool isValid();
    int length();
	unsigned long getSizeInMemory();
  protected:
    bool isArray;
    void putInternal(String s);
    String getItem(uint16_t index);
};
#endif
