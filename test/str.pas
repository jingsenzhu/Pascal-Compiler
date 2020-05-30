program prog;
const
  s = 'wow';
type
  int = integer;
var
  x, y, i: int;
  st, sr, s1: string;
function succstr(_s: string): string;
var
  i: int;
  n: int;
begin
  n := length(_s) - 1;
  succstr := _s;
  for i := 0 to n do begin
    succstr[i] := succ(_s[i]);
  end;
end;
function test(ss: string): string;
begin
  readln(x, y, i);
  readln(st);
  sr := concat(x,y,chr(i + 64),st,s);
  writeln(sr);
  sr := s;
  writeln(st);
  writeln(sr);
  st := sr;
  st := s;
  writeln(length(sr));
  writeln(length(succstr(sr)));
  writeln(length('123'));
  readln(ss);
  test := str(length(ss));
  ss[0] := chr(val(test) + 64);
  writeln(ss);
  test := succstr(ss);
end;
{main}
begin
  writeln(test(s1));
end.
