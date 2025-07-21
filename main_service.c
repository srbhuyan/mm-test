#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <thmgr.h>
// ---- thread management code start ----

int num_threads;
threadpool thpool;

typedef struct PData {
  double ** a;
  double ** b;
  double ** res;
  int       r;
  int       c;
  int       rStart;
  int       rEnd;
} PData;

void mult(double ** a, double ** b, double ** res, int r, int c, int rStart, int rEnd);

void mult_worker(void * data){
  PData * d = (PData *)data;
  mult(d->a, d->b, d->res, d->r, d->c, d->rStart, d->rEnd);
}

void mult_parallel(double ** a, double ** b, double ** res, int r, int c){

  PData* row_data = (PData*)malloc(num_threads * sizeof(PData));
  int rows_per_thread = r / num_threads;
  int remaining_rows = r % num_threads;

  for(int i=0; i<num_threads; i++){
    row_data[i].a      = a;
    row_data[i].b      = b;
    row_data[i].res    = res;
    row_data[i].r      = r;
    row_data[i].c      = c;
    row_data[i].rStart = i * rows_per_thread;
    row_data[i].rEnd   = (i+1) * rows_per_thread;

    if(i == num_threads - 1){
      row_data[i].rEnd += remaining_rows;
    }

    thpool_add_work(thpool, *mult_worker, &row_data[i]);
  }

  thpool_wait(thpool);

  free(row_data);
}

// ---- thread management code end ----

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

int main_worker(int argc, char * argv[]){

  if(argc < 3){
    printf("Usage: %s <row count> <column count> <num threads>\n", argv[0]);
    return 1;
  }

  // Set number of threads (default to number of CPU cores)
  if(argc > 3) {
    num_threads = atoi(argv[3]);
  } else {
    num_threads = sysconf(_SC_NPROCESSORS_ONLN);
  }

  // job id
  if(argc <= 4) {
    return 1;
  }

  // Get thread pool
  char * jid = argv[4];
  thpool = thpool_get_shared(jid);

  int r  = atoi(argv[1]);
  int c  = atoi(argv[2]);

  double ** a   = allocateMatrix(r, c);
  double ** b   = allocateMatrix(r, c);
  double ** res = allocateMatrix(r, c);

  double result    = 0.0;

  printf("Input: row = %d, col = %d, num threads = %d\n", r, c, num_threads);

  // init data
  for(int i=0;i<r;i++){
    for(int j=0;j<c;j++){
      a[i][j] = (double)(i + j);
      b[i][j] = (double)(i + j);
      res[i][j] = 0;
    }
  }

  print(a, r, c);
  print(b, r, c);

  mult_parallel(a, b, res, r, c);

  print(res, r, c);

  freeMatrix(a, r);
  freeMatrix(b, r);
  freeMatrix(res, r);

  return 0;
}

