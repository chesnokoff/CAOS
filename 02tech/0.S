.global f
.text

f:
  // x0 - A, x1 - B, x2 - C, x3 - x
  // Return x0 - Ax^2 + Bx + C
  madd x0, x0, x3, x1 // x0 <- x0 * x3 + x1 Ax+B
  madd x0, x0, x3, x2 // x0 <- x0 * x3 + x2 (Ax+B)*x+C=Ax^2+Bx+C
  ret
