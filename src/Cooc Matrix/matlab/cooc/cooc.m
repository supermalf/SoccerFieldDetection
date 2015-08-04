function co = cooc( a, xstep, ystep, levels );

%
% co = cooc( a, xstep, ystep, levels )
%
% Return the co-occurence matrix of the input image a
% with the direction and distance parameters xstep
% and ystep which are indeces (positive integers) 
% that tell the distance
% and direction of the comparison.
% The input image should have levels grey levels
%

imsiz = size( a );
co = zeros( levels );

for i = xstep+1: imsiz(1)-xstep,
  for j = ystep+1: imsiz(2)-ystep,
    co( a(i,j)+1, a(i+xstep,j+ystep) +1 ) = co( a(i,j)+1, a(i+xstep,j+ystep)+1 )+1;
    co( a(i,j)+1, a(i-xstep,j-ystep) +1 ) = co( a(i,j)+1, a(i-xstep,j-ystep)+1 )+1;
  end
end
