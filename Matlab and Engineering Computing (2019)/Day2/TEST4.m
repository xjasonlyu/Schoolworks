clear all

% create 100*100 matrix
rows=100;
cols=100;
A=round(100*rand(rows,cols));
disp(A)

for x=0:10:90
    if (x==0)
        % 0-10
        s=find(A>=0 & A<=10);
        fprintf("%.2f%%\n", length(s)/(rows*cols)*100);
    else
        % 11-20,21-30...
        s=find(A>=x+1 & A<=x+10);
        fprintf("%.2f%%\n", length(s)/(rows*cols)*100);
    end
end