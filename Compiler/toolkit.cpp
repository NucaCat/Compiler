#include "toolkit.h"
#include <string>
#include <vector>
#include <gsl/gsl>
namespace toolkit {
	bool is_letter(char letter)
	{
		return (
			(letter >= 'a' && letter <= 'z')
			|| (letter >= 'A' && letter <= 'Z')
			|| (letter == '_')
			);
	}

	bool is_digit(char letter)
	{
		return (letter <= '9' && letter >= '0');
	}

	bool is_symbol(char letter)
	{
		return (
			is_letter(letter)
			|| is_digit(letter)
			);
	}

	bool is_quote(char letter)
	{
		return (letter == '\'');
	}

	bool is_backslash(char letter)
	{
		return (letter == '\\');
	}

	bool is_slash(char letter)
	{
		return (letter == '/');
	}

	bool is_double_quote(char letter)
	{
		return (letter == '\"');
	}

	bool is_space(char letter)
	{
		return (letter == ' ');
	}

	int toInt(char s)
	{
		return s + (0 - '0');
	}

	long long build_number(std::string str)
	{
		long long num{};
		for (int i = gsl::narrow_cast<int>(str.length() - 1), multiplier{ 1 }; i >= 0; --i, multiplier *= 10)
			num += (toInt(str[i])) * multiplier;
		return num;
	}

	std::vector<std::string>
		get_keywords()
	{
		return std::move
		(
			std::vector<std::string>
		{
			"alignas",
				"alignof",
				"and",
				"and_eq",
				"asm",
				"auto",
				"bitand",
				"bitor",
				"bool",
				"break",
				"case",
				"catch",
				"char",
				"char16_t",
				"char32_t",
				"class",
				"compl",
				"const",
				"constexpr",
				"const_cast",
				"continue",
				"decltype",
				"default",
				"delete",
				"do",
				"double",
				"dynamic_cast",
				"else",
				"enum",
				"explicit",
				"export",
				"extern",
				"false",
				"float",
				"for",
				"friend",
				"goto",
				"if",
				"inline",
				"int",
				"long",
				"mutable",
				"namespace",
				"new",
				"noexcept",
				"not",
				"not_eq",
				"nullptr",
				"operator",
				"or",
				"or_eq",
				"private",
				"protected",
				"public",
				"register",
				"reinterpret_cast",
				"return",
				"short",
				"signed",
				"sizeof",
				"static",
				"static_assert",
				"static_cast",
				"struct",
				"switch",
				"template",
				"this",
				"thread_local",
				"throw",
				"true",
				"try",
				"typedef",
				"typeid",
				"typename",
				"union",
				"unsigned",
				"using",
				"virtual",
				"void",
				"volatile",
				"wchar_t",
				"while",
				"xor",

				"Program",
				"Var",
				"Begin",
				"read",
				"End",
				"let",
				"then",
				"write",
				"Array",
				"until",
				"loop",
				"to",
				"next",
				"dummy",
				"step"
		}
		);
	}

	std::vector<std::string>
		get_schar_delimiters()
	{
		return std::move
		(
			std::vector<std::string>
		{
			"<",
				">",
				"=",
				"#",
				"/",
				"*",
				"%",
				"^",
				"|",
				"&",
				"~",
				".",
				"?",
				"!",
				"+",
				"-",
				";",
				":",
				"{",
				"}",
				"(",
				")",
				"[",
				"]",
				","
		}
		);
	}

	std::vector<std::string>
		get_dchar_delimiters()
	{
		return std::move
		(
			std::vector<std::string>
		{
			"<<",
				">>",
				"==",
				"!=",
				">=",
				"<=",
				"&&",
				"||",
				"+=",
				"-=",
				"*=",
				"/=",
				"%=",
				"&=",
				"|=",
				"^=",
				"++",
				"->",
				"::",
				"?:",
				"->",
				"--",
				"/*",
				"*/"
		}
		);
	}

	std::vector <std::vector<std::string>>
		get_tables()
	{
		std::vector <std::vector<std::string>> tables{
			get_keywords(), get_dchar_delimiters(), get_schar_delimiters(), //const tables
			std::vector<std::string>{},//id
			std::vector<std::string>{},//dq_literals
			std::vector<std::string>{},//sq_literals
			std::vector<std::string>{} };//numbers
		return std::move(tables);
	}
}