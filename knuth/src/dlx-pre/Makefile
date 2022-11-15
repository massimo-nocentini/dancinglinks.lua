
pre:
	gcc -I/usr/local/sgb/include/ -Wall -O2 dlx-pre.c -o dlx-pre -lgb

tangle:
	/usr/local/bin/ctangle dlx-pre.w 
	/usr/local/bin/cweave dlx-pre.w 

pdf:
	pdftex dlx-pre.tex
