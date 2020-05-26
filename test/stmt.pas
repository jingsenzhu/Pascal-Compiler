program stmt;
const
    a = 1;
    str = 'fuck';
var
    b, c, d : integer;

begin
    d := 1;
    if a = 1 then
    begin
        for d := 1 to 10 do begin
            b := 1;
            while b < 10 do begin
                b := b + 1;
                c := 1;
                repeat
                    c := c + 1;
                until c >= 10;
            end;
        end;
    end
    else begin
        writeln(str);
    end;
    writeln(a, ' ', b, ' ', c, ' ', d);
    writeln(str);
end.
