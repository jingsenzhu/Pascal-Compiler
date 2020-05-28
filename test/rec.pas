program sysfunc;
const
  NEGONE = -1;
type
  chars = string;
  rec = record
    ia: integer;
    ic: string;
    ie: real;
  end;
var
  i: integer;
  s: string;
  r: rec;
  {r: record
    ia: integer;
    ic: string;
    ie: real;
  end;}
procedure printRec(re: record a: integer; c: string; e: real; end);
//procedure printRec(re: rec);
const
  IA = 'ia: ';
  IC = 'ic: ';
  IE = 'ie: ';
begin
  writeln(IA, re.a);
  writeln(IC, re.c);
  writeln(IE, re.e);
end;
procedure t(st: chars);
begin
  st := concat(st, 'tql');
  writeln(st);
end;
function sss(_s: chars):chars;
begin sss := _s; sss[0] := succ(sss[0]);  end;
begin
  readln(r.ia, r.ie);
  readln(r.ic);
  r.ia := r.ia + NEGONE;
  //r.ic := concat(r.ic, 'tql');
  r.ie := sqrt(r.ie);
  (*writeln(s);
  readln(s);
  t(s);
  writeln(s);*)
  t(r.ic);
  writeln(r.ic);
  printRec(r);
  s := r.ic;
  //s := sss(s);
  //s := sss(r.ic);
  writeln(sss(r.ic));
end.
