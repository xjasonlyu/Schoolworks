function fee=weight2fee(weight)
fee=0;
if (weight<=2)
    fee=10;
elseif (weight>2 & weight<=70)
    fee1=10;
    fee=fee1+(weight-2)*3.75;
elseif (weight>70 & weight<=100)
    fee1=10+(70-2)*3.75;
    fee=fee1+(weight-70)*1;
end
return;