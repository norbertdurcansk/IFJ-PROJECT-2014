var

a:integer;

vysl :integer;

begin

write('Zadejtecisloprovypocetfaktorialu:');
readln(a);


if a < 0 then

begin

write('Faktorialnelzespocitat'#10'')

end

else


begin

vysl := 1;
while

a > 0
do
begin
vysl := vysl*a;
a := a - 1

end;
write('Vysledekje:'vysl,''#10'')

end

end.