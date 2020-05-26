program a;
CONST
    b = 1;
    c = 2.2;
VAR
    f : longint;
    g : real;
    h : boolean;
    j : longint;
    s : string;
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
  readln(s);
  writeln(x[2]);
  writeln(x[1]);
  writeln(s);
  s := 'Hello world';
  writeln(s);
  s[0] := 'h';
  writeln(s);
end.
