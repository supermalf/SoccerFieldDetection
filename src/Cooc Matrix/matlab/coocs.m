function [mata,matb,matc,matd] = coocs(Image,D,thesize)
%
%  [mata,matb,matc,matd] = cooc(Image,D,thesize)
%
%
%

[m,n] = size(Image);

% mx = max(max(Image));

mx = thesize;

mat1 = zeros(mx,mx);
mat2 = zeros(mx,mx);
mat3 = zeros(mx,mx);
mat4 = zeros(mx,mx);

mata = zeros(mx,mx);
matb = zeros(mx,mx);
matc = zeros(mx,mx);
matd = zeros(mx,mx);

%135 degrees
for r=1+D:m,
	for c=1+D:n,
		row = Image(r,c);
		col = Image(r-D,c-D);
		mat1(row,col) = mat1(row,col) + 1;
	end
end

%90 degrees
for r=1+D:m,
	for c=1:n,
		row = Image(r,c);
		col = Image(r-D,c);
		mat2(row,col) = mat2(row,col) + 1;
	end
end


%45 degrees
for r=1+D:m,
	for c=1:n-D,
		row = Image(r,c);
		col = Image(r-D,c+D);
		mat3(row,col) = mat3(row,col) + 1;
	end
end

%0 degrees
for r=1:m,
	for c=1:n-D,
		row = Image(r,c);
		col = Image(r,c+D);
		mat4(row,col) = mat4(row,col) + 1;
	end
end

if nargout==1 
   mata = mat1+mat2+mat3+mat4;
else
   mata = mat1;
   matb = mat2;
   matc = mat3;
   matd = mat4;
end