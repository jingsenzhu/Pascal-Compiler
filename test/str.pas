program prog;
const
  s = 'wow';
type
  int = integer;
var
  x, y, i: int;
  st, sr: string;

begin
  readln(x, y, i, st);
  writeln(st);
  sr := concat(x,y,i,st,s);
  writeln(sr);
  st := s;
  writeln(st);
end.
