program recarr;
type
  subrec = record a: array[-1..1] of integer; i: integer; end;
  rec = record i: integer; r: subrec; end;
var
  gr: array [0..1] of rec;
procedure test;
var
  r: array [0..1] of rec;
  i,j,k: integer;
  sum: longint;
{test}
begin
  sum := 0;
  for i:=0 to 1 do begin
    read(gr[i].i, r[i].i, gr[i].r.i, r[i].r.i);
    for j:=-1 to 1 do begin
      read(gr[i].r.a[j], r[i].r.a[j]);
      sum := sum + gr[i].r.a[j] * r[i].r.a[j];
    end;
    writeln(gr[i].i + r[i].i + gr[i].r.i + r[i].r.i);
    readln;
  end;
  writeln(sum);
end;
{main}
begin
  test;
end.

