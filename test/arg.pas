program arg;
const
  ONE = 1;
  CH = 'c';
var
  i: integer;
  r: real;
procedure test1(ti: integer);
begin writeln(ti); end;
procedure test2(tr: real);
begin writeln(sqrt(tr)); end;
procedure test3(tc: char);
begin writeln(tc); end;
{main}
begin
  i := 2;
  r := 1.44;
  test1(r);
  test2(r);
  test1(i);
  test2(i);
  test1(ONE);
  test2(ONE);
  test3(CH);
end.
