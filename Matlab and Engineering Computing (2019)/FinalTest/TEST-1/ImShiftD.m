clear all
% B->A (edge<-center->edge)
filename='ImShiftD.avi';
% read image files
f=imread('A.jpg');
g=imread('B.jpg');
% rows*cols size
[rows,cols,dim]=size(f);
% frame variable
h=g;

% create the video object
video=VideoWriter(filename);
% open the file for writing
open(video);

% duration
step=0.005;
for t=0:step:1
    if (t~=0)
        x=uint64(rows*t/2);
        y=uint64(cols*t/2);
        a=rows/2-x+1;
        b=rows/2+x;
        c=cols/2-y+1;
        d=cols/2+y;
        h(a:b,c:d,:)=(1-t)*g(a:b,c:d,:)+t*f(a:b,c:d,:);
    end
    % write the frame to file
    writeVideo(video,h);
end

% close the file
close(video);