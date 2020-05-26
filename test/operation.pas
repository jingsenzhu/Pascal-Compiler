program prog;
var
  a, b: integer;
  c, d: real;
  ch: char;

begin
  { Integer operations }
  readln(a, b);
  writeln('-a = ', -a, ', -b = ', -b);
  writeln('a  +  b = ', a + b);
  writeln('a  -  b = ', a - b);
  writeln('a  *  b = ', a * b);
  writeln('a  /  b = ', a / b);
  writeln('a div b = ', a div b);
  writeln('a mod b = ', a mod b);
  writeln('a and b = ', a and b);
  writeln('a or  b = ', a or b);
  writeln('a xor b = ', a xor b);
  writeln('sqrt(abs(a)) = ', sqrt(abs(a)));
  { Floating-point operations }
  readln(c, d);
  writeln('c + d = ', c + d);
  writeln('c - d = ', c - d);
  writeln('c * d = ', c * d);
  writeln('c / d = ', c / d);
  writeln('sqrt(abs(c)) = ', sqrt(abs(c)));
  { Implicit conversion }
  writeln('a + c = ', a + c);
  writeln('b * d = ', b * d);
  { Char functions }
  read(ch);
  writeln('ord(ch)  = ', ord(ch));
  writeln('pred(ch) = ', pred(ch));
  writeln('succ(ch) = ', succ(ch));
end.
