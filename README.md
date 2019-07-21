# Compiler
Задание на курсовую работу РГУПС
Программа написанная на языке с++ позволяет перевести код с языка, подобного Паскалю в ассемблерный код.
Лексический анализатор представлен классом Lexical_analyzer, а сам лексический анализ проводится с использованием конечного автомата
Синтаксический анализатор представлен классом Syntactical_analyzer, а сам синтаксический анализ проводится с использованием метода
рекурсивного спуска. Рядом с каждой функцией считывающей определенный оператор, написана грамматика данного оператора.
Функции-помощники находятся в файле toolkit и находятся в аналогичном пространстве имен.
Входным файлом является файл program.txt с текстом произваольной программы на языке подобном Паскалю.
Выходными файлами становятся cod.asm и dat.asm содержащие кодовый сегмент и сегмент данных.

ENG:
c++ program that can translate text written in Pascal-like to assembly. program consists of lexical analyzer and syntactical analyzer(parser). Lexical analyzer uses state machine to find lexemes, and syntactical analyzer uses recursive descent method to match tokens with grammar. The grammar can be found in comments next to functions in syntactical_analyzer.cpp.
How does it work.
You write a program in Pascal-like according to grammar in program.txt, run compiler and you'll get 2 files: cod.asm and dat.asm with code segment and data segment respectively.
