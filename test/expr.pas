program exprcheck;
CONST
    y = 33;
    a = 1;
    b = 2.2;
    c = 'string';
    d = false;

TYPE
    z = record
            a : integer;
            b : integer;
        end;

VAR
    e : integer;
    f : real;
    g : longint;
    h : string;
    i : boolean;
    j : array [-2..2] of integer;
    k : z;

function test : integer;
begin
    test := 3;
end;

begin
    e := a + y;
    g := a - y;
    f := a * b;
    f := a / b;
    i := a = b;
    i := a <> b;
    i := a > b;
    i := a < b;
    h := 'dydxh';
    f := 2.2;
    g := 3;
    i := false;
    e := a and y;
    e := a or y;
    e := a xor y;
    e := a div g;
    e := a mod g;
    e := not g;
    j[-2] := -2;
    j[-1] := -1;
    j[0] := 0;
    j[1] := 1;
    j[2] := 2;

    k.a := (j[-2] + a - g) * 4 div 3 * 3 + test();

end.