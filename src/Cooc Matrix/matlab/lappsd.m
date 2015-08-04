function Sout = lappsd(I1,filename,Recurse)
%  Sout = lappsd(I1,filename)
%
%
% {I1} First input image (image to put on the left)
%
% {I2} Second input image (image to put on the right)
%
% note I1 & I2 have to be the same size and must be square
%
% {GR} A binary template 1 = I1 0 = I2
%
% {Sout} The result of the spline
%

[k,l] = size(I1);
rec = nargin;

if nargin == 2
	Recurse = 0;
end

if k>3 & l>3
	[L,G] = lap(I1,[.05,.25,.4,.25,.05]);
	Sout = lappsd(G,filename,Recurse+1);
	%
	Tout = L;
	%
	[sr1,sc1] = size(Tout);
	[sr2,sc2] = size(Sout);
	sr = min([sr1,sr2]);
	sc = min([sc1,sc2]);
	%
	Sout = Sout(1:sr,1:sc) + Tout(1:sr,1:sc);
	if nargin == 3
	  Sout = expand2([.05,.25,.4,.25,.05],Sout,1);
	end
      Soutmaxmin = [max(max(Sout)), min(min(Sout))];
else
	Sout = expand2([.05,.25,.4,.25,.05],I1,1);
end


[fft,angl] = psd(Sout,1);

[hr,ha] = psd2hist(fft,45);

filename = sprintf('%s%d.txt', filename, Recurse);
fid = fopen(filename,'at');
fprintf(fid, '%f\t%f\t%f\t%f\t%f\t%f\t%f',Recurse,mean(hr),std(hr),median(hr),mean(ha),std(ha),median(ha));
[m,n] = size(ha);

fprintf(fid,'\tha');

for j=1:n
   fprintf(fid,'\t%f',ha(1,j));
end

fprintf(fid,'\thr');

for j=1:n
   fprintf(fid,'\t%f',hr(1,j));
end
fprintf(fid,'\n');
fclose(fid);


%figure
%colormap(gray);
%imshow(Sout);



