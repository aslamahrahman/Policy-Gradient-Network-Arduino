#include <stdlib.h>
#include "Arduino.h"

#pragma once

class Utilities {
	public:
    int8_t* allocate_1D_int8_t(int nx);
    int8_t** allocate_2D_int8_t(int nx, int ny);
    int8_t*** allocate_3D_int8_t(int nx, int ny, int nz);
    int16_t* allocate_1D_int16_t(int nx);
    int16_t** allocate_2D_int16_t(int nx, int ny);
    int16_t*** allocate_3D_int16_t(int nx, int ny, int nz);
    int* allocate_1D_int(int nx);
    int** allocate_2D_int(int nx, int ny);
    int*** allocate_3D_int(int nx, int ny, int nz);
		float* allocate_1D_float(int nx);
		float** allocate_2D_float(int nx, int ny);
		float*** allocate_3D_float(int nx, int ny, int nz);
    float random_between_vals(float v1, float v2);
		void free(void);
};
