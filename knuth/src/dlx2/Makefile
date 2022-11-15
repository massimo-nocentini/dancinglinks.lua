
dlx2:
	gcc -I/usr/local/sgb/include/ -Wall -O2 dlx2.c -o dlx2 -lgb

tangle:
	/usr/local/bin/ctangle dlx2.w 
	/usr/local/bin/cweave dlx2.w 

pdf:
	pdftex dlx2.tex
