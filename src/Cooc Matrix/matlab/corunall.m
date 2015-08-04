function [stats,names] = corunall(plot)
%
% [stats,names] = crounall(plot)
%
tic;

names(1,:) = 'cy1-002.tif ';
names(2,:) = 'cy3-222.tif ';
names(3,:) = 'cy4-260.tif ';
names(4,:) = 'gd_002.tif  ';
names(5,:) = 'gd2-002.tif ';
names(6,:) = 'gd3-002.tif ';
names(7,:) = 'gd4-089.tif ';
names(8,:) = 'gd5-119.tif ';
names(9,:) = 'gd6-132.tif ';
names(10,:) = 'gd7-132.tif ';
names(11,:) = 'gd8-132.tif ';
names(12,:) = 'gd9-132.tif ';
names(13,:) = 'gd10-132.tif';
names(14,:) = 'gd11-162.tif';
names(15,:) = 'gd12-162.tif';
names(16,:) = 'gd13-162.tif';
names(17,:) = 'gd14-206.tif';
names(18,:) = 'gd15-206.tif';
names(19,:) = 'gd16-206.tif';
names(20,:) = 'gd17-206.tif';
names(21,:) = 'gd18-258.tif';
names(22,:) = 'gd19-258.tif';
names(23,:) = 'hl1-050.tif ';
names(24,:) = 'hl2-135.tif ';
names(25,:) = 'hl3-135.tif ';
names(26,:) = 'me1-124.tif ';
names(27,:) = 'me2-124.tif ';
names(28,:) = 'me3-124.tif ';
names(29,:) = 'me4-200.tif ';
names(30,:) = 'me5-200.tif ';
names(31,:) = 'me6-200.tif ';
names(32,:) = 'me7-200.tif ';
names(33,:) = 'os1-222.tif ';
names(34,:) = 'os2-222.tif ';
names(35,:) = 'rm1-008.tif ';
names(36,:) = 'rm2-008.tif ';
names(37,:) = 'rm3-050.tif ';
names(38,:) = 'rm4-050.tif ';
names(39,:) = 'rm5-135.tif ';
names(40,:) = 'rm6-135.tif ';
names(41,:) = 'rm7-164.tif ';
names(42,:) = 'sc1-007.tif ';
names(43,:) = 'sc2-007.tif ';
names(44,:) = 'sc3-008.tif ';
names(45,:) = 'sc4-008.tif ';
names(46,:) = 'sc5-008.tif ';
names(47,:) = 'sc6-089.tif ';
names(48,:) = 'sc7-106.tif ';
names(49,:) = 'sc8-106.tif ';
names(50,:) = 'sc9-106.tif ';
names(51,:) = 'sc10-119.tif';
names(52,:) = 'sc11-119.tif';
names(53,:) = 'st1-007.tif ';
names(54,:) = 'st3-106.tif ';
names(55,:) = 'st4-164.tif ';
names(56,:) = 'st5-258.tif ';


[m,n] = size(names);


for i=1:m,
  [r1, g1, h1, i1] = coocg(names(i,:),1);
  fid = fopen('corr2.txt', 'a+');
  fprintf(fid,'\n\n\n');
  fprintf(fid,'   file 1       file 2           red          green        hue       intensity  \n');
  fclose(fid);

  fid1 = fopen('std2.txt', 'a+');
  fprintf(fid1,'%s: %f %f %f %f\n', names(i,:), std2(r1), std2(g1), std2(h1), std2(i1));
  fclose(fid1);

  for j=1:m
    fili = i
    filj = j
    [r2, g2, h2, i2] = coocg(names(j,:),1);
    dotred = corr2(r1,r2);
    dotgre = corr2(g1,g2);
    dothue = corr2(h1,h2);
    dotgra = corr2(i1,i2);

    fid = fopen('corr2.txt', 'a+');
    fprintf(fid, '%s %s %12d %12d %12d %12d \n', names(i,:), names(j,:), dotred, dotgre, dothue, dotgra);
    fclose(fid);
    toc 
  end
end

