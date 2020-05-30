program strtest;
const
  consst = 'wow';
var
  gst: string;

procedure test;
type
  r = record s: string; end;
var
  i: longint;
  rst: r;
  st: string;
  ch: char;
{test}
begin
  readln(gst);readln(rst.s);readln(ch,st);
  writeln(gst);writeln(rst.s);writeln(ch);writeln(st);
  read(gst);read(rst.s);read(st);
  writeln(gst);writeln(rst.s);writeln(st);
  gst := consst;
  gst := rst.s;
  gst := st;
  gst[0] := '1';
  gst := '123';
  rst.s := consst;
  rst.s := gst;
  rst.s := st;
  rst.s := '124';
  st := consst;
  st := rst.s;
  st := gst;
  st[0] := '1';
  st := '125';
  i := length(gst);
  i := length(rst.s);
  i := length(st);
  writeln(sqr(val(rst.s)));
  writeln(sqr(val(gst)));
  writeln(sqr(val(st)));
  rst.s := concat(rst.s,gst,st,consst,'123');
  gst := concat(rst.s,gst,st,consst,'124');
  st := concat(st,rst.s,gst,consst,'125');
  writeln(rst.s);
  writeln(gst);
  writeln(st);
  writeln(gst[1],st[1]);
end;
{main}
begin
  test;
end.
