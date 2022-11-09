#define maxn 16 \

/*1:*/
#line 7 "queendom-dlx.w"

#include <stdio.h> 
#include <stdlib.h> 
int m,n;
main(int argc,char*argv[]){
register int i,j,k;
/*2:*/
#line 19 "queendom-dlx.w"

if(argc!=3||sscanf(argv[1],"%d",
&n)!=1||
sscanf(argv[2],"%d",
&m)!=1){
fprintf(stderr,"Usage: %s n m\n",
argv[0]);
exit(-1);
}
if(n> maxn){
fprintf(stderr,"Sorry, I don't presently allow n>%d!\n",
maxn);
exit(-2);
}
printf("| %s %d %d\n",
argv[0],n,m);

/*:2*/
#line 13 "queendom-dlx.w"
;
/*3:*/
#line 36 "queendom-dlx.w"

for(i= 0;i<n;i++)for(j= 0;j<n;j++)
printf("1:%d|%x%x ",
m,i,j);
printf("%d|Q\n",
m);

/*:3*/
#line 14 "queendom-dlx.w"
;
for(i= 0;i<n;i++)for(j= 0;j<n;j++)
/*4:*/
#line 43 "queendom-dlx.w"

{
printf("Q %x%x",
i,j);
for(k= 0;k<n;k++)if(k!=i)
printf(" %x%x",
k,j);
for(k= 0;k<n;k++)if(k!=j)
printf(" %x%x",
i,k);
for(k= 1;i+k<n&&j+k<n;k++)
printf(" %x%x",
i+k,j+k);
for(k= 1;i-k>=0&&j-k>=0;k++)
printf(" %x%x",
i-k,j-k);
for(k= 1;i+k<n&&j-k>=0;k++)
printf(" %x%x",
i+k,j-k);
for(k= 1;i-k>=0&&j+k<n;k++)
printf(" %x%x",
i-k,j+k);
printf("\n");
}

/*:4*/
#line 16 "queendom-dlx.w"
;
}

/*:1*/
