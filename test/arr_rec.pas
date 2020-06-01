program arrtest;
const
  BEG = -2;
  ED = 2;
type
  rec = record i: integer; c:char; s:string; end;
var
  garr: array [BEG..ED] of longint;
  grarr: array [BEG..ED] of rec;

procedure test;
var
  i: longint;
  arr: array [BEG..ED] of longint;
  st: string;
  rarr: array[BEG..ED] of rec;
procedure printRec(re: rec);
{printRec}
begin
  writeln('  i: ', re.i);
  writeln('  c: ', re.c);
  writeln('  s: ', re.s); 
end;
{test}
begin
  for i:=ED downto BEG do
  begin
    arr[i] := ED - i;
    garr[i] := sqr(i);
    readln(grarr[i].i, grarr[i].c);
    readln(grarr[i].s);
    writeln('arr[', i, ']: ', arr[i], '; garr[', i, ']: ', garr[i]);
    writeln('grarr[', i, ']:');
    printRec(grarr[i]);
  end;
end;
{main}
begin
  test;
end.
