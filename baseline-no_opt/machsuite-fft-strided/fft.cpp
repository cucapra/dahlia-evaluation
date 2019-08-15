#include "fft.h"

void fft(double real[FFT_SIZE], double img[FFT_SIZE], double real_twid[FFT_SIZE/2], double img_twid[FFT_SIZE/2]){
#pragma HLS INTERFACE s_axilite port=real
#pragma HLS INTERFACE s_axilite port=img
#pragma HLS INTERFACE s_axilite port=real_twid
#pragma HLS INTERFACE s_axilite port=img_twid

    int even, odd, span, log, rootindex;
    double temp;
    log = 0;

  int max =0; int min = 0; int number = 0; int total = 0;
    outer:for(span=FFT_SIZE>>1; span; span>>=1, log++){
        int count = 0;
        number += 1;
        inner:for(odd=span; odd<FFT_SIZE; odd++){
          count += 1;
            odd |= span;
            even = odd ^ span;

            temp = real[even] + real[odd];
            real[odd] = real[even] - real[odd];
            real[even] = temp;

            temp = img[even] + img[odd];
            img[odd] = img[even] - img[odd];
            img[even] = temp;

            rootindex = (even<<log) & (FFT_SIZE - 1);
            if(rootindex){
                temp = real_twid[rootindex] * real[odd] -
                    img_twid[rootindex]  * img[odd];
                img[odd] = real_twid[rootindex]*img[odd] +
                    img_twid[rootindex]*real[odd];
                real[odd] = temp;
            }
        }
        total += count;
        if (count >= max)
            max = count;
        if (count <= min)
            min = count;
    }
        printf("max: %d, min: %d, total: %d, number: %d, avg: %d \n",max, min, total, number, total/number);
}
