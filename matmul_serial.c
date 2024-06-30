#include <stdio.h>
#include <stdlib.h>

void real_mult(double ** a, double ** b, double ** res, int r, int c){
  for(int i=0;i<r;i++){
    for(int j=0;j<c;j++){
      res[i][j] = 0;
      for(int k=0;k<r;k++){
        res[i][j] += a[i][k] * b[k][j];
      }
    }
  }
}

void mult(double ** a, double ** b, double ** res, int r, int c){

  // do stuff
  real_mult(a, b, res, r, c);
  // do more stuff    
}

void print(double ** a, int r, int c){
  r = r > 5 ? 5 : r;
  c = c > 5 ? 5 : c;

  printf("\n");
  for(int row=0;row<r;row++){
    for(int col=0;col<c;col++){
      printf("%.2lf ", a[row][col]);
    }
    printf("\n");
  }
  printf("\n");
}

double ** allocateMatrix(int r, int c){
  double ** m = (double **)malloc(r * sizeof(double *));
  for(int i=0;i<r;i++){
    m[i] = (double *)malloc(c * sizeof(double));
  }
  return m;
}

void freeMatrix(double ** m, int r){
  for(int i=0;i<r;i++){
    free(m[i]);
  }
  free(m);
}

int main(int argc, char * argv[]){

  if(argc < 3){
    printf("Usage: main <row count> <column count>\n");
    exit(1);
  }

  int r  = atoi(argv[1]);
  int c  = atoi(argv[2]);

  double ** a   = allocateMatrix(r, c);
  double ** b   = allocateMatrix(r, c);
  double ** res = allocateMatrix(r, c);

  double result    = 0.0;

  printf("Input: row = %d, col = %d\n", r, c);

  // init data
  for(int i=0;i<r;i++){
    for(int j=0;j<c;j++){
      a[i][j] = (double)(i + j);
      b[i][j] = (double)(i + j);
    }
  }

  print(a, r, c);
  print(b, r, c);

    mult(a, b, res, r, c);

  print(res, r, c);

  freeMatrix(a, r);
  freeMatrix(b, r);
  freeMatrix(res, r);

  return 0;
}
