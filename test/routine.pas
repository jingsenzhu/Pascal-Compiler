program prog;
var
  x: integer;

function f(x: integer): integer;
  begin
    if x = 0 then f := 0
    else f := f(x-1) + x;
  end;

procedure g(x: integer);
  var
    i: integer;
  procedure h;
  begin writeln(sqr(i)); end;
  begin
    for i := 1 to x do begin
        writeln(f(i));
        h;
    end;
  end;

begin
  read(x);
  g(x);
end.
