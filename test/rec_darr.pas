program recarr;
type
  iarr = array [-1..0] of integer;
  rec = record s: string; ar: array [0..1] of iarr; end;
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
    read(gr[i].s, r[i].s);
    for j:=0 to 1 do begin
      for k:=-1 to 0 do begin
        read(gr[i].ar[j][k], r[i].ar[j][k]);
        sum := sum + gr[i].ar[j][k] + r[i].ar[j][k];
      end;
    end;
    writeln(length(gr[i].s) * length(r[i].s), gr[i].s[0]);
  end;
  writeln(sum);
end;
{main}
begin
  test;
end.

