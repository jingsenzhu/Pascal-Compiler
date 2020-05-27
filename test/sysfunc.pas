program sysfunc;
const
  ZERO = 0;
  TWO = 2;
  NEGONE = -1;
  NEGONER = -1.0;
  Z = 'Z';
  s = 'wow';
var
  a,b,c: integer;
  ch: char;
  st: string;
begin
  writeln('Test writeln');
  write('Test write');
  writeln;
  write('Readln st: ');
  readln(st);
  write('Read a,b,c: ');
  read(a,b,c);
  writeln('Test writeln(a,b,c,st): ');
  writeln(a, ' ', b, ' ', c, ' ', st);
  writeln('Test concat(a,b,c,st): ', concat(a,b,c,st));
  writeln('Test string assignment (strcpy)');
  st := s;
  writeln(st);
  st := concat(a,b,c,st);
  writeln(st);
  writeln('Test abs(-1): ', abs(NEGONE));
  writeln('Test abs(-1.0): ', abs(NEGONER));
  writeln('Test sqrt(2): ', sqrt(TWO));
  writeln('Test sqr(2): ', sqr(TWO));
  writeln('Test chr(65): ', chr(65));
  writeln('Test ord(A): ', ord('A'));
  writeln('Test succ(A): ', succ('A'));
  writeln('Test pred(Z): ', pred(Z));
  writeln('End test');
end.
