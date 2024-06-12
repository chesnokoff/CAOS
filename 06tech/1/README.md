Submit a solution for tech04-1-posix/filesystem/find-invalid-executables

Full score:	100

Run penalty:	5

Time limit:	1 s

Real time limit:	5 s

Memory limit:	16M

Open date:	2022/10/19 09:00:00

Problem tech04-1: posix/filesystem/find-invalid-executables

Программе на стандартном потоке ввода передаётся список имён файлов.

Необходимо вывести на стандартный поток вывода имена "неправильных" выполняемых файлов, то есть файлов, которые отмечены как исполняемые, но при этом они заведомо не могут быть выполнены в системе (не начинаются с корректной строки #!, содержащей запускаемый интерпретатор, и не являются ELF-файлами).

