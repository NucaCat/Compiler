#include "Lexical_analyzer.h"

namespace lex {

	Lexical_analyzer::Lexical_analyzer(std::string path)
	{
		in.open(path, std::ios::binary);
	}


	Lexical_analyzer::~Lexical_analyzer()
	{
		in.close();
	}

	Lexeme Lexical_analyzer::scan()
	{
		State current_state{ State::start };
		char        char_buffer{};
		std::string string_buffer{};
		if (cache_valid)
		{
			cache_valid = false;
			in.seekg(cache_pos);
			return cache;
		}

		for (;;)
			switch (current_state)
			{
			case State::start:
			{
				if (in.get(char_buffer).fail())
				{
					current_state = State::end;
					continue;
				}
				if (char_buffer == '\n')
					str_number++;
				//digit check
				if (toolkit::is_digit(char_buffer))
				{
					current_state = State::number;
					in.unget();
					continue;
				}
				//letter check
				if (toolkit::is_letter(char_buffer))
				{
					current_state = State::identifier;
					in.unget();
					continue;
				}
				//quote check
				if (toolkit::is_quote(char_buffer))
				{
					current_state = State::single_quoted_literal;
					string_buffer.push_back('\'');
					//in.unget(); no unget there
					continue;
				}
				if (toolkit::is_double_quote(char_buffer))
				{
					current_state = State::double_quoted_literal;
					string_buffer.push_back('\"');
					//in.unget(); no unget there
					continue;
				}
				if (toolkit::is_slash(char_buffer))
				{
					current_state = State::additional_slash;
					//in.unget(); no unget there
					continue;
				}
				if (is_delimiter(char_buffer, Token::single_character_delimiter))
				{
					current_state = State::delimiter;
					//in.unget(); no unget there
					continue;
				}
			} break;
			case State::delimiter:
			{
				if (check_delimiter(char_buffer, Token::double_character_delimiter) == -1)
				{
					current_state = State::schar_delimiter;
					continue;
				}
				string_buffer.push_back(char_buffer);
				if (in.get(char_buffer).fail()) // check eof
				{
					string_buffer.clear();
					current_state = State::end;
					continue;
				}
				string_buffer.push_back(char_buffer);
				if (check_delimiter(string_buffer,
					Token::double_character_delimiter) != -1)
				{
					current_state = State::dchar_delimiter;
					continue;
				}
				char_buffer = string_buffer[0];
				string_buffer.clear();
				current_state = State::schar_delimiter;
				in.unget();
				continue;
			} break;
			case State::schar_delimiter:
			{
				return Lexeme{
					Token::single_character_delimiter,
					contains(std::string{ char_buffer }, Token::single_character_delimiter)
				};
			} break;
			case State::dchar_delimiter:
			{
				return Lexeme{
					Token::double_character_delimiter,
					contains(string_buffer, Token::double_character_delimiter)
				};
			} break;
			case State::number:
			{
				if (!in.get(char_buffer).fail())
				{
					if (toolkit::is_digit(char_buffer))
						string_buffer.push_back(char_buffer);
					else
					{
						Lexeme token;
						token.type = Token::number;
						token.value = contains(string_buffer, Token::number);
						string_buffer.clear();
						in.unget();
						return token;
					}
				}
				else
				{
					if (string_buffer.length() > 0)
					{
						Lexeme token;
						token.type = Token::number;
						token.value = contains(string_buffer, Token::number);
						return token;
					}
					else
					{
						current_state = State::end;
						continue;
					}
				}
			} break;
			case State::identifier:
			{
				if (!in.get(char_buffer).fail())
				{
					if (toolkit::is_symbol(char_buffer))
						string_buffer.push_back(char_buffer);
					else
					{
						Lexeme token;
						token.value = contains(string_buffer, Token::keyword);
						if (token.value != -1)
							token.type = Token::keyword;
						else
						{
							token.value = contains(string_buffer, Token::identifier);
							token.type = Token::identifier;
						}
						string_buffer.clear();
						in.unget();
						return token;
					}
				}
				else
				{
					if (string_buffer.length() > 0)
					{
						Lexeme token;
						token.value = contains(string_buffer, Token::keyword);
						if (token.value != -1)
							token.type = Token::keyword;
						else
						{
							token.value = contains(string_buffer, Token::identifier);
							token.type = Token::identifier;
						}
						return token;
					}
					else
					{
						current_state = State::end;
						continue;
					}
				}
			} break;
			case State::additional_slash:
			{
				char prev_char = char_buffer;
				if (in.get(char_buffer).fail())
				{
					current_state = State::end;
					continue;
				}
				if (toolkit::is_slash(char_buffer))
				{
					current_state = State::dslash_commentary;
					continue;
				}
				if (char_buffer == '*')
				{
					current_state = State::slash_commentary;
					continue;
				}
				in.unget();
				char_buffer = prev_char;
				current_state = State::schar_delimiter;
				continue;
			} break;
			case State::dslash_commentary:
			{
				if (!in.get(char_buffer).fail())
				{
					if (char_buffer == '\n')
					{
						str_number++;
						current_state = State::start;
					}
					continue;
				}
				else
				{
					current_state = State::end;
					continue;
				}
			} break;
			case State::slash_commentary:
			{
				if (in.get(char_buffer).fail())
				{
					current_state = State::end;
				}
				if (char_buffer == '\n')
					str_number++;
				else if (char_buffer == '*')
					current_state = State::slash_commentary_additional;
				continue;
			} break;
			case State::slash_commentary_additional:
			{
				if (in.get(char_buffer).fail())
				{
					current_state = State::end;
					continue;
				}
				else  if (toolkit::is_slash(char_buffer))
					current_state = State::start;
				else
					current_state = State::slash_commentary;
				continue;
			} break;
			case State::single_quoted_literal:
			{
				if (in.get(char_buffer).fail())
				{
					current_state = State::end;
					continue;
				}
				if (toolkit::is_quote(char_buffer))
				{
					string_buffer.push_back('\'');
					Lexeme token;
					token.type = Token::single_quoted_literal;
					token.value = contains(string_buffer, Token::single_quoted_literal);
					string_buffer.clear();
					return token;
				}
				else
					string_buffer.push_back(char_buffer);
				if (toolkit::is_slash(char_buffer))
				{
					current_state = State::sqliteral_slash;
					continue;
				}
			} break;
			case State::double_quoted_literal:
			{
				if (in.get(char_buffer).fail())
				{
					current_state = State::end;
					continue;
				}
				if (toolkit::is_double_quote(char_buffer))
				{
					string_buffer.push_back('\"');
					return Lexeme{
						Token::double_quoted_literal,
						contains(string_buffer, Token::double_quoted_literal) };
				}
				else
					string_buffer.push_back(char_buffer);
				if (toolkit::is_slash(char_buffer))
				{
					current_state = State::dqliteral_slash;
					continue;
				}
			} break;
			case State::sqliteral_slash:
			{
				if (!in.get(char_buffer).fail())
				{
					string_buffer.push_back(char_buffer);
					current_state = State::single_quoted_literal;
				}
				else
					current_state = State::end;
				continue;
			} break;
			case State::dqliteral_slash:
			{
				if (!in.get(char_buffer).fail())
				{
					string_buffer.push_back(char_buffer);
					current_state = State::single_quoted_literal;
				}
				else
					current_state = State::end;
				continue;
			} break;
			case State::end:
			{
				return make_end_lexeme();
			} break;
			}
	}

	Lexeme Lexical_analyzer::scan_v()
	{
		if (cache_valid)
		{
			return cache;
		}

		auto pos = in.tellg();
		auto token = scan();
		cache_pos = in.tellg();
		in.seekg(pos);

		cache = token;
		cache_valid = true;
		return token;
	}

	void Lexical_analyzer::debug()
	{
		auto pos = in.tellg();
		Lexeme token{ scan() };
		while (token.type != Token::eof)
		{
			std::cout << get_token_representation(token) << '\n';
			token = scan();
		}
		in.seekg(pos);
	}

	void Lexical_analyzer::formatted_debug()
	{
		auto pos = in.tellg();
		auto prev = get_str_number();
		Lexeme token{ scan() };
		while (token.type != Token::eof)
		{
			std::cout << get_token_representation(token);
			token = scan();
			if (prev < get_str_number())
				std::cout << '\n';
			else std::cout << ' ';
			prev = get_str_number();
		}
		in.seekg(pos);
	}

	int Lexical_analyzer::get_str_number()
	{
		return str_number;
	}

	/*std::string Lexical_analyzer::get_by_value(Lexeme t)
	{
		int index{ get_token_number(t.type) };
		return tables[index][t.value];
	}*/

	bool Lexical_analyzer::is_delimiter(
		char letter, Token t)
	{
		if (contains(std::string{ letter }, t) == -1)
			return false;
		else return true;
	}

	int Lexical_analyzer::check_delimiter(
		char letter,
		Token t,
		int position)
	{
		int index{ get_token_number(t) };
		int i = 0;
		for (auto x : tables[index])
		{
			if (x[position] == letter)
				return i;
			i++;
		}
		return -1;
	}

	int Lexical_analyzer::check_delimiter(
		std::string str,
		Token t)
	{
		int index{ get_token_number(t) };
		int i = 0;
		for (auto x : tables[index])
		{
			if (x == str)
				return i;
			i++;
		}
		return -1;
	}

	int Lexical_analyzer::contains(
		std::string key,
		Token t)
	{
		int index{ get_token_number(t) };
		unsigned int i = 0;
		for (auto x : tables[index])
		{
			{
				if (x == key)
					return i;
			}
			i++;
		}
		if (index >= 3)//push to non-const table
		{
			tables[index].push_back(key);
			return static_cast<int>(tables[index].size()) - 1;
		}
		else return -1;

	}

	int Lexical_analyzer::get_token_number(Token t)
	{
		switch (t)
		{
		case Token::keyword: return 0;
		case Token::double_character_delimiter: return 1;
		case Token::single_character_delimiter: return 2;
		case Token::identifier: return 3;
		case Token::double_quoted_literal: return 4;
		case Token::single_quoted_literal: return 5;
		case Token::number: return 6;
		default: return -1;
		}

	}

	Lexeme Lexical_analyzer::make_end_lexeme()
	{
		return Lexeme{ Token::eof, 0 };
	}

	std::ostream& operator<<(std::ostream& of, Lexeme token)
	{
		switch (token.type)
		{
		case Token::keyword:
		{
			of << "keyword ";
			break;
		}
		case Token::identifier:
		{
			of << "identifier ";
			break;
		}
		case Token::single_quoted_literal:
		{
			of << "single quoted literal ";
			break;
		}
		case Token::double_quoted_literal:
		{
			of << "double quoted literal ";
			break;
		}
		case Token::single_character_delimiter:
		{
			of << "single character delimiter ";
			break;
		}
		case Token::double_character_delimiter:
		{
			of << "double character delimiter ";
			break;
		}
		case Token::eof:
		{
			of << "eof ";
			break;
		}
		case Token::number:
		{
			of << "number ";
			break;
		}
		default:
		{
			of << "unknown lexeme ";
			break;
		}
		}
		of << token.value;
		return of;
	}

	std::string Lexical_analyzer::get_token_representation(Lexeme t)
	{
		int index{ get_token_number(t.type) };
		if (index != -1)
			return tables[index][t.value];
		return "end of file";
	}
	int Lexical_analyzer::find_identifier(std::string s)
	{
		for (int i{}; i != identifiers.size(); ++i)
			if (identifiers[i].name == s)
				return i;
		return -1;
	}
}
