
dlx1:
	gcc -I/usr/local/sgb/include/ -Wall -g dlx1.c -o dlx1 -lgb

tangle:
	/usr/local/bin/ctangle dlx1.w 
	/usr/local/bin/cweave dlx1.w 

pdf:
	pdftex dlx1.tex
