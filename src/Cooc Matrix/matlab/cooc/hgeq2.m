function eqa = hgeq2( a, chisto, nsize );

%
% eqa = hgeq2( a, chisto, nsize )
%
% returns the histogram equalized image of a
% by using the cumulative histogram.
% The equalized histogram is of size nsize.
%

imsiz = size( a );
total = imsiz(1)*imsiz(2);

eqa = zeros(imsiz(1), imsiz(2));

for i = 1:imsiz(1),
  for j = 1:imsiz(2),
    eqa(i,j) = round( chisto(1,a(i,j)+1)*nsize/total - 0.499 );
    if eqa(i,j)==nsize,
      eqa(i,j) = nsize-1;
    end
  end  
end

