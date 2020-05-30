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
{test}
begin
  for i:=ED downto BEG do
  begin
    arr[i] := ED - i;
    garr[i] := sqr(i);
    writeln('arr[', i, ']: ', arr[i], '; garr[', i, ']: ', garr[i]);
  end;
end;
{main}
begin
  test;
end.
