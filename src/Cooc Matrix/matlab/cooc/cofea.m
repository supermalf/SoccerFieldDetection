function cf = cofea( com );

%
% cf = cofea( com )
%
% Calculates and returns the our features based
% on the co-occurence matrix. Returns them as a vector
% in the order energy, inertia, entropy homogentity.
%

msiz = size( com );
energy = 0;
inertia = 0;
entropy = 0;
homogen = 0;
counter = 0;


for i = 1: msiz(1),
  for j = 1: msiz(2),
    counter = counter+com(i,j);
  end
end

for i = 1: msiz(1),
  for j = 1: msiz(2),
    hlp = com(i,j)/counter;
    energy = energy + hlp*hlp;
    inertia = inertia + (i-j)*(i-j)*hlp;
    hlp2 = hlp;
    if hlp2 == 0,
      hlp2 = 1;
    end
    entropy = entropy + hlp*log(hlp2);
    homogen = homogen + hlp/(1+(i-j)*(i-j));
  end
end

cf = [ energy inertia entropy homogen ];
