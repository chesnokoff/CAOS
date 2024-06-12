Submit a solution for tech02-I-2-asm-arm/basics/eqsolution-64

Full score:	100

Run penalty:	5

Time limit:	1 s

Real time limit:	5 s

Memory limit:	1G

Open date:	2022/09/21 09:00:00

Problem tech02-I-2: asm-arm/basics/eqsolution-64

Реализуйте на языке ассемблера armv8 (AArch64) функцию solve, с прототипом
```
int solve(int64_t A, int B, int C, int D);
```

которая находит минимальное значение x ∈[0,254], которое является одним из корней уравнения: Ax3+Bx2+Cx+D=0.

Все аргументы и значение выражения представимы 32-разрядными знаковыми целыми числами.

Гарантируется, что на тестовых данных существует целочисленное решение.