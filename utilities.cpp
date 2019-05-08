#include "utilities.h"

int8_t* Utilities::allocate_1D_int8_t(int nx) {
  int8_t *arr;
  int i;
  arr = new int8_t[nx];
  for(i=0; i<nx; i++) {
    arr[i] = 0.0f;
  }
  return (arr);
}

int8_t** Utilities::allocate_2D_int8_t(int nx, int ny) {
  int8_t **arr2d;
  int i, j;

  arr2d = new int8_t*[nx];
  for(i=0; i<nx; i++) {
    arr2d[i] = new int8_t[ny];
  }
  for(i=0; i<nx; i++) {
    for(j=0; j<ny; j++) {
      arr2d[i][j] = 0.0f;
    }
  }

  return (arr2d);
}

int8_t*** Utilities::allocate_3D_int8_t(int nx, int ny, int nz) {
  int8_t ***arr3d;
  int i, j, k;

  arr3d = new int8_t**[nx];
  for(i=0; i<nx; i++) {
    arr3d[i] = new int8_t*[ny];
    for(j=0; j<ny; j++) {
      arr3d[i][j] = new int8_t[nz];
    }
  }
  for(i=0; i<nx; i++) {
    for(j=0; j<ny; j++) {
      for(k=0; k<nz; k++) {
        arr3d[i][j][k] = 0.0f;
      }
    }
  }

  return (arr3d);
}

int16_t* Utilities::allocate_1D_int16_t(int nx) {
  int16_t *arr;
  int i;
  arr = new int16_t[nx];
  for(i=0; i<nx; i++) {
    arr[i] = 0.0f;
  }
  return (arr);
}

int16_t** Utilities::allocate_2D_int16_t(int nx, int ny) {
  int16_t **arr2d;
  int i, j;

  arr2d = new int16_t*[nx];
  for(i=0; i<nx; i++) {
    arr2d[i] = new int16_t[ny];
  }
  for(i=0; i<nx; i++) {
    for(j=0; j<ny; j++) {
      arr2d[i][j] = 0.0f;
    }
  }

  return (arr2d);
}

int16_t*** Utilities::allocate_3D_int16_t(int nx, int ny, int nz) {
  int16_t ***arr3d;
  int i, j, k;

  arr3d = new int16_t**[nx];
  for(i=0; i<nx; i++) {
    arr3d[i] = new int16_t*[ny];
    for(j=0; j<ny; j++) {
      arr3d[i][j] = new int16_t[nz];
    }
  }
  for(i=0; i<nx; i++) {
    for(j=0; j<ny; j++) {
      for(k=0; k<nz; k++) {
        arr3d[i][j][k] = 0.0f;
      }
    }
  }

  return (arr3d);
}

int* Utilities::allocate_1D_int(int nx) {
  int *arr;
  int i;
  arr = new int[nx];
  for(i=0; i<nx; i++) {
    arr[i] = 0.0f;
  }
  return (arr);
}

int** Utilities::allocate_2D_int(int nx, int ny) {
  int **arr2d;
  int i, j;

  arr2d = new int*[nx];
  for(i=0; i<nx; i++) {
    arr2d[i] = new int[ny];
  }
  for(i=0; i<nx; i++) {
    for(j=0; j<ny; j++) {
      arr2d[i][j] = 0.0f;
    }
  }

  return (arr2d);
}

int*** Utilities::allocate_3D_int(int nx, int ny, int nz) {
  int ***arr3d;
  int i, j, k;

  arr3d = new int**[nx];
  for(i=0; i<nx; i++) {
    arr3d[i] = new int*[ny];
    for(j=0; j<ny; j++) {
      arr3d[i][j] = new int[nz];
    }
  }
  for(i=0; i<nx; i++) {
    for(j=0; j<ny; j++) {
      for(k=0; k<nz; k++) {
        arr3d[i][j][k] = 0.0f;
      }
    }
  }

  return (arr3d);
}

float* Utilities::allocate_1D_float(int nx) {
	float *arr;
  int i;
  arr = new float[nx];
  for(i=0; i<nx; i++) {
    arr[i] = 0.0f;
  }
  return (arr);
}

float** Utilities::allocate_2D_float(int nx, int ny) {
  float **arr2d;
  int i, j;

  arr2d = new float*[nx];
  for(i=0; i<nx; i++) {
    arr2d[i] = new float[ny];
  }
  for(i=0; i<nx; i++) {
    for(j=0; j<ny; j++) {
  		arr2d[i][j] = 0.0f;
		}
  }

  return (arr2d);
}

float*** Utilities::allocate_3D_float(int nx, int ny, int nz) {
  float ***arr3d;
  int i, j, k;

  arr3d = new float**[nx];
  for(i=0; i<nx; i++) {
    arr3d[i] = new float*[ny];
    for(j=0; j<ny; j++) {
    	arr3d[i][j] = new float[nz];
    }
  }
  for(i=0; i<nx; i++) {
    for(j=0; j<ny; j++) {
    	for(k=0; k<nz; k++) {
  			arr3d[i][j][k] = 0.0f;
  		}
		}
  }

  return (arr3d);
}

float Utilities::random_between_vals(float v1, float v2) {
  return ((v2 - v1)*(((float)rand())/((float)RAND_MAX)) + v1);
}
