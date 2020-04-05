clear all
% A->B
filename='ImShiftA.avi';
% read image file
f=imread('A.jpg');
% rows*cols size
[rows,cols,dim]=size(f);
% turn A to 3d gray
p=rgb2gray(f);
g=cat(dim,p,p,p);
imwrite(g,'B.jpg');
% frame variable
h=uint8(zeros(rows,cols,dim));

% create the video object
video=VideoWriter(filename);
% open the file for writing
open(video);

% duration
step=0.005;
for t=0:step:1
    h=(1-t)*f+t*g;
    % write the frame to file
    writeVideo(video,h);
end

% close the file
close(video);