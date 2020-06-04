program main;
VAR
  i: integer;
begin
  { const }
  i := 3 + 4 * 5 - 12 div 6;

  { if statement }
  if 1 < 2 then writeln('x < y')
  else writeln('x >= y');

  { repeat statement }
  i := 0;
  repeat
    write(i);
    i := i + 1;
  until 2 > 1;
  writeln;

  { while statement }
  while 2 < 1 do begin
    i := i - 1;
    write(i);
  end;
  writeln;
end.
