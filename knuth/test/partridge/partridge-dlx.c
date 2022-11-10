#define maxn 16					\

#include <stdio.h> 
#include <stdlib.h> 

int n;

int main(int argc,char*argv[]){
  register int i,j,t,N;

  if(argc!=2||sscanf(argv[1],"%d", &n)!=1){
    fprintf(stderr,"Usage: %s n\n", argv[0]);
    exit(-1);
  }

  if(n> maxn){
    fprintf(stderr,"Sorry, I don't presently allow n>%d!\n", maxn);
    exit(-2);
  }

  printf("| %s %d\n", argv[0],n);

  N = n * (n + 1) / 2;

  for(t=1;t<=n;t++) printf("%d|#%d ", t,t);
  for(i= 0;i<N;i++)for(j= 0;j<N;j++) printf("x%dy%d ", i,j);

  printf("\n");

  for (t = 1; t <= n; t++) {
	for (int r = 0; r < N-t+1; r++) {
		for (int c = 0; c < N-t+1; c++) {
			printf("#%d ", t);
			for (int rr = 0; rr < t; rr++) {
				for (int cc = 0; cc < t; cc++) {
					printf("x%dy%d ", r+rr, c+cc);
				}
			}
			printf("\n");
		}
	}
}

  return 0;
}
