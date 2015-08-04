function [stats,results] = runall(plot)
%
% result = runallpsd(filename,plot)
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
	fil = i
	[hr_r,ha_r,hr_g,ha_g,hr_h,ha_h,hr_i,ha_i,hr_I,ha_I] = runpsd(names(i,:));
	stats(i,1) = mean(hr_r);
	stats(i,2) = std(hr_r);
	stats(i,3) = median(hr_r);

	stats(i,4) = mean(ha_r);
	stats(i,5) = std(ha_r);
	stats(i,6) = median(ha_r);

	stats(i,7) = mean(hr_g);
	stats(i,8) = std(hr_g);
	stats(i,9) = median(hr_g);

	stats(i,10) = mean(ha_g);
	stats(i,11) = std(ha_g);
	stats(i,12) = median(ha_g);

	stats(i,13) = mean(hr_h);
	stats(i,14) = std(hr_h);
	stats(i,15) = median(hr_h);

	stats(i,16) = mean(ha_h);
	stats(i,17) = std(ha_h);
	stats(i,18) = median(ha_h);

	stats(i,19) = mean(hr_i);
	stats(i,20) = std(hr_i);
	stats(i,21) = median(hr_i);

	stats(i,22) = mean(ha_i);
	stats(i,23) = std(ha_i);
	stats(i,24) = median(ha_i);

	stats(i,25) = mean(hr_I);
	stats(i,26) = std(hr_I);
	stats(i,27) = median(hr_I);

	stats(i,28) = mean(ha_I);
	stats(i,29) = std(ha_I);
	stats(i,30) = median(ha_I);

	j = i * 10 - 9;
	results(j+0,:) = hr_r;
	results(j+1,:) = ha_r;
	results(j+2,:) = hr_g;
	results(j+3,:) = ha_g;
	results(j+4,:) = hr_h;
	results(j+5,:) = ha_h;
	results(j+6,:) = hr_i;
	results(j+7,:) = ha_i;
	results(j+8,:) = hr_I;
	results(j+9,:) = ha_I;
	
	toc 
end
