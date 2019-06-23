clear
clc
A = [3 2 1; 2 2 1 ; 1 2 1 ; 0 1 1];
y = [4; 7; 2 ; 5];
k = inv(A'*A)*(A')*y; 
D = A*k-y; 