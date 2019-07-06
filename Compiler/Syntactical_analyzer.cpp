#include "Syntactical_analyzer.h"

using lex::Lexeme;
using lex::Token;

synt::Syntactical_analyzer::Syntactical_analyzer(std::string path)
{
	lex = new lex::Lexical_analyzer(path);
	in_cod << ".model small \n"
		<< "INCLUDE dat.asm \n";
}

synt::Syntactical_analyzer::~Syntactical_analyzer()
{
	delete lex;
}

void synt::Syntactical_analyzer::debug()
{
	lex->debug();
}

void synt::Syntactical_analyzer::formatted_debug()
{
	lex->formatted_debug();
}

void synt::Syntactical_analyzer::_l()
{
	for (auto& x : lex->identifiers)
		std::cout << "\nname: " << x.name
		<< "\ntype: " << x.type
		<< "\narray type: " << x.array_type
		<< "\nsize: " << x.array_size << "\n";
}

std::string synt::Syntactical_analyzer::next_label()
{
	return std::string("@" + std::to_string(label_counter++));
}

bool synt::Syntactical_analyzer::analysis()
{
	program();
	return true;
}

//lg -> tl {'or' tl}
void synt::Syntactical_analyzer::logical_expr()
{
	tl();
	Lexeme token = lex->scan_v();
	while (!((token.type == Token::single_character_delimiter
		&& token.value == lex->contains(")", Token::single_character_delimiter))
		|| (token.type == Token::single_character_delimiter
			&& token.value == lex->contains(";", Token::single_character_delimiter)))
		)
	{
		if (token.type == Token::keyword
			&& token.value == lex->contains("or", Token::keyword))
		{
			token = lex->scan();
			tl();
			in_cod << "pop bx \n"
				<< "pop ax \n"
				<< "or ax, bx \n"
				<< "push ax \n";
		}
		else
		{
			error(1);
		}
		token = lex->scan_v();
	}
}

//tl -> fl {'and' fl}
void synt::Syntactical_analyzer::tl()
{
	fl();
	Lexeme token = lex->scan_v();
	while (!((token.type == Token::single_character_delimiter
		&& token.value == lex->contains(")", Token::single_character_delimiter))
		|| (token.type == Token::single_character_delimiter
			&& token.value == lex->contains(";", Token::single_character_delimiter))
		|| (token.type == Token::keyword
			&& token.value == lex->contains("or", Token::keyword))
		/*|| (token.type == Token::keyword
	&& token.value == lex->contains("then", Token::keyword))*/
		))
	{
		/*if (token.type == Token::single_character_delimiter
			&& token.value == lex->contains("(", Token::single_character_delimiter))
			token = lex->scan();

		else*/ if (token.type == Token::keyword
			&& token.value == lex->contains("and", Token::keyword))
		{
			token = lex->scan();
			fl();
			in_cod << "pop bx \n"
				<< "pop ax \n"
				<< "and ax, bx \n"
				<< "push ax \n";
		}
		else
		{
			error(2);
		}
	token = lex->scan_v();
	}
}

/*fl -> 'true' | 'false' | 'not' fl | '(' lg ')'
	| '[' e zn e ']' | I ('[' e ']')
*/
void synt::Syntactical_analyzer::fl()
{
	Lexeme token = lex->scan_v();
	auto num = token;
	std::string trueLabel, falseLabel;
	if (token.type == Token::identifier)
	{
		token = lex->scan();
		auto type = lex->identifiers[lex->find_identifier(lex->get_token_representation(token))];
		//std::cout << token;
		in_cod << "mov di, 0 \n";
		token = lex->scan_v();
		if (token.type == Token::single_character_delimiter && token.value == lex->contains("[", Token::single_character_delimiter))
		{
			token = lex->scan();
			arithmetic_expr();
			in_cod << "pop di \n";
			token = lex->scan();
			if (!(token.type == Token::single_character_delimiter && token.value == lex->contains("]", Token::single_character_delimiter)))
			{
				error(51);
			}
		}
		in_cod << "xor ah, ah \n"
			<< "mov al, " << type.name << "[di] \n"
			<< "push ax \n";
	}
	else if (token.type == Token::keyword
		&& token.value == lex->contains("true", Token::keyword))
	{
		token = lex->scan();
		in_cod << "push 1 \n";
	}
	else if (token.type == Token::keyword
		&& token.value == lex->contains("false", Token::keyword))
	{
		token = lex->scan();
		in_cod << "push 0 \n";
	}
	else if (token.type == Token::single_character_delimiter
		&& token.value == lex->contains("(", Token::single_character_delimiter))
	{
		token = lex->scan();
		logical_expr();
		token = lex->scan_v();
		if (token.type == Token::single_character_delimiter
			&& token.value == lex->contains(")", Token::single_character_delimiter))
		{
			token = lex->scan();
		}
		else error(3);
	}
	else if ((token.type == Token::single_character_delimiter
		&& token.value == lex->contains("[", Token::single_character_delimiter)))
	{
		token = lex->scan();
		arithmetic_expr();
		auto znak = zn();
		arithmetic_expr();
		trueLabel = next_label();
		falseLabel = next_label();
		in_cod << "pop bx \n"
			<< "pop ax \n"
			<< "cmp ax, bx \n";

		if (znak == "==")
			in_cod << "je " << trueLabel << '\n';
		else if (znak == "!=")
			in_cod << "jne " << trueLabel << '\n';
		else if (znak == "<=")
			in_cod << "jbe " << trueLabel << '\n';
		else if (znak == ">=")
			in_cod << "jae " << trueLabel << '\n';
		else if (znak == ">")
			in_cod << "ja " << trueLabel << '\n';
		else if (znak == "<")
			in_cod << "jb " << trueLabel << '\n';

		in_cod << "push 0 \n"
			<< "jmp " << falseLabel << '\n'
			<< trueLabel << ": \n"
			<< "push 1 \n"
			<< falseLabel << ": \n";

		token = lex->scan_v();
		if (token.type == Token::single_character_delimiter
			&& token.value == lex->contains("]", Token::single_character_delimiter))
		{
			token = lex->scan();
		}
		else error(5);
	}
	else if (token.type == Token::keyword
		&& token.value == lex->contains("not", Token::keyword))
	{
		token = lex->scan();
		fl();
		in_cod << "pop ax \n"
			<< "not ax \n"
			<< "push ax \n";
	}
	else error(4);
}

//zn -> '>' | '<' | '==' | '!=' | '>=' | '<='
std::string synt::Syntactical_analyzer::zn()
{
	Lexeme token = lex->scan_v();
	if (token.type == Token::single_character_delimiter
		&& token.value == lex->contains(">", Token::single_character_delimiter))
	{
		token = lex->scan();
	}
	else if (token.type == Token::single_character_delimiter
		&& token.value == lex->contains("<", Token::single_character_delimiter))
	{
		token = lex->scan();
	}
	else if (token.type == Token::double_character_delimiter
		&& token.value == lex->contains(">=", Token::double_character_delimiter))
	{
		token = lex->scan();
	}
	else if (token.type == Token::double_character_delimiter
		&& token.value == lex->contains("<=", Token::double_character_delimiter))
	{
		token = lex->scan();
	}
	else if (token.type == Token::double_character_delimiter
		&& token.value == lex->contains("!=", Token::double_character_delimiter))
	{
		token = lex->scan();
	}
	else if (token.type == Token::double_character_delimiter
		&& token.value == lex->contains("==", Token::double_character_delimiter))
	{
		token = lex->scan();
	}
	else error(6);
	return lex->get_token_representation(token);
}

//e -> T {+T | -T}
void synt::Syntactical_analyzer::arithmetic_expr()
{
	t();
	Lexeme token = lex->scan_v();
	while
		(!((token.type == Token::single_character_delimiter)
			&& (token.value == lex->contains(")", Token::single_character_delimiter)
				|| token.value == lex->contains("]", Token::single_character_delimiter)
				|| token.value == lex->contains(";", Token::single_character_delimiter)
				|| token.value == lex->contains(",", Token::single_character_delimiter)
				|| token.value == lex->contains(">", Token::single_character_delimiter)
				|| token.value == lex->contains("<", Token::single_character_delimiter))
			|| (token.type == Token::double_character_delimiter)
			&& (token.value == lex->contains(">=", Token::double_character_delimiter)
				|| token.value == lex->contains("<=", Token::double_character_delimiter)
				|| token.value == lex->contains("==", Token::double_character_delimiter)
				|| token.value == lex->contains("!=", Token::double_character_delimiter))
			|| (token.type == Token::keyword
				&& token.value == lex->contains("to", Token::keyword))))
	{
		if (token.value == lex->contains("+", Token::single_character_delimiter))
		{
			token = lex->scan();
			t();
			in_cod << "pop bx \n"
				<< "pop ax \n"
				<< "add ax, bx \n"
				<< "push ax \n";
		}
		else if (token.value == lex->contains("-", Token::single_character_delimiter))
		{
			token = lex->scan();
			t();
			in_cod << "pop bx \n"
				<< "pop ax \n"
				<< "sub ax, bx \n"
				<< "push ax \n";
		}
		else
		{
			error(7);
		}
		token = lex->scan_v();
	}
}

//T -> F {*F | /F}
void synt::Syntactical_analyzer::t()
{
	f();
	Lexeme token = lex->scan_v();
	while (!((token.type == Token::single_character_delimiter)
		&& (token.value == lex->contains(")", Token::single_character_delimiter)
			|| token.value == lex->contains("]", Token::single_character_delimiter)
			|| token.value == lex->contains(";", Token::single_character_delimiter)
			|| token.value == lex->contains(">", Token::single_character_delimiter)
			|| token.value == lex->contains(",", Token::single_character_delimiter)
			|| token.value == lex->contains("<", Token::single_character_delimiter)
			|| token.value == lex->contains("+", Token::single_character_delimiter)
			|| token.value == lex->contains("-", Token::single_character_delimiter))
		|| (token.type == Token::double_character_delimiter)
		&& (token.value == lex->contains(">=", Token::double_character_delimiter)
			|| token.value == lex->contains("<=", Token::double_character_delimiter)
			|| token.value == lex->contains("==", Token::double_character_delimiter)
			|| token.value == lex->contains("!=", Token::double_character_delimiter))
		|| (token.type == Token::keyword
			&& token.value == lex->contains("to", Token::keyword))))
	{
		if (token.value == lex->contains("*", Token::single_character_delimiter))
		{
			token = lex->scan();
			f();
			in_cod << "pop bx\n"
				<< "pop ax \n"
				<< "mul bx \n"
				<< "push ax \n";
		}
		else if (token.value == lex->contains("/", Token::single_character_delimiter))
		{
			token = lex->scan();
			f();
			in_cod << "pop bx \n"
				<< "pop ax \n"
				<< "xor dx, dx \n"
				<< "div bx \n"
				<< "push ax \n";
		}
		else
		{
			error(8);
		}
		token = lex->scan_v();
	}
}

//F -> '(' e ')' | I ('[' e ']') | c
void synt::Syntactical_analyzer::f()
{
	Lexeme token = lex->scan_v();
	if (token.type == Token::number)
	{
		token = lex->scan();
		in_cod << "mov ax, "
			<< lex->get_token_representation(token) << '\n'
			<< "push ax \n";
	}
	else if (token.type == Token::single_character_delimiter
		&& token.value == lex->contains("(", Token::single_character_delimiter))
	{
		token = lex->scan();
		arithmetic_expr();
		token = lex->scan_v();
		if (token.type == Token::single_character_delimiter
			&& token.value == lex->contains(")", Token::single_character_delimiter))
		{
			token = lex->scan();
		}
		else error(9);
	}
	else if (token.type == Token::identifier)
	{
		token = lex->scan();
		auto num = token;//save identifier
		in_cod << "mov di, 0 \n";
		token = lex->scan_v();
		if (token.type == Token::single_character_delimiter && token.value == lex->contains("[", Token::single_character_delimiter))
		{
			token = lex->scan();
			arithmetic_expr();
			in_cod << "pop di \n"
				<< "shl di, 1 \n";
			token = lex->scan();
			if (!(token.type == Token::single_character_delimiter && token.value == lex->contains("]", Token::single_character_delimiter)))
			{
				error(49);
			}

		}
		in_cod << "mov   ax," << lex->identifiers[lex->find_identifier(lex->get_token_representation(num))].name << "[di] \n"
			<< "push ax \n";
	}
	else error(10);
}

//program -> 'Program' I define_block operator_block '.'
void synt::Syntactical_analyzer::program()
{
	Lexeme token = lex->scan();
	if (!(token.type == Token::keyword
		&& token.value == lex->contains("Program", Token::keyword)))
	{
		error(11);
	}
	token = lex->scan();
	if (!(token.type == Token::identifier))
	{
		error(12);
	}
	define_block();
	in_cod << ".stack 100h \n"
		<< ".code \n"
		<< ".386 \n"
		<< "start: \n"
		<< "mov ax, @data \n"
		<< "mov   ds, ax \n";

	operator_block();
	token = lex->scan();
	if (!(token.type == Token::single_character_delimiter
		&& token.value == lex->contains(".", Token::single_character_delimiter)))
	{
		error(13);
	}
	in_cod << "error: \n"
		<< "mov ax, 4C00h \n"
		<< "int 21h \n"
		<< "end start \n";
}

//define_block -> 'Var' {I (',' | 0) ':' type}
void synt::Syntactical_analyzer::define_block()
{
	in_dat << ".data \n"
		<< "; SUPPORTING VARIABLES \n"
		<< "@buffer   db		6 \n"
		<< "blength   db      (?) \n"
		<< "@buf      db      256 DUP (?) \n"
		<< "clrf      db      0Dh, 0Ah, \"$\" \n"
		<< "output    db      6 DUP (?), \"$\" \n"
		<< "err_msg   db      \"Input error, try again\", 0Dh, 0Ah, \"$\" \n"
		<< "@true     db      \"true\" \n"
		<< "@@true    db      \"true$\" \n"
		<< "@false    db      \"false\" \n"
		<< "@@false   db      \"false$\" \n"
		<< "; USING VARIABLES \n";


	Lexeme token = lex->scan();
	if (!(token.type == Token::keyword
		&& token.value == lex->contains("Var", Token::keyword)))
	{
		error(14);
	}
	token = lex->scan_v();
	while ((!(token.type == Token::keyword
		&& token.value == lex->contains("Begin", Token::keyword))))
	{
		lex->variables.clear();
		token = lex->scan();
		lex->variables.push_back(lex->get_token_representation(token));
		if (!(token.type == Token::identifier))
		{
			error(15);
		}
		token = lex->scan_v();
		while ((!(token.type == Token::single_character_delimiter
			&& token.value == lex->contains(":", Token::single_character_delimiter))))
		{
			token = lex->scan();
			if ((!(token.type == Token::single_character_delimiter
				&& token.value == lex->contains(",", Token::single_character_delimiter))))
			{
				error(16);
			}
			token = lex->scan();
			lex->variables.push_back(lex->get_token_representation(token));
			if (!(token.type == Token::identifier))
			{
				error(17);
			}
			token = lex->scan_v();
		}
		if ((!(token.type == Token::single_character_delimiter
			&& token.value == lex->contains(":", Token::single_character_delimiter))))
		{
			error(18);
		}
		token = lex->scan();
		type();
		token = lex->scan();
		if ((!(token.type == Token::single_character_delimiter
			&& token.value == lex->contains(";", Token::single_character_delimiter))))
		{
			error(19);
		}
		token = lex->scan_v();
	}

	for (auto& x : lex->identifiers)
	{
		in_dat << x.name;

		if (x.array_type == "int")
			in_dat << " dw ";
		else if (x.array_type == "bool")
			in_dat << " db ";
		else if (x.array_type == "char")
			in_dat << " db ";

		if (!x.array_size.empty())
		{
			in_dat << ' ' << x.array_size << " DUP (?) \n";
		}
		else in_dat << " (?) \n";
	}

}

//type -> 'Array' '[' c ']' simple_type
void synt::Syntactical_analyzer::type()
{
	Lexeme token = lex->scan_v();
	if (token.type == Token::keyword)
	{
		if (token.value == lex->contains("Array", Token::keyword))
		{
			token = lex->scan();
			token = lex->scan();
			if (!(token.type == Token::single_character_delimiter
				&& token.value == lex->contains("[", Token::single_character_delimiter)))
			{
				error(20);
			}
			token = lex->scan();
			for (auto& x : lex->variables)
			{
				if (lex->find_identifier(x) == -1)
					lex->identifiers.push_back(x);
				lex->identifiers[lex->find_identifier(x)].type = "Array";
				lex->identifiers[lex->find_identifier(x)].array_size = lex->get_token_representation(token);
			}

			if (!(token.type == Token::number))
			{
				error(21);
			}
			token = lex->scan();
			if (!(token.type == Token::single_character_delimiter
				&& token.value == lex->contains("]", Token::single_character_delimiter)))
			{
				error(22);
			}
		}
		simple_type();
	}
	else
	{
		error(23);
	}
}

//simple_type -> 'int' | 'char' | 'bool'
void synt::Syntactical_analyzer::simple_type()
{
	Lexeme token = lex->scan_v();
	if (token.type == Token::keyword)
	{
		if (token.value == lex->contains("int", Token::keyword)
			|| token.value == lex->contains("bool", Token::keyword)
			|| token.value == lex->contains("char", Token::keyword))
		{
			token = lex->scan();
			std::string type = lex->get_token_representation(token);
			for (auto& x : lex->variables)
			{
				if (lex->find_identifier(x) == -1)
					lex->identifiers.push_back(x);
				lex->identifiers[lex->find_identifier(x)].name = x;
				lex->identifiers[lex->find_identifier(x)].array_type = type;
			}
		}

		else
		{
			error(24);
		}
	}
	else
	{
		error(25);
	}
}

//oper_block -> 'Begin' {oper (';' | 0)} 'End'
void synt::Syntactical_analyzer::operator_block()
{
	Lexeme token = lex->scan();
	if (!(token.type == Token::keyword
		&& token.value == lex->contains("Begin", Token::keyword)))
	{
		error(26);
	}
	token = lex->scan_v();
	while (!(token.type == Token::keyword
		&& token.value == lex->contains("End", Token::keyword)))
	{
		oper();
		token = lex->scan_v();

		if (token.type == Token::single_character_delimiter
			&& token.value == lex->contains(";", Token::single_character_delimiter))
		{
			token = lex->scan();
			token = lex->scan_v();
		}
	}
	token = lex->scan();
}

//oper -> oper_read | oper_write | oper_while |  oper_if | oper_block | oper_assign
void synt::Syntactical_analyzer::oper()
{
	Lexeme token = lex->scan_v();
	if (token.type == Token::keyword)
	{
		std::string value = lex->get_token_representation(token);
		if (value == "read")
		{
			oper_read();
			return;
		}
		else if (value == "write")
		{
			oper_write();
			return;
		}
		else if (value == "while")
		{
			oper_while();
			return;
		}
		else if (value == "if")
		{
			oper_if();
			return;
		}
		else if (value == "Begin")
		{
			operator_block();
			return;
		}
		else if (value == "do")
		{
			oper_do();
			return;
		}
		
		//else oper_assign();
	}
	oper_assign();
	//else error(28);
}

//oper_read -> 'read' '('{ I('[' e ']') (',' | 0) } ')'
void synt::Syntactical_analyzer::oper_read()
{
	in_cod << "; READ() \n";
	Lexeme token = lex->scan();
	if (!(token.type == Token::keyword
		&& token.value == lex->contains("read", Token::keyword)))
	{
		error(29);
	}
	token = lex->scan();
	if (!(token.type == Token::single_character_delimiter
		&& token.value == lex->contains("(", Token::single_character_delimiter)))
	{
		error(30);
	}
	token = lex->scan_v();
	while (!(token.type == Token::single_character_delimiter
		&& token.value == lex->contains(")", Token::single_character_delimiter)))
	{

		if (token.type == Token::identifier)
		{
			token = lex->scan();
		}
		else error(31);

		auto type = lex->identifiers[lex->find_identifier(lex->get_token_representation(token))];
		in_cod << "push  0 \n";

		token = lex->scan_v();
		if (token.type == Token::single_character_delimiter
			&& token.value == lex->contains("[", Token::single_character_delimiter))
		{
			token = lex->scan();
			arithmetic_expr();
			token = lex->scan();
			if (!(token.type == Token::single_character_delimiter
				&& token.value == lex->contains("]", Token::single_character_delimiter)))
			{
				error(32);
			}

			token = lex->scan_v();
		}
		if (type.array_type == "int")
			vvodInt(type);
		else if (type.array_type == "char")
			vvodChar(type);
		else if (type.array_type == "bool")
			vvodBool(type);
		if (token.type == Token::single_character_delimiter
			&& token.value == lex->contains(",", Token::single_character_delimiter))
		{
			token = lex->scan();
			token = lex->scan_v();
		}
	}
	token = lex->scan();
	if (!(token.type == Token::single_character_delimiter
		&& token.value == lex->contains(")", Token::single_character_delimiter)))
		error(33);
}

//oper_write -> 'write' '(' { literal  | e | I ('[' e ']')  (',' | 0) } ')'
void synt::Syntactical_analyzer::oper_write()
{
	in_cod << "; WRITE() \n";
	Lexeme token = lex->scan();
	if (!(token.type == Token::keyword
		&& token.value == lex->contains("write", Token::keyword)))
	{
		error(34);
	}
	token = lex->scan();
	if (!(token.type == Token::single_character_delimiter
		&& token.value == lex->contains("(", Token::single_character_delimiter)))
	{
		error(35);
	}
	token = lex->scan_v();
	while (!(token.type == Token::single_character_delimiter
		&& token.value == lex->contains(")", Token::single_character_delimiter)))
	{
		/*if (token.type == Token::identifier)
		{
			auto type = lex->identifiers[lex->find_identifier(lex->get_token_representation(token))];

			token = lex->scan();
			token = lex->scan_v();
			if (token.type == Token::single_character_delimiter
				&& token.value == lex->contains("[", Token::single_character_delimiter))
			{
				token = lex->scan();
				arithmetic_expr();

				token = lex->scan();
				if (!(token.type == Token::single_character_delimiter
					&& token.value == lex->contains("]", Token::single_character_delimiter)))
				{
					error(37);
				}
				token = lex->scan_v();
			}
		}

		else */
		
		
		//std::cout<<type.array_type;
		if (token.type == Token::single_quoted_literal)
		{
			token = lex->scan();
			vivodLiteral(lex->get_token_representation(token));
		}
		else if (token.type == Token::identifier)
		{
			auto type = lex->identifiers[lex->find_identifier(lex->get_token_representation(token))];
			if (type.array_type == "int")
			{
				arithmetic_expr();
				vivodInt();
			}
			else if (type.array_type == "char")
			{
				token = lex->scan();
				in_cod << "     mov   di, 0 \n";
				token = lex->scan_v();
				if (token.type == Token::single_character_delimiter
					&& token.value == lex->contains("[", Token::single_character_delimiter))
				{
					token = lex->scan();
					arithmetic_expr();
					in_cod << "     pop   di \n";
					token = lex->scan();
					if (!(token.type == Token::single_character_delimiter
						&& token.value == lex->contains("]", Token::single_character_delimiter)))
					{
						error(59);
					}
				}
				vivodChar(type);
			}
			else if (type.array_type == "bool")
			{
				//arithmetic_expr();
				logical_expr();
				vivodBool(type);
			}
		}
		
		token = lex->scan_v();
		if (token.type == Token::single_character_delimiter
			&& token.value == lex->contains(",", Token::single_character_delimiter))
		{
			token = lex->scan();
			token = lex->scan_v();
		}
	}
	token = lex->scan();
	if (token.type == Token::single_character_delimiter
		&& token.value == lex->contains(")", Token::single_character_delimiter))
	{
		token = lex->scan();
	}
	else error(38);
	//in_cod << "lea   dx, clrf \n"
	//	<< "mov   ah, 9 \n"
	//	<< "int   21h \n";
}

//oper_while -> while '(' lg ')' oper
void synt::Syntactical_analyzer::oper_while()
{
	in_cod << "; WHILE() \n";
	Lexeme token = lex->scan_v();

	if (!(token.type == Token::keyword
		&& token.value == lex->contains("while", Token::keyword)))
	{
		error(39);
	}
	token = lex->scan();
	token = lex->scan_v();
	if (!(token.type == Token::single_character_delimiter
		&& token.value == lex->contains("(", Token::single_character_delimiter)))
	{
		error(48);
	}
	token = lex->scan();
	auto repeatLabel = next_label();
	in_cod << repeatLabel << ": \n";

	logical_expr();
	in_cod << "pop   ax \n"
		<< "cmp   ax, 0 \n";
	auto endLabel = next_label();
	in_cod << "jz " << endLabel << '\n';
	token = lex->scan_v();
	if (token.type == Token::single_character_delimiter
		&& token.value == lex->contains(")", Token::single_character_delimiter))
	{
		token = lex->scan();
	}
	else error(47);

	oper();
	in_cod << "jmp " << repeatLabel << '\n'
		<< endLabel << ": \n";
}

//oper_do -> 'do' oper 'loop' 'while' | 'until' (lg)
void synt::Syntactical_analyzer::oper_do()
{
	in_cod << "; DO() \n";
	Lexeme token = lex->scan();
	auto startLabel = next_label();
	auto endLabel = next_label();
	std::string type{};
	//in_cod << "mov eax, $x \n"
	in_cod << startLabel << ": \n";
	if (!(token.type == Token::keyword
		&& token.value == lex->contains("do", Token::keyword)))
	{
		error(52);
	}
	oper();
	token = lex->scan();
	if (token.type == Token::single_character_delimiter
		&& token.value == lex->contains(";", Token::single_character_delimiter))
	{
		token = lex->scan();
	}

	if (!(token.type == Token::keyword
		&& token.value == lex->contains("loop", Token::keyword)))
	{
		error(53);
	}
	token = lex->scan();
	if (token.type == Token::keyword
		&& token.value == lex->contains("while", Token::keyword))
	{
		type = "while";

	}
	else if (token.type == Token::keyword
		&& token.value == lex->contains("until", Token::keyword))
	{
		type = "until";

	}
	else error(54);

	token = lex->scan();
	if (!(token.type == Token::single_character_delimiter
		&& token.value == lex->contains("(", Token::single_character_delimiter)))
	{
		error(55);
	}
	logical_expr();
	in_cod << "pop   ax \n"
		<< "cmp   ax, 0 \n";
	if (type == "while")
	{
		in_cod << "jz " << endLabel << '\n';
	}
	else
	{
		in_cod << "jnz " << endLabel << '\n';
	}
	token = lex->scan();
	if (token.type == Token::single_character_delimiter
		&& token.value == lex->contains(")", Token::single_character_delimiter))
	{
		token = lex->scan();
	}
	else error(56);
	in_cod << "jmp " << startLabel << '\n'
		<< endLabel << ": \n";
}

// oper_if -> 'if' '(' lg ')' 'then' oper (else oper)
void synt::Syntactical_analyzer::oper_if()
{
	in_cod << "; IF() \n";
	Lexeme token = lex->scan_v();
	if (token.type == Token::keyword
		&& token.value == lex->contains("if", Token::keyword))
	{
		token = lex->scan();
		token = lex->scan_v();
	}
	else error(40);
	if (token.type == Token::single_character_delimiter
		&& token.value == lex->contains("(", Token::single_character_delimiter))
	{
		token = lex->scan();
		logical_expr();
		token = lex->scan_v();
		if (token.type == Token::single_character_delimiter
			&& token.value == lex->contains(")", Token::single_character_delimiter))
		{
			token = lex->scan();
		}
		else error(45);
	}
	else error(46);

	std::string thenLabel = next_label();
	std::string elseLabel = next_label();
	in_cod << "pop   ax \n"
		<< "cmp   ax, 0 \n"
		<< "jz " << thenLabel << '\n';

	token = lex->scan_v();
	if (token.type == Token::keyword
		&& token.value == lex->contains("then", Token::keyword))
	{
		token = lex->scan();
	}
	else error(41);
	in_cod << "; IF THEN \n";
	oper();
	in_cod << "jmp " << elseLabel << '\n'
		<< thenLabel << ": \n";
	token = lex->scan_v();
	if (token.type == Token::single_character_delimiter
		&& token.value == lex->contains(";", Token::single_character_delimiter))
	{
		token = lex->scan();
		token = lex->scan_v();
	}
	if (token.type == Token::keyword
		&& token.value == lex->contains("else", Token::keyword))
	{
		in_cod << "; IF ELSE \n";
		token = lex->scan();
		oper();
	}
	in_cod << elseLabel << ": \n";
}

//oper_assign -> (let) I ( '[' e ']' ) '=' e
void synt::Syntactical_analyzer::oper_assign()
{
	in_cod << "; LET() \n";
	Lexeme token = lex->scan_v();
	//std::cout << lex->get_token_representation(token) << '\n';
	if (token.type == Token::keyword
		&& token.value == lex->contains("let", Token::keyword))
	{
		token = lex->scan();
	}
	token = lex->scan_v();
	if (token.type == Token::identifier)
	{
		token = lex->scan();
	}
	else error(42);

	auto type = lex->identifiers[lex->find_identifier(lex->get_token_representation(token))];
	in_cod << "mov   di, 0 \n";


	token = lex->scan_v();
	if (token.type == Token::single_character_delimiter
		&& token.value == lex->contains("[", Token::single_character_delimiter))
	{
		token = lex->scan();
		arithmetic_expr();
		in_cod << "pop   di \n";
		token = lex->scan();
		if (!(token.type == Token::single_character_delimiter
			&& token.value == lex->contains("]", Token::single_character_delimiter)))
		{
			error(43);
		}
		token = lex->scan_v();
	}
	in_cod << "push  di \n";
	if (!(token.type == Token::single_character_delimiter
		&& token.value == lex->contains("=", Token::single_character_delimiter)))
		error(44);
	token = lex->scan();
	arithmetic_expr();
	in_cod << "pop   ax \n"
		<< "pop   di \n";
	if (!type.array_size.empty())
		in_cod << "shl   di, 1 \n";
	in_cod << "mov   " << type.name << "[di], ax \n";
}

void synt::Syntactical_analyzer::vvodInt(lex::Identifier type)
{
	in_cod << "; READ INTEGER \n";
	auto sLabel = next_label();
	auto lerror = next_label();
	auto lstart = next_label();
	auto lend = next_label();
	in_cod << lstart << ": \n";
	in_cod << "     mov   ah, 0Ah \n";
	in_cod << "     lea   dx, @buffer \n";
	in_cod << "     int   21h \n";
	in_cod << "     mov   ax, 0 \n";
	in_cod << "     mov   cx, 0 \n";
	in_cod << "     mov   cl, byte ptr[blength] \n";
	in_cod << "     mov   bx, cx \n";
	in_cod << sLabel << ": \n";
	in_cod << "     dec   bx \n";
	in_cod << "     mov   al, @buf[bx] \n";
	in_cod << "     cmp   al, \"9\" \n";
	in_cod << "     ja    " << lerror << " \n";
	in_cod << "     cmp   al, \"0\" \n";
	in_cod << "     jb    " << lerror << " \n";
	in_cod << "     loop  " << sLabel << " \n";
	sLabel = next_label();
	in_cod << "     mov   cl, byte ptr[blength] \n";
	in_cod << "     mov   di, 0 \n";
	in_cod << "     mov   ax, 0 \n";
	in_cod << sLabel << ": \n";
	in_cod << "     mov   bl, @buf[di] \n";
	in_cod << "     inc   di \n";
	in_cod << "     sub   bl, 30h \n";
	in_cod << "     add   ax, bx \n";
	in_cod << "     mov   si, ax \n";
	in_cod << "     mov   bx, 10 \n";
	in_cod << "     mul   bx \n";
	in_cod << "     loop  " << sLabel << " \n";
	in_cod << "     mov   ax, si \n";
	in_cod << "     pop   di \n";
	in_cod << "     shl   di, 1 \n";
	in_cod << "     mov  " << type.name << "[di],ax \n";
	in_cod << "     jmp   " << lend << " \n";
	in_cod << lerror << ": \n";
	in_cod << "     lea   dx, err_msg \n";
	in_cod << "     mov   ah, 9 \n";
	in_cod << "     int   21h \n";
	in_cod << "     jmp   " << lstart << " \n";
	in_cod << lend << ": \n";
}

void synt::Syntactical_analyzer::vvodBool(lex::Identifier type) {
	in_cod << "; READ BOOLEAN \n";
	std::string start = next_label();
	std::string l4 = next_label();
	std::string l5 = next_label();
	std::string le = next_label();
	std::string lt = next_label();
	std::string lf = next_label();
	std::string lend = next_label();
	std::string lerror = next_label();
	in_cod << start << ": \n";
	in_cod << "     mov   ah, 0Ah \n";
	in_cod << "     lea   dx, @buffer \n";
	in_cod << "     int   21h \n";

	in_cod << "     cmp   blength, 4 \n";
	in_cod << "     je    " << le << " \n";
	in_cod << "     cmp   blength, 5 \n";
	in_cod << "     je    " << le << " \n";
	in_cod << "     jmp   " << lerror << " \n";

	/*in_cod << l4 << ": \n";
	in_cod << "     lea   si, @true \n";
	in_cod << "     lea   di, @buf \n";
	in_cod << "     mov   cx, 4 \n";
	in_cod << "     repe  cmpsb \n";
	in_cod << "     jz  " << le << " \n";
	in_cod << "     jmp   " << lerror << " \n";

	in_cod << l5 << ": \n";
	in_cod << "     lea   si, @false \n";
	in_cod << "     lea   di, @buf \n";
	in_cod << "     mov   cx, 5 \n";
	in_cod << "     repe  cmpsb \n";
	in_cod << "     jz  " << le << " \n";
	in_cod << "     jmp   " << lerror << " \n";*/

	in_cod << le << ": \n";
	in_cod << "     cmp   @buf[0], \"t\" \n";
	in_cod << "     je    " << lt << " \n";
	in_cod << "     push  0 \n";
	in_cod << "     jmp   " << lend << " \n";
	in_cod << lt << ": \n";
	in_cod << "     push  1 \n";
	in_cod << "     jmp   " << lend << " \n";
	in_cod << lerror << ": \n";
	in_cod << "     lea   dx, err_msg \n";
	in_cod << "     mov   ah, 9 \n";
	in_cod << "     int   21h \n";
	in_cod << "     jmp   " << start << " \n";
	in_cod << lend << ": \n";
	in_cod << "     pop   ax \n";
	in_cod << "     pop   di \n";
	in_cod << "     mov  " << type.name << "[di], al \n";
}

void synt::Syntactical_analyzer::vvodChar(lex::Identifier type) {
	in_cod << "; READ CHAR \n";
	in_cod << "     mov   ah, 0Ah \n";
	in_cod << "     lea   dx, @buffer \n";
	in_cod << "     int   21h \n";
	in_cod << "     xor   dx, dx \n";
	in_cod << "     mov   dl, @buf[0] \n";
	in_cod << "     pop   di \n";
	in_cod << "     mov  " << type.name << "[di], dl \n";
}


void synt::Syntactical_analyzer::vivodBool(lex::Identifier type)
{
	std::string l0 = next_label();
	std::string l1 = next_label();
	in_cod << "; WRITE BOOLEAN \n";
	in_cod << "     pop   ax \n";
	in_cod << "     cmp   ax, 0 \n";
	in_cod << "     je " << l0 << " \n";
	in_cod << "     lea   dx, @@true \n";
	in_cod << "     jmp " << l1 << " \n";
	in_cod << l0 << ": \n";
	in_cod << "     lea   dx, @@false \n";
	in_cod << l1 << ": \n";
	in_cod << "     mov   ah, 9 \n";
	in_cod << "     int   21h \n";
}


void synt::Syntactical_analyzer::vivodChar(lex::Identifier type)
{
	in_cod << "; WRITE CHAR \n";
	in_cod << "     mov   ax, 0 \n";
	in_cod << "     mov   al, " << type.name << "[di] \n";
	in_cod << "     mov   dl, al \n";
	in_cod << "     mov   ah, 2 \n";
	in_cod << "     int   21h \n";

	in_cod << "lea   dx, clrf \n"
			<< "mov   ah, 9 \n"
			<< "int   21h \n";
}

void synt::Syntactical_analyzer::vivodInt()
{
	in_cod << "; WRITE ARIPHMETICS \n";
	auto l1 = next_label();
	auto l2 = next_label();
	in_cod << "pop   ax \n"
		<< "mov   bx, 10 \n"
		<< "mov   di, 0 \n"
		<< "mov   si, ax \n"
		<< "cmp   ax, 0 \n";
	auto sLabel = next_label();
	in_cod << "jns " << sLabel << '\n'
		<< "neg   si \n"
		<< "mov   ah, 2 \n"
		<< "mov   dl, \"-\" \n"
		<< "int   21h \n"
		<< "mov   ax, si \n"
		<< sLabel << ": \n"
		<< "mov   dx, 0 \n"
		<< "div   bx \n"
		<< "add   dl, 30h \n"
		<< "mov   output[di], dl \n"
		<< "inc   di \n"
		<< "cmp   al, 0 \n"
		<< "jnz " << sLabel << " \n"
		<< "mov   cx, di \n"
		<< "dec   di \n"
		<< "mov   ah, 2 \n";
	sLabel = next_label();
	in_cod << sLabel << ": \n"
		<< " mov   dl, output[di] \n"
		<< "dec   di \n"
		<< "int   21h \n"
		<< "loop " << sLabel << " \n";

	//space
	in_cod << "lea   dx, clrf \n"
		<< "mov   ah, 9 \n"
		<< "int   21h \n";
}

void synt::Syntactical_analyzer::vivodLiteral(std::string sqliteral)
{
	in_cod << "; WRITE LITERAL \n";
	auto text = next_label();
	in_dat << text << "p db \""
		<< sqliteral << "$\" \n";
	in_cod << "lea   dx, " << text << "p \n"
		<< "mov   ah, 9 \n"
		<< "int   21h \n";

	//space
	in_cod << "lea   dx, clrf \n"
		<< "mov   ah, 9 \n"
		<< "int   21h \n";
}

void synt::Syntactical_analyzer::error(int code)
{
	switch (code)
	{
	case 1: std::cout << "expected token with value 'or' in function 'lg' on line " << lex->get_str_number() << '\n'; break;
	case 2: std::cout << "expected token with value 'and' in function 'tl' on line " << lex->get_str_number() << '\n'; break;
	case 3: std::cout << "expected token with value ')' in function 'fl' on line " << lex->get_str_number() << '\n'; break;
	case 4: std::cout << "expected token with type 'keyword' or 'identifier' in function 'fl' on line " << lex->get_str_number() << '\n'; break;
	case 5: std::cout << "expected token with value ']' in function 'fl' on line " << lex->get_str_number() << '\n'; break;
	case 6: std::cout << "expected logical operator in function 'zn' on line " << lex->get_str_number() << '\n'; break;
	case 7: std::cout << "expected token is delimiter, but it's value not '-' or '+' in function 'e' on line " << lex->get_str_number() << '\n'; break;
	case 8: std::cout << "expected token is delimiter, but it's value not '/' or '*' in function 't' on line " << lex->get_str_number() << '\n'; break;
	case 9: std::cout << "expected token with value ')' in function 'f' on line " << lex->get_str_number() << '\n'; break;
	case 10: std::cout << "expected token with value '(' or with type 'number' in function 'f' on line " << lex->get_str_number() << '\n'; break;
	case 11: std::cout << "expected token with type 'keyword' and value 'Program' in function 'program' on line " << lex->get_str_number() << '\n'; break;
	case 12: std::cout << "expected token with type 'identifier' in function 'program' on line " << lex->get_str_number() << '\n'; break;
	case 13: std::cout << "expected token is delimiter with value '.' in function 'program' on line " << lex->get_str_number() << '\n'; break;
	case 14: std::cout << "expected token with type 'keyword' and value 'Var' in function 'define_block' on line " << lex->get_str_number() << '\n'; break;
	case 15: std::cout << "expected token with type 'identifier' in function 'define_block' on line " << lex->get_str_number() << '\n'; break;
	case 16: std::cout << "expected token with type 'delimiter' and value ',' in function 'define_block' on line " << lex->get_str_number() << '\n'; break;
	case 17: std::cout << "expected token with type 'identifier' in function 'define_block' on line " << lex->get_str_number() << '\n'; break;
	case 18: std::cout << "expected token with type 'delimiter' and value ':' in function 'define_block' on line " << lex->get_str_number() << '\n'; break;
	case 19: std::cout << "expected token with type 'delimiter' and value ';' in function 'define_block' on line " << lex->get_str_number() << '\n'; break;
	case 20: std::cout << "expected token with type 'delimiter' and value '[' in function 'type' on line " << lex->get_str_number() << '\n'; break;
	case 21: std::cout << "expected token with type 'number' in function 'type' on line " << lex->get_str_number() << '\n'; break;
	case 22: std::cout << "expected token with value ']' in function 'type' on line " << lex->get_str_number() << '\n'; break;
	case 23: std::cout << "expected token type 'keyword' in function 'type' on line " << lex->get_str_number() << '\n'; break;
	case 24: std::cout << "expected token representing type in function 'simple_type' on line " << lex->get_str_number() << '\n'; break;
	case 25: std::cout << "expected token with type 'keyword' in function 'simple_type' on line " << lex->get_str_number() << '\n'; break;
	case 26: std::cout << "expected token with type 'keyword' with value 'Begin' in function 'operator_block' on line " << lex->get_str_number() << '\n'; break;
	case 27: std::cout << "expected token with type 'keyword' and be valid in function 'operator' on line " << lex->get_str_number() << '\n'; break;
	case 28: std::cout << "expected token with type 'keyword' in function 'operator' on line " << lex->get_str_number() << '\n'; break;
	case 29: std::cout << "expected token with type 'keyword' with value 'read' in function 'oper_read' on line " << lex->get_str_number() << '\n'; break;
	case 30: std::cout << "expected token with type 'delimiter' and value '(' in function 'oper_read' on line " << lex->get_str_number() << '\n'; break;
	case 31: std::cout << "expected token with type 'identifier' in function 'oper_read' on line " << lex->get_str_number() << '\n'; break;
	case 32: std::cout << "expected token with type 'delimiter' and value ']' in function 'oper_read' on line " << lex->get_str_number() << '\n'; break;
	case 33: std::cout << "expected token with type 'delimiter' and value ')' in function 'oper_read' on line " << lex->get_str_number() << '\n'; break;
	case 34: std::cout << "expected token with type 'keyword' with value 'read' in function 'oper_write' on line " << lex->get_str_number() << '\n'; break;
	case 35: std::cout << "expected token with type 'delimiter' and value '(' in function 'oper_write' on line " << lex->get_str_number() << '\n'; break;
	case 36: std::cout << "expected token with type 'identifier' in function 'oper_write' on line " << lex->get_str_number() << '\n'; break;
	case 37: std::cout << "expected token with type 'delimiter' and value ']' in function 'oper_write' on line " << lex->get_str_number() << '\n'; break;
	case 38: std::cout << "expected token with type 'delimiter' and value ')' in function 'oper_write' on line " << lex->get_str_number() << '\n'; break;
	case 39: std::cout << "expected token with type 'keyword' with value 'while' in function 'oper_while' on line " << lex->get_str_number() << '\n'; break;
	case 40: std::cout << "expected token with type 'keyword' with value 'if' in function 'oper_if' on line " << lex->get_str_number() << '\n'; break;
	case 41: std::cout << "expected token with type 'keyword' with value 'then' in function 'oper_if' on line " << lex->get_str_number() << '\n'; break;
	case 42: std::cout << "expected token with type 'identifier' in function 'oper_assign' on line " << lex->get_str_number() << '\n'; break;
	case 43: std::cout << "expected token with type 'delimiter' and value ']' in function 'oper_assign' on line " << lex->get_str_number() << '\n'; break;
	case 44: std::cout << "expected token with type 'delimiter' and value '=' in function 'oper_assign' on line " << lex->get_str_number() << '\n'; break;
	case 45: std::cout << "expected token with type 'delimiter' and value ')' in function 'oper_if' on line " << lex->get_str_number() << '\n'; break;
	case 46: std::cout << "expected token with type 'delimiter' and value '(' in function 'oper_if' on line " << lex->get_str_number() << '\n'; break;
	case 47: std::cout << "expected token with type 'delimiter' and value ')' in function 'oper_while' on line " << lex->get_str_number() << '\n'; break;
	case 48: std::cout << "expected token with type 'delimiter' and value '(' in function 'oper_while' on line " << lex->get_str_number() << '\n'; break;
	case 49: std::cout << "expected token with value ']' in function 'f' on line " << lex->get_str_number() << '\n'; break;
	case 50: std::cout << "expected token with value '[' in function 'f' on line " << lex->get_str_number() << '\n'; break;
	case 51: std::cout << "expected token with value ']' in function 'fl' on line " << lex->get_str_number() << '\n'; break;
	case 52: std::cout << "expected token with value 'for' in function 'oper_for' on line " << lex->get_str_number() << '\n'; break;
	case 53: std::cout << "expected token with value 'let' in function 'oper_for' on line " << lex->get_str_number() << '\n'; break;
	case 54: std::cout << "expected token with value 'to' in function 'oper_for' on line " << lex->get_str_number() << '\n'; break;
	case 55: std::cout << "expected token with value 'next' in function 'oper_for' on line " << lex->get_str_number() << '\n'; break;
	case 56: std::cout << "Variable name not correct in function 'oper_do' on line " << lex->get_str_number() << '\n'; break;
	case 57: std::cout << "expected token with type 'delimiter' and value '(' in function 'oper_for' on line " << lex->get_str_number() << '\n'; break;
	case 58: std::cout << "expected token with type 'delimiter' and value ')' in function 'oper_for' on line " << lex->get_str_number() << '\n'; break;
	case 59: std::cout << "expected token with value ']' in function 'oper_write' on line " << lex->get_str_number() << '\n'; break;
	}
	system("pause");
	std::exit(code);
}
