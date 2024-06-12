Submit a solution for tech08-1-posix/pipe/connect-2-processes

Full score:	100

Run penalty:	5

Time limit:	1 s

Real time limit:	5 s

Memory limit:	64M

Open date:	2022/11/16 09:00:00

Problem tech08-1: posix/pipe/connect-2-processes

Программе передаётся два аргумента: CMD1 и CMD2. Необходимо запустить два процесса, выполняющих эти команды, и перенаправить стандартный поток вывода CMD1 на стандартный поток ввода CMD2.

В командной строке это эквивалентно CMD1 | CMD2.

Родительский процесс должен завершаться самым последним!