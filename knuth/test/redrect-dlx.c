#define maxd 36
#define encode(v) ((v) <10?(v) +'0':(v) -10+'a')  \

/*1:*/
#line 25 "redrect-dlx.w"

#include <stdio.h> 
#include <stdlib.h> 
int m,n;
main(int argc,char*argv[]){
register int a,b,c,d,j,k;
/*2:*/
#line 40 "redrect-dlx.w"

if(argc!=3||sscanf(argv[1],"%d",
&m)!=1||sscanf(argv[2],"%d",
&n)!=1){
fprintf(stderr,"Usage: %s m n\n",
argv[0]);
exit(-1);
}
if(m> maxd||n> maxd){
fprintf(stderr,"Sorry, m and n must be at most %d!\n",
maxd);
exit(-2);
}
printf("| redrect-dlx %d %d\n",
m,n);

/*:2*/
#line 31 "redrect-dlx.w"
;
/*3:*/
#line 66 "redrect-dlx.w"

for(j= 0;j<m;j++)for(k= 0;k<n;k++)
printf(" %c%c",
encode(j),encode(k));
for(a= 1;a<m;a++)printf(" 1:%d|x%c",
n,encode(a));
for(c= 1;c<n;c++)printf(" 1:%d|y%c",
m,encode(c));
for(a= 0;a<m;a++)for(b= a+1;b<=m;b++)
printf(" 0:%d|x%c%c",
n,encode(a),encode(b));
for(c= 0;c<n;c++)for(d= c+1;d<=n;d++)
printf(" 0:%d|y%c%c",
m,encode(c),encode(d));
printf("\n");

/*:3*/
#line 32 "redrect-dlx.w"
;
for(a= 0;a<m;a++)for(b= a+1;b<=m;b++){
for(c= 0;c<n;c++)for(d= c+1;d<=n;d++){
/*4:*/
#line 82 "redrect-dlx.w"

for(j= a;j<b;j++)for(k= c;k<d;k++)
printf(" %c%c",
encode(j),encode(k));
if(a)printf(" x%c",
encode(a));
if(c)printf(" y%c",
encode(c));
printf(" x%c%c y%c%c\n",
encode(a),encode(b),encode(c),encode(d));

/*:4*/
#line 35 "redrect-dlx.w"

}
}
}

/*:1*/
