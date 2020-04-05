function sxhs
for x=100:999
    a=rem(x,10);
    b=rem(fix(x/10),10);
    c=rem(fix(x/100),10);
    n=a.^3+b.^3+c.^3;
    % disp([a,b,c,n])
    if (x==n)
        disp(x)
    end
end
return;