#ifndef BASE_JSON_JSON_UTIL_H_
#define BASE_JSON_JSON_UTIL_H_

#include <string>
#include <list>
#include <assert.h>
#include "json.h"

namespace base {

bool StrListToJsonString(const std::list<std::string>& list, std::string& out);

bool JsonStrArrayToList(const Json::Value& array_str, std::list<std::string>& out);

bool JsonArrayStringToList(const std::string& array_str, std::list<std::string>& out);

bool ParseJsonValue(const std::string &content, Json::Value &values);

std::string GetJsonStringWithNoStyled(const Json::Value& values);

Json::Value GetJsonValueFromJsonString(const std::string& json_string);

} // namespace base

#endif //BASE_JSON_JSON_UTIL_H_