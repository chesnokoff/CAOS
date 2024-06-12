Submit a solution for tech04-3-posix/filesystem/manage-symlinks

Full score:	100

Run penalty:	5

Time limit:	1 s

Real time limit:	5 s

Memory limit:	16M

Open date:	2022/10/19 09:00:00

Problem tech04-3: posix/filesystem/manage-symlinks

Программе на стандартном потоке ввода передаётся список имён файлов.

Если файл является символической ссылкой, то нужно вывести абсолютное имя того файла, на который эта ссылка указывает.

Если файл является регулярным, то необходимо создать символическую ссылку в текущем каталоге, приписав в названии файла префикс link_to_.