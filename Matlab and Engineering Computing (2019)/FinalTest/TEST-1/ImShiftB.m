clear all

% B->A (left-center-right)
filename='ImShiftB.avi';
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
        x=uint64(cols*t/2);
        y=uint64(cols-x+1);
        h(:,1:x,:)=(1-t)*g(:,1:x,:)+t*f(:,1:x,:);
        h(:,y:cols,:)=(1-t)*g(:,y:cols,:)+t*f(:,y:cols,:);
    end
    % write the frame to file
    writeVideo(video,h);
end

% close the file
close(video);