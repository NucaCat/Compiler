#include "Syntactical_analyzer.h"

int main()
{
	setlocale(LC_ALL, "Russian");
	synt::Syntactical_analyzer synt("program.txt");
	synt.analysis();
	std::cout << "���������� ������ �������\n"
		<< "������������� ����� cod.asm � dat.asm\n";
	//synt._l();
	//synt.formatted_debug();
	system("pause");
	return 0;
}