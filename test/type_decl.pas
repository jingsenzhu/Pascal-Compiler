program typedeclcheck;
CONST
    a = 1;
    b = 2.2;
    c = '33333';
    d = false;

TYPE
    z = integer;
    y = record
            a, b : integer;
            c, d, e : real;
            f, g : boolean;
        end;

VAR 
    e : integer;
    f : boolean;
    g : longint;
    h : string;
    i : real;
    j : array [-1..20] of real;
    k : array [-1..20] of integer;
    l : array [-23..20] of longint;
    m : array [-1..20] of boolean;
    n : z;
    o : y;

procedure proc1;
const
    pa = 1;
    pb = 3.3;
    pc = '2222';
    pd = true;

type
    pz = boolean;
    py = record
            a, b : integer;
            c, d, e : real;
            f, g : boolean;
        end;

var
    pe : integer;
    pf : z;
    pg : y;

function func2(n : boolean) : boolean;

procedure proc3;
begin
    writeln('pass.');
end;

begin
    proc3();
end;

begin
    func2(true);
end;

function func1(n : real): longint;
begin
    writeln('pass.');
end;

begin
    proc1();
end.