Submit a solution for tech02-I-1-asm-arm/basics/memaccess-64

Full score:	20

Run penalty:	5

Time limit:	1 s

Real time limit:	5 s

Memory limit:	1G

Open date:	2022/09/21 09:00:00

Problem tech02-I-1: asm-arm/basics/memaccess-64

Реализуйте на языке ассемблера armv8 (AArch64) функцию с сигнатурой:
```
int sum(int x0, size_t N, int *X)
```
Функция должна вычислять значение x0+∑x_i, где 0≤i<N

Обратите внимание на то, что тип int имеет размер 32 бит, а тип size_t - 64 бит.