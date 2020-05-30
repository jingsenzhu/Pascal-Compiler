program arrstr;
var
  garr: array[-1..0] of string;
procedure test;
var
  i: integer;
  arr: array[-1..0] of string;
begin
  for i:=-1 to 0 do begin
    readln(arr[i]);
    writeln(length(arr[i]));
    writeln(val(arr[i]));
    writeln(concat(arr[i], 'tql'));
    writeln(str(val(arr[i]) - 0.5));
    readln(garr[i]);
    writeln(length(garr[i]));
    writeln(val(garr[i]));
    writeln(concat(garr[i], 'tql'));
    writeln(str(val(garr[i]) - 0.5));
    arr[i] := 'wowow';
    writeln(arr[i]);
    garr[i] := 'momom';
    writeln(garr[i]);
  end;
end;
begin
  test;
end.
