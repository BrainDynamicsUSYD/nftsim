function [result,karray]=resortlattice(c,deltax,xlen)
% Resort the square k_x and k_y lattice into a modulus k array
% [result,karray]=resortlattice(c,deltax,xlen)
ylen=xlen;
% Build k array for resorting
a=zeros(xlen,ylen);
for i=[0:(xlen-1)]
  for j=[0:(ylen-1)]
    a(i+1,j+1)=i*i+j*j;
  end
end
a=reshape(a,xlen*ylen,1);
nfreq=size(c,1); % Determine the number of elements in frequency dimension
c=reshape(c,nfreq,xlen*ylen)'; % Prepare for sorting
% Combine the array of K values to test array
b=[a,c];
% sort the arrays based on the k array prepended
sortedb=sortrows(b,1);
% Create array to hold the omega, |k| data
nmodk=(xlen*(xlen+1))/2;
result=zeros(nmodk,nfreq);
karray=zeros(nmodk,1);
% Average duplicate entries and copy non-duplicated entries
ii=1; %Index in array result
i=1; %Index in array sortedb
while(i<(xlen*ylen))
  if( sortedb(i,1)==sortedb(i+1,1) )
    result(ii,:)=(sortedb(i,2:nfreq+1)+sortedb(i+1,2:nfreq+1))/2;
    karray(ii)=sortedb(i);
    i=i+2;
  else
    result(ii,:)=sortedb(i,2:nfreq+1);
    karray(ii)=sortedb(i);
    i=i+1;
  end
  ii=ii+1;
end
result(ii,:)=sortedb(i,2:nfreq+1);
karray(ii)=sortedb(i);
deltak=2*pi/(deltax*2*(xlen-1)) % Note N is the number in FFT in the original not symmetric squeezed array
karray(:)=sqrt(karray(:))*deltak;
clear sortedb;
end
