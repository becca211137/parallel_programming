#include "PPintrin.h"

#include <math.h>
#include <iostream>
// implementation of absSerial(), but it is vectorized using PP intrinsics
void absVector(float *values, float *output, int N)
{
  __pp_vec_float x;
  __pp_vec_float result;
  // 根據vector width設vector
  __pp_vec_float zero = _pp_vset_float(0.f);
  // 根據vector width設vector(bool值)
  __pp_mask maskAll, maskIsNegative, maskIsNotNegative;

  //  Note: Take a careful look at this loop indexing.  This example
  //  code is not guaranteed to work when (N % VECTOR_WIDTH) != 0.
  //  Why is that the case?
  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {

    // All ones
    maskAll = _pp_init_ones();

    // All zeros
    maskIsNegative = _pp_init_ones(0);

    // Load vector of values from contiguous memory addresses
    // 會跑一個迴圈，一次是做一個vector_width
    _pp_vload_float(x, values + i, maskAll); // x = values[i];

    // Set mask according to predicate
    _pp_vlt_float(maskIsNegative, x, zero, maskAll); // if (x < 0) {

    // Execute instruction using mask ("if" clause)
    _pp_vsub_float(result, zero, x, maskIsNegative); //   output[i] = -x;

    // Inverse maskIsNegative to generate "else" mask
    maskIsNotNegative = _pp_mask_not(maskIsNegative); // } else {

    // Execute instruction ("else" clause)
    _pp_vload_float(result, values + i, maskIsNotNegative); //   output[i] = x; }

    // Write results back to memory
    _pp_vstore_float(output + i, result, maskAll);
  }
}

void clampedExpVector(float *values, int *exponents, float *output, int N)
{
  __pp_vec_float x;
  __pp_vec_int y;
  __pp_vec_float result ,result_temp;
  __pp_vec_int zero_vector, y_temp, y_temp2;
  __pp_vec_int one_vector;
  __pp_mask which_zero_mask, which_not_zero_mask, clamp_mask;
  __pp_mask one_mask;
  __pp_vec_float temp, nine, zero_float;
  zero_vector = _pp_vset_int(0);
  zero_float = _pp_vset_float(0.f);
  nine = _pp_vset_float(9.999999f);
  one_vector = _pp_vset_int(1);
  one_mask = _pp_init_ones();

  int total = floor( N / VECTOR_WIDTH); /// 要做幾次
  int remainder = N % VECTOR_WIDTH;
  int index=0;
  for (int i = 0; i < N; i += VECTOR_WIDTH){
    y_temp = _pp_vset_int(0);
    y_temp2 = _pp_vset_int(0);
    which_zero_mask = _pp_init_ones(0);
    which_not_zero_mask = _pp_init_ones(0);
    clamp_mask = _pp_init_ones(0);
    temp = _pp_vset_float(0.f);
    _pp_vload_float(x, values + i, one_mask); // x = values[i];
    _pp_vload_float(result, values + i, one_mask); // x = values[i];
    _pp_vload_int(y, exponents + i, one_mask); // y = exponents[i];
    _pp_veq_int(which_zero_mask, y, zero_vector, one_mask); // 記錄哪個是0
    which_not_zero_mask = _pp_mask_not(which_zero_mask);

    _pp_vset_float(result, 1.f, which_zero_mask); // 把exp是0的result設1
    _pp_vsub_int(y_temp, y, one_vector, which_not_zero_mask);
    _pp_veq_int(which_zero_mask, y_temp, zero_vector, one_mask); // 記錄哪個是0
    which_not_zero_mask = _pp_mask_not(which_zero_mask);
    int count = _pp_cntbits(which_not_zero_mask);
    result_temp = result;
    while(count>0){
      _pp_vmult_float(result_temp, result, x, which_not_zero_mask);
      _pp_vsub_int(y_temp2, y_temp, one_vector, which_not_zero_mask);
      _pp_veq_int(which_zero_mask, y_temp2, zero_vector, one_mask); // 記錄哪個是0
      which_not_zero_mask = _pp_mask_not(which_zero_mask);
      y_temp = y_temp2;
      result = result_temp;
      y_temp2 = _pp_vset_int(0);
      count = _pp_cntbits(which_not_zero_mask);
    }
    _pp_vsub_float(temp, result, nine, one_mask);
    _pp_vgt_float(clamp_mask, temp, zero_float, one_mask);
    _pp_vset_float(result, 9.999999f, clamp_mask);

    if ((N - i) <= VECTOR_WIDTH && (remainder!=0)){
      __pp_mask final_mask = _pp_init_ones(remainder);
      __pp_mask final_mask_2 = _pp_mask_not(final_mask);

      _pp_vset_float(result, 0.f, final_mask_2);
      _pp_vstore_float(output + i, result, one_mask);

    }
    // Write results back to memory
    else{
      _pp_vstore_float(output + i, result, one_mask);
    }
  }
}

// returns the sum of all elements in values
// You can assume N is a multiple of VECTOR_WIDTH
// You can assume VECTOR_WIDTH is a power of 2
float arraySumVector(float *values, int N)
{
  __pp_vec_float x;
  __pp_mask one_mask;
  one_mask = _pp_init_ones();
  int repeat = -1;
  int width = VECTOR_WIDTH;
  float result=0.0;
  while (width>1){
    repeat++;
    width = width/2;
  }
  for (int i = 0; i < N; i += VECTOR_WIDTH){
    _pp_vload_float(x, values + i, one_mask);
    for (int j = 0; j<repeat;j++){
      _pp_hadd_float(x,x);
      _pp_interleave_float(x,x);
    }
    _pp_hadd_float(x,x);
    float *ans = new float[VECTOR_WIDTH];
    _pp_vstore_float(ans, x, one_mask);
    result+=ans[0];
  }

  return result;
}