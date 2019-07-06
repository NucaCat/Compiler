# Compiler
Задание на курсовую работу РГУПС
Программа написанная на языке с++ позволяет перевести код с языка, подобного Паскалю в ассемблерный код.
Лексический анализатор представлен классом Lexical_analyzer, а сам лексический анализ проводится с использованием конечного автомата
Синтаксический анализатор представлен классом Syntactical_analyzer, а сам синтаксический анализ проводится с использованием метода
рекурсивного спуска. Рядом с каждой функцией считывающей определенный оператор, написана грамматика данного оператора.
Функции-помощники находятся в файле toolkit и находятся в аналогичном пространстве имен.
Входным файлом является файл program.txt с текстом произваольной программы на языке подобном Паскалю.
Выходными файлами становятся cod.asm и dat.asm содержащие кодовый сегмент и сегмент данных.
