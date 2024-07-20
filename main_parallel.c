#include <stdio.h>
#include <stdlib.h>
#include <thpool.h>

// ---- thread management code start ----

struct MultData{
  double ** a;
  double ** b;
  double ** res;
  int r;
  int c;
  int rStart;
  int rEnd;
};

void mult(double ** a, double ** b, double ** res, int r, int c, int rStart, int rEnd);

void worker(void * data){
  double ** a      = ((struct MultData *)data)->a;
  double ** b      = ((struct MultData *)data)->b;
  double ** res    = ((struct MultData *)data)->res;
  int       r      = ((struct MultData *)data)->r;
  int       c      = ((struct MultData *)data)->c;
  int       rStart = ((struct MultData *)data)->rStart;
  int       rEnd   = ((struct MultData *)data)->rEnd;

  // init data
  for(int i=rStart;i<rEnd;i++){
    for(int j=0;j<c;j++){
      a[i][j] = (double)(i + j);
      b[i][j] = (double)(i + j);
      res[i][j] = 0;
    }
  }

  mult(a, b, res, r, c, rStart, rEnd);
}

void mult_parallel(double ** a, double ** b, double ** res, int r, int c, int pe, threadpool thpool){

  for(int i=0;i<pe;i++){
    struct MultData * mData = (struct MultData *)malloc(sizeof(struct MultData));
    mData->a      = a;
    mData->b      = b;
    mData->res    = res;
    mData->r      = r;
    mData->c      = c;
    mData->rStart = (r/pe) * i;
    mData->rEnd   = (r/pe) * (i+1);

    thpool_add_work(thpool, worker, (void *)mData);
  }

  thpool_wait(thpool);
}
// ---- thread management code end ----

// The only changes to the user code `real_mult` are the introduction of
// parameters `rStart` and `rEnd` and their use in the loop index. 

void mult(double ** a, double ** b, double ** res, int r, int c, int rStart, int rEnd){
  for(int i=rStart;i<rEnd;i++){
    for(int j=0;j<c;j++){
      for(int k=0;k<r;k++){
        res[i][j] += a[i][k] * b[k][j];
      }
    }
  }
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

  if(argc < 4){
    printf("Usage: main <row count> <column count> <processing elements>\n");
    exit(1);
  }

  int r    = atoi(argv[1]);
  int c    = atoi(argv[2]);
  int core = atoi(argv[3]);

  // thread pool
  threadpool thpool = thpool_init(core);

  double ** a   = allocateMatrix(r, c);
  double ** b   = allocateMatrix(r, c);
  double ** res = allocateMatrix(r, c);

  double result    = 0.0;

  printf("Input: row = %d, col = %d, core = %d\n", r, c, core);

//  // init data
//  for(int i=0;i<r;i++){
//    for(int j=0;j<c;j++){
//      a[i][j] = (double)(i + j);
//      b[i][j] = (double)(i + j);
//    }
//  }

  print(a, r, c);
  print(b, r, c);

  //  mult(a, b, res, r, c);
  mult_parallel(a, b, res, r, c, core, thpool);

  print(res, r, c);

  freeMatrix(a, r);
  freeMatrix(b, r);
  freeMatrix(res, r);

  thpool_destroy(thpool);

  return 0;
}
