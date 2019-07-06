#pragma once
#include "Lexical_analyzer.h"

namespace synt {
	class Syntactical_analyzer {
	public:
		Syntactical_analyzer(std::string path);
		~Syntactical_analyzer();
		bool analysis();

		void debug();
		void formatted_debug();
		void _l();

		std::string next_label();
		std::ofstream in_cod{ "cod.asm" };
		std::ofstream in_dat{ "dat.asm" };

	private:

		lex::Lexical_analyzer* lex;

		void logical_expr();
		void tl();
		void fl();
		std::string zn();

		void arithmetic_expr();
		void t();
		void f();

		void program();
		void define_block();
		void type();
		void simple_type();
		void operator_block();
		void oper();
		void oper_read();
		void oper_write();
		void oper_while();
		void oper_if();
		void oper_assign();
		void oper_do();
		void vvodInt(lex::Identifier);
		void vvodBool(lex::Identifier);
		void vvodChar(lex::Identifier);
		void vivodInt();
		void vivodChar(lex::Identifier);
		void vivodBool(lex::Identifier);
		void vivodLiteral(std::string);

		int label_counter{ 28 };
		[[noreturn]] void error(int code);
	};
}
