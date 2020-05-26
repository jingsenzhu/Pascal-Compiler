program fib;
const
    hello = '---hello pascal---';

var
    a, d, e, t : longint;
    b, c : Real;

function fib(n : longint): longint;
begin
    if n <= 1 then
    begin
        fib := 1;
    end
    else if n = 1 then
    begin
        fib := 1;
    end
    else begin
        fib := fib(n - 1) + fib(n - 2);
    end;
end;
begin
    writeln(hello);
    a := 1;
    d := 1;
    e := 1;
    writeln(d);
    writeln(e);
    while a < 10 do begin
        t := d + e;
        d := e;
        e := t;
        writeln(e);
	writeln(fib(a+1));
        a := a + 1;
    end;
end.
