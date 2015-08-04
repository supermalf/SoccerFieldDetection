function [absfft,anglefft] = psd(xcimage,iflog)
%
% [absfft,anglefft] = psd(xcimage,iflog)
%
%

ft = fft2(xcimage);

fts = fftshift(ft);

absft = abs(fts);

anglefft = angle(fts);

absfft = absft .* absft;

if nargin == 2

  absfft = log(absfft + 1);

end
