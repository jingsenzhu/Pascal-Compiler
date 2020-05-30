program arrtest;
const
  BEG = -2;
  ED = 2;
var
  garr: array [BEG..ED] of longint;

procedure test;
var
  i: longint;
  arr: array [BEG..ED] of longint;
  st: string;
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
