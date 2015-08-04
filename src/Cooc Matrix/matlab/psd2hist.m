function [histrad,histang] = psd2hist(psd,bins,plot)
%
% [histrad,histang] = psd2hist(psd,bins,plot)
%
%
[m,n] = size(psd);

rcent = m/2 + 1;
ccent = n/2 + 1;

maxbin = sqrt( (m-rcent+1)^2 + (n-ccent+1)^2 );
binlen = maxbin / bins;
binlena = 360 / bins;

histrad = zeros(1,bins+1);
arearad = zeros(1,bins+1);

histang = zeros(1,bins+1);
areaang = zeros(1,bins+1);

for r=1:m,
	for c=1:n,
		%find radian bin
		rdist = sqrt((r-rcent)^2 + (c-ccent)^2);
		index = floor(rdist/binlen) + 1;			
	
		histrad(index) = histrad(index) + psd(r,c);	
		arearad(index) = arearad(index) + 1;

		%find angular bin
		rang = 180/pi*atan2((rcent-r),(ccent-c));
		if rang < 0
			rang = rang + 360;
		end
		indexa = floor(rang/binlena) + 1;

		histang(indexa) = histang(indexa) + psd(r,c);	
		areaang(indexa) = areaang(indexa) + 1;
	end
end

[m,n] = size(histrad);
for c = 1:n,
	if arearad(c) > 0
		histrad(c) = histrad(c) / arearad(c);
	else
		histrad(c) = 0;
	end
end

[m,n] = size(histang);
for c = 1:n,
	if areaang(c) > 0
		histang(c) = histang(c) / areaang(c);
	else
		histang(c) = 0;
	end
end
if nargin == 3
	figure;
	bar(histrad);
	title('Radial Histogram');
	figure;
	bar(histang);
	title('Angular Histogram');
end
