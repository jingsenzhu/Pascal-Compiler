program mat;
type
  arr = array [-1..1] of integer;
var
  ma, mb: array [-1..1] of arr;
  i, j: integer;
procedure mul(a: array[-1..1] of arr; b: array[-1..1] of arr);
var
  i, j, k: integer;
  c: array[-1..1] of arr;
{mul}
  begin
    for i:=-1 to 1 do for j:=-1 to 1 do c[i][j] := 0;
    for i:=-1 to 1 do
      for j:=-1 to 1 do
        for k:=-1 to 1 do
          c[i][j] := c[i][j] + a[i][k]*b[k][j];
    for i:=-1 to 1 do begin
      for j:=-1 to 1 do write(c[i][j], ' ');
      writeln;
    end;
  end;
{main}
begin
  for i:=-1 to 1 do begin
    for j:=-1 to 1 do read(ma[i][j]);
    readln;
  end;
  for i:=-1 to 1 do begin
    for j:=-1 to 1 do read(mb[i][j]);
    readln;
  end;
  mul(ma,mb);
end.
