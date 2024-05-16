#ifndef SIGMAFOX_CORE_STRINGUTILS_H
#define SIGMAFOX_CORE_STRINGUTILS_H
#include <string>

std::string string_to_uppercase(const std::string& str);
std::string string_to_lowercase(const std::string& str);
std::string string_tabulate_lines(size_t tab_size, const std::string &str);
std::string string_replace_all(const std::string& str, char match, char replace);
std::string string_remove_all(const std::string& str, char match);

#endif
