program a;
CONST
    b = 1;
    c = 2.2;
VAR
    f : longint;
    g : real;
    h : boolean;
    j : longint;
    x : array [0..3] of integer;

begin
  x[2] := 2;
  f := x[1];
  if b = 1 then
  begin
      f := 6;
  end
  else begin
      f := 9;
  end;
  writeln(x[2]);
  writeln(x[1]);
end.
