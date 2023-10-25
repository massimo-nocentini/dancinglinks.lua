/*1:*/
#line 6 "queens-dlx.w"

#include <stdio.h> 
#include <stdlib.h> 
int pn;
/*4:*/
#line 46 "queens-dlx.w"

char encode(x)
int x;
{
if(x<10)return'0'+x;
else if(x<36)return'a'+x-10;
else return'A'+x-36;
}

/*:4*/
#line 10 "queens-dlx.w"
;

main(int argc,char*argv[])
{
register int j,k,n,nn,t;
/*2:*/
#line 20 "queens-dlx.w"

if(argc!=2||sscanf(argv[1],"%d",&pn)!=1){
fprintf(stderr,"Usage: %s n\n",argv[0]);
exit(-1);
}
n= pn,nn= n+n-2;
if(nn> 62){
fprintf(stderr,"Sorry, I can't currently handle n>32!\n");
exit(-2);
}
printf("| This data produced by %s %d\n",
argv[0],n);

/*:2*/
#line 15 "queens-dlx.w"
;
/*3:*/
#line 37 "queens-dlx.w"

for(j= 0;j<n;j++){
t= (j&1?n-1-j:n+j)>>1;
printf("r%c c%c ",encode(t),encode(t));
}
printf("|");
for(j= 1;j<nn;j++)printf(" a%c b%c",encode(j),encode(j));
printf("\n");

/*:3*/
#line 16 "queens-dlx.w"
;
/*5:*/
#line 55 "queens-dlx.w"

for(j= 0;j<n;j++)for(k= 0;k<n;k++){
printf("r%c c%c",encode(j),encode(k));
t= j+k;
if(t&&(t<nn))printf(" a%c",encode(t));
t= n-1-j+k;
if(t&&(t<nn))printf(" b%c",encode(t));
printf("\n");
}

/*:5*/
#line 17 "queens-dlx.w"
;
}

/*:1*/
