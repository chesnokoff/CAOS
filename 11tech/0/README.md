Submit a solution for tech09-0-posix/signals/count-sigint

Full score:	20

Run penalty:	5

Time limit:	1 s

Real time limit:	5 s

Memory limit:	64M

Open date:	2022/11/23 09:00:00

Problem tech09-0: posix/signals/count-sigint

Программа при запуске сообщает на стандартный поток вывода свой PID, выталкивает буфер вывода с помощью fflush, после чего начинает обрабатывать поступающие сигналы.

При поступлении сигнала SIGTERM необходимо вывести на стандартный поток вывода целое число: количество ранее поступивших сигналов SIGINT и завершить свою работу.

Семантика повединия сигналов (Sys-V или BSD) считается не определенной.