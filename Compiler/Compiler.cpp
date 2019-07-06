#include "Syntactical_analyzer.h"

int main()
{
	setlocale(LC_ALL, "Russian");
	synt::Syntactical_analyzer synt("program.txt");
	synt.analysis();
	std::cout << "Компиляция прошла успешно\n"
		<< "Сгенерированы файлы cod.asm и dat.asm\n";
	//synt._l();
	//synt.formatted_debug();
	system("pause");
	return 0;
}