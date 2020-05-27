program prog;
const
  ZERO = 0;
  ONE = 1;
  TWO = 2;
type
  int = integer;
var
  x, y, i: int;

begin
  readln(x, y);

  { if statement }
  if x < y then writeln('x < y')
  else if x > y then writeln('x > y')
  else writeln('x = y');

  { case statement }
  case x of
    0: x := ZERO;
    1: x := ONE;
    TWO: x := TWO;
  end;
  writeln;

  { repeat statement }
  i := 0;
  repeat
    write(i);
    i := i + 1;
  until i = 10;
  writeln;

  { while statement }
  while i > 0 do begin
    i := i - 1;
    write(i);
  end;
  writeln;
end.
