// Developed by @lucns

#include "JsonParser.h"

JsonParser::JsonParser() {
  index = 0;
}

JsonParser::JsonParser(String content) {
  valid = (content.startsWith("[") && content.endsWith("]")) || (content.startsWith("{") && content.endsWith("}"));
  if (!valid) return;
  index = 0;
  char c;
  int openedBraces = 0;
  int openedBrackets = 0;
  String s;
  for (uint16_t i = 1; i < content.length() - 1; i++) {
    c = content.charAt(i);
    switch (c) {
      case '{':
        openedBrackets++;
        break;
      case '}':
        openedBrackets--;
        break;
      case '[':
        openedBraces++;
        break;
      case ']':
        openedBraces--;
        break;
      case ',':
        if (!openedBrackets && !openedBraces) {
          putInternal(s);
          s = "";
          continue;
        }
        break;
    }
    s += c;
  }
  putInternal(s);
}

bool JsonParser::isValid() {
	return valid;
}

void JsonParser::putInternal(String s) {
  if (!isArray) {
	String key = s.substring(1, s.substring(1).indexOf("\"") + 1);
	for (uint16_t i = 0; i < index; i++) {
	  if (key.equals(data[i].substring(1, data[i].substring(1).indexOf("\"") + 1))) {
		data[i] = "\"" + key + "\":" + s.substring(s.indexOf(":") + 1);
		return;
	  }
	}
  }  
  
  if (index == MAXIMUM_ARRAY_SIZE) return;
  data[index++] = s;
}

String JsonParser::toString() {
  String s;
  for (uint16_t i = 0; i < index; i++) {
    if (i > 0) s += ",";
    s += data[i];
  }
  if (isArray) return "[" + s + "]";
  return "{" + s + "}";
}

unsigned long JsonParser::getSizeInMemory() {
  unsigned long a = 0;	
  for (uint16_t i = 0; i < index; i++) {
    if (i > 0) a++;
    a += data[i].length();
  }
  return a + 2;
}

int JsonParser::length() {
  return index;
}

String JsonParser::getItem(uint16_t i) {
  if (i >= index) return "";
  return data[i];
}
