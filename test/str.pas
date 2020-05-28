program prog;
const
  s = 'wow';
type
  int = integer;
var
  x, y, i: int;
  st, sr: string;
function succ10(_s: string): string;
const
  TEN = 10;
var
  i: int;
begin
  for i := 0 to TEN do begin
    _s[i] := succ(_s[i]);
  end;
  succ10 := _s;
end;
{ main }
begin
  readln(x, y, i);
  readln(st);
  sr := concat(x,y,chr(i),st,s);
  writeln(sr);
  sr := succ10(st);
  writeln(st);
  writeln(sr);
end.
