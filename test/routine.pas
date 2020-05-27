program prog;
const
  st = 'wow';
  ZERO = 0;
  ONE = 1;
type
  long = longint;
  intarr = array [0..10] of integer;
var
  x: integer;
  ar: intarr;

function f(x: integer): integer;
  begin
    if x = ZERO then f := 0
    else f := f(x-ONE) + sqr(x);
    ar[x] := f;
  end;

procedure g(x: integer);
  const
    s = 'wow';
    cg = 233333;
  var
    i: integer;
  procedure h;
  const
    cs = 'wowow';
    ch = 666;
    begin
      writeln(concat(s, ' ', ch, ' ', cg, ' ', cs)); 
    end;
  begin
    for i := ONE to x do begin
        writeln(f(i));
        h;
    end;
  end;

procedure printAr;
  var
    i: long;
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
