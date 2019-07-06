#pragma once
#include <vector>
namespace toolkit {
	bool is_letter(char letter);

	bool is_digit(char letter);

	bool is_symbol(char letter);

	bool is_quote(char letter);

	bool is_backslash(char letter);

	bool is_slash(char letter);

	bool is_double_quote(char letter);

	bool is_space(char letter);

	bool is_double_quote(char letter);

	int toInt(char s);

	long long build_number(std::string str);

	std::vector<std::string> get_keywords();

	std::vector<std::string> get_schar_delimiters();

	std::vector<std::string> get_dchar_delimiters();

	std::vector <std::vector<std::string>> get_tables();
}