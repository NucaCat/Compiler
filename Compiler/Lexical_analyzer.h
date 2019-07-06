#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "toolkit.h"

namespace lex {
	enum class Token {
		keyword = '0',
		identifier,
		single_quoted_literal,
		double_quoted_literal,
		single_character_delimiter,
		double_character_delimiter,
		number,
		eof
	};

	struct Lexeme
	{
		Token type;
		int value;
	};

	struct Identifier {
		std::string name{};
		std::string type{};
		std::string array_type{};
		std::string array_size{};
		Identifier(std::string n) : name{ n } {}
	};

	enum class State {
		start = '0',
		number,
		identifier,
		single_quoted_literal,
		double_quoted_literal,
		additional_slash,
		delimiter,
		schar_delimiter,
		dchar_delimiter,
		dslash_commentary,
		slash_commentary,
		slash_commentary_additional,
		sqliteral_slash,
		dqliteral_slash,
		end
	};

	std::ostream& operator<<(std::ostream& of, Lexeme token);


	class Lexical_analyzer
	{
	public:
		Lexical_analyzer(std::string path);
		~Lexical_analyzer();
		void debug();
		void formatted_debug();
		Lexeme scan();
		Lexeme scan_v();
		int contains(std::string, Token);
		int get_str_number();
		//std::string get_by_value(Lexeme);
		std::string get_token_representation(Lexeme t);
		int find_identifier(std::string s);

		friend std::ostream& operator<<(std::ostream& of, Lexeme token);
		std::vector<std::string> variables;
		std::vector<Identifier> identifiers;

	private:
		std::ifstream in{}; // поток для анализа
		Lexeme cache{};//кэш
		bool cache_valid{};//действителен ли кэш
		std::streampos cache_pos{};
		int str_number{ 1 };//номер строки



		std::vector <std::vector<std::string>> tables{ toolkit::get_tables() };


		bool is_delimiter(char letter, Token);

		int check_delimiter(char letter, Token, int position = 0);
		//check schar delimiter if it's in table dchar delimiter

		int check_delimiter(std::string str, Token);

		int get_token_number(Token t);

		Lexeme make_end_lexeme();
	};
}

