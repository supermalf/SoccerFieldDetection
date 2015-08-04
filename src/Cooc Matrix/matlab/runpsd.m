function [hr_r,ha_r,hr_g,ha_g,hr_h,ha_h,hr_i,ha_i,hr_I,ha_I] = runpsd(filename,plot)
%
% [hr_r,ha_r,hr_g,ha_g,hr_h,ha_h,hr_i,ha_i,hr_I,ha_I] = runpsd(filename,plot)
%
[r,g,b] = tiffrd(filename);

[h,s,i] = rgb2hsi(r,g,b);

I = rgb2gray(r,g,b);

[fftr,angl] = psd(r,1);
[fftg,angl] = psd(g,1);
[ffth,angl] = psd(h,1);
[ffti,angl] = psd(i,1);
[fftI,angl] = psd(I,1);

if nargin == 2
	figure
	waterfall(fftI);
	title(filename);
	figure
	psd2hist(fftI,45,1);
	title(filename);
else
	[hr_r,ha_r] = psd2hist(fftr,45);
	[hr_g,ha_g] = psd2hist(fftg,45);
	[hr_h,ha_h] = psd2hist(ffth,45);
	[hr_i,ha_i] = psd2hist(ffti,45);
	[hr_I,ha_I] = psd2hist(fftI,45);
end


