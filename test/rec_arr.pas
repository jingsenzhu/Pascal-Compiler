program recarr;
type
  rec = record s: string; ar: array [-1..1] of integer; end;
var
  gr: array [0..1] of rec;
procedure test;
var
  r: array [0..1] of rec;
  i,j: integer;
  sum: longint;
{test}
begin
  sum := 0;
  for i:=0 to 1 do begin
    read(gr[i].s, r[i].s);
    for j:=-1 to 1 do begin
      read(gr[i].ar[j], r[i].ar[j]);
      sum := sum + sqr(gr[i].ar[j]) + r[i].ar[j];
    end;
    writeln(length(gr[i].s) * length(r[i].s));
  end;
  writeln(sum);
end;
{main}
begin
  test;
end.

