#ifndef BASE_UTIL_STRING_UTIL_H_
#define BASE_UTIL_STRING_UTIL_H_

#include <list>
#include <string>

typedef char UTF8Char;

typedef wchar_t UTF16Char;
typedef int32_t UTF32Char;

typedef std::basic_string<UTF8Char> UTF8String;
typedef UTF8String U8String;
typedef std::basic_string<UTF16Char> UTF16String;
typedef UTF16String U16String;
typedef std::basic_string<UTF32Char> UTF32String;
typedef UTF32String U32String;

namespace base
{

// format a string
std::string StringPrintf(const char *format, ...);
std::wstring StringPrintf(const wchar_t *format, ...);
const std::string& StringPrintf(std::string &output, const char *format, ...);
const std::wstring& StringPrintf(std::wstring &output, const wchar_t *format, ...);
void StringPrintfV(std::string &output, const char *format, va_list ap);
void StringPrintfV(std::wstring &output, const wchar_t *format, va_list ap);

// format a string and append the result to a existing string
void StringAppendF(std::string &output, const char *format, ...);
void StringAppendF(std::wstring &output, const wchar_t *format, ...);
void StringAppendV(std::string &output, const char *format, va_list ap);
void StringAppendV(std::wstring &output, const wchar_t *format, va_list ap);

// find all tokens splitted by one of the characters in 'delimitor'
std::list<std::string> StringTokenize(const char *input, const char *delimitor);
std::list<std::wstring> StringTokenize(const wchar_t *input, const wchar_t *delimitor);
int StringTokenize(const std::string& input, const std::string& delimitor, std::list<std::string>& output);
int StringTokenize(const std::wstring& input, const std::wstring& delimitor, std::list<std::wstring>& output);

// replace all 'find' with 'replace' in the string
size_t StringReplaceAll(const std::string& find, const std::string& replace, std::string& output);
size_t StringReplaceAll(const std::wstring& find, const std::wstring& replace, std::wstring& output);

// print the binary data in a human-readable hexadecimal format
void BinaryToHexString(const void *binary, size_t length, std::string &output);
std::string BinaryToHexString(const void *binary, size_t length);
std::string BinaryToHexString(const std::string &binary);

void LowerString(std::string &str);
void LowerString(std::wstring &str);
void UpperString(std::string &str);
void UpperString(std::wstring &str);
std::string MakeLowerString(const std::string &src);
std::wstring MakeLowerString(const std::wstring &src);
std::string MakeUpperString(const std::string &src);
std::wstring MakeUpperString(const std::wstring &src);

// convert hexadecimal string to binary
// the functions will try to convert the characters in input stream until the end of the stream or an error occurs
int HexStringToBinary(const char *input, size_t length, std::string &output);
std::string HexStringToBinary(const std::string &input);

// convert a hexadecimal character to decimal 8-bit signed integer
// if c is not a hexadecimal character, -1 will be returned
char HexCharToInt8(char c);

// the following functions are used to convert encodings in utf-8、utf-16 and utf-32
std::wstring UTF8ToUTF16(const UTF8Char *utf8, size_t length);
std::string UTF16ToUTF8(const UTF16Char *utf16, size_t length);
std::basic_string<UTF32Char> UTF8ToUTF32(const UTF8Char *utf8, size_t length);
std::string UTF32ToUTF8(const UTF32Char *utf32, size_t length);
std::basic_string<UTF32Char> UTF16ToUTF32(const UTF16Char *utf16, size_t length);
std::wstring UTF32ToUTF16(const UTF32Char *utf32, size_t length);
std::wstring UTF8ToUTF16(const std::string &utf8);
std::string UTF16ToUTF8(const std::wstring &utf16);
std::basic_string<UTF32Char> UTF8ToUTF32(const std::string &utf8);
std::string UTF32ToUTF8(const std::basic_string<UTF32Char> &utf32);
std::basic_string<UTF32Char> UTF16ToUTF32(const std::wstring &utf16);
std::wstring UTF32ToUTF16(const std::basic_string<UTF32Char> &utf32);

// the following functions are used to validate encoded streamings, such as utf-8, gbk, big5, etc.
void UTF8CreateLengthTable(unsigned table[256]);
bool ValidateUTF8Stream(const void* stream, unsigned length);
bool ValidateGB2312Stream(const void* stream, unsigned length);
bool ValidateGBKStream(const void* stream, unsigned length);
bool ValidateBIG5Stream(const void* stream, unsigned length);

// trimming, removing extra spaces
std::string StringTrimLeft(const char *input);
std::string StringTrimRight(const char *input);
std::string StringTrim(const char *input); /* both left and right */
std::string& StringTrimLeft(std::string &input);
std::string& StringTrimRight(std::string &input);
std::string& StringTrim(std::string &input); /* both left and right */
std::wstring StringTrimLeft(const wchar_t *input);
std::wstring StringTrimRight(const wchar_t *input);
std::wstring StringTrim(const wchar_t *input); /* both left and right */
std::wstring& StringTrimLeft(std::wstring &input);
std::wstring& StringTrimRight(std::wstring &input);
std::wstring& StringTrim(std::wstring &input); /* both left and right */

}

#endif // BASE_UTIL_STRING_UTIL_H_
