function [histo,chisto] = hg( a, levels );

%
% [histo,chisto] = hg( a, levels )
%
% calculates a histogram of an input image a
% with size xsize x ysize and with levels grey levels
% Returns the histogram and the cumulative histogram.
%

imsiz = size( a );
h = zeros(1, levels);
ch = zeros(1, levels);

for i=1:imsiz(1),
  for j=1:imsiz(2),
    index = a(i,j)+1;
    h( index ) = h( index )+1;
  end
end

ch(1,1) = h(1,1);

hlp = size(h);
index = hlp(2);

for i = 2:index,
  ch(1,i) = h(1,i) + ch(1,i-1);
end

% bar(h);
% pause;
% bar(ch);
% pause;

histo = h;
chisto = ch;
