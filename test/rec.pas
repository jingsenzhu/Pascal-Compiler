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
procedure printRec(re: record a: integer; c: string; e: real; end);
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
function sss(_s: string):string;
var
  i: integer;
begin 
  sss := _s; 
  for i := length(sss) - 1 downto 0 do
    sss[i] := succ(sss[i]);
end;
procedure test(re: rec);
begin
  readln(re.ia, re.ie);
  readln(re.ic);
  re.ia := re.ia + NEGONE;
  re.ic := concat(re.ic, 'tql');
  re.ie := sqrt(re.ie);
  t(re.ic);
  writeln(re.ic);
  printRec(re);
  writeln(sss(re.ic));
end;
begin test(r); end.
