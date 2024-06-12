Submit a solution for tech02-III-0-posix/syscalls/hello-world

Full score:	20

Run penalty:	5

Time limit:	1 s

Real time limit:	5 s

Memory limit:	16M

Open date:	2022/10/05 09:00:00

Problem tech02-III-0: posix/syscalls/hello-world

Реализуйте на языке Си программу, которая выводит "Hello, World!".

Использование стандартной библиотеки Си запрещено, единственная доступная функция - это syscall(2).

Точка входа в программу - функция _start.

Для использования syscall можно включить в текст программы следующее объявление:
```
long syscall(long number, ...);
```
Для локального тестирования можно взять реализацию syscall здесь: [syscall.S](syscall.S).