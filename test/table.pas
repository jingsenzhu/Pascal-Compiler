program main;
CONST
    ONE = 1;
    ONEPONE = 1.5;
type
    int = integer;
    arr = array [0..3] of int;
    rec = record a: integer; c: string; e: real; end;
VAR
    a : integer;
    b : real;
    c : boolean;
    d : longint;
    e : string;
    f : char;

function f1(k: arr): rec;
CONST
    t = 't';
    tr = TRUE;
type
    arr2 = array[-2..2] of string;
    rec2 = record a: arr2; end;
    arr3 = array[-2..2] of rec2;
    rec3 = record a: rec2; end;
var
    a1: rec2;
    a2: arr3;

    procedure f2(k: boolean);
    var
        r1: rec;
        r2: arr;
    begin
    end;
    
begin
end;


begin
end.
