program prog;
const
  st = 'wow';
{type
  intarr = array [0..10] of integer;}
var
  x: integer;
  ar: array [0..10] of integer;

function f(x: integer): integer;
  begin
    if x = 0 then f := 0
    else f := f(x-1) + sqr(x);
    ar[x] := f;
  end;

procedure g(x: integer);
  const
    s = '233333';
  var
    i: integer;
  procedure h;
    begin 
      writeln(s); 
    end;
  begin
    for i := 1 to x do begin
        writeln(f(i));
        h;
    end;
  end;

procedure printAr;
  var
    i: longint;
  begin
    for i := 0 to 9 do write(ar[i], ' ');
    writeln(ar[10]);
  end;

procedure pr;
  const
    s = 'kono dio da';
  begin writeln(s); end;

begin
  read(x);
  if x > 10 then begin
    writeln(x, ' is too big!');
    x := 10;
  end;
  g(x);
  printAr;
  writeln(st);
  pr;
end.
