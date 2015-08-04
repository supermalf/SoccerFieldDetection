function [cRed, cGreen, cHue, cIntensity] = coocg(filename, D, toSave)

%
%  [Red, Green, Hue, Intensity] = coocg(filename, D, toSave)
%

[Red, Green, Blue] = tiffrd(filename);

% Use MATLAB functions to convert from RGB to HSI
%
%[X, RGBMap] = rgb2ind(Red, Green, Blue);
%HSVMap = rgb2hsv(RGBMap);
%[Hue, Saturation, Intensity] = ind2rgb(X ,HSVMap);

% Use a specialy written function to convert from RGB to HSI
[Hue, Saturation, Intensity] = rgb2hsi(Red, Green, Blue);

Red = floor(Red*255)+1;
Green = floor(Green*255)+1;
Hue = floor(Hue*255)+1;
Intensity = floor(Intensity*255)+1;

cRed =       cooc(Red      , D);
cGreen =     cooc(Green    , D);
cHue =       cooc(Hue      , D);
cIntensity = cooc(Intensity, D);

if nargin == 3

  cd results

  outfile = sprintf('%sR.tif', filename); 
  outfile = strrep(outfile, outfile(1), '');
  tiffwrite(cRed, outfile);

  outfile = sprintf('%sG.tif', filename);
  outfile = strrep(outfile, outfile(1), ''); 
  tiffwrite(cGreen, outfile);

  outfile = sprintf('%sH.tif', filename);
  outfile = strrep(outfile, outfile(1), '');
  tiffwrite(cHue, outfile);

  outfile = sprintf('%sI.tif', filename);
  outfile = strrep(outfile, outfile(1), '');
  tiffwrite(cIntensity, outfile);

  cd ..

else if nargin==4

  figure;
  colormap(gray);
  imshow(cRed);
  printmsg = sprintf('%s cooccurrance matrix for RED', filename);
  title(printmsg);

  figure;
  colormap(gray);
  imshow(cGreen);
  printmsg = sprintf('%s cooccurrance matrix for GREEN', filename);
  title(printmsg);

  figure;
  colormap(gray);
  imshow(cHue);
  printmsg = sprintf('%s cooccurrance matrix for HUE', filename);
  title(printmsg);

  figure;
  colormap(gray);
  imshow(cIntensity);
  printmsg = sprintf('%s cooccurrance matrix for INTENSITY', filename);
  title(printmsg);

end
end