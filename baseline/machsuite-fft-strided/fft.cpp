#include "fft.h"

extern "C"
{
    void fft(double real[FFT_SIZE], double img[FFT_SIZE], double real_twid[FFT_SIZE / 2], double img_twid[FFT_SIZE / 2])
    {
#pragma HLS INTERFACE m_axi port = real offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = img offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = real_twid offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = img_twig offset = slave bundle = gmem
#pragma HLS INTERFACE s_axilite port = real bundle = control
#pragma HLS INTERFACE s_axilite port = img bundle = control
#pragma HLS INTERFACE s_axilite port = real_twid bundle = control
#pragma HLS INTERFACE s_axilite port = img_twid bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control
        int even, odd, span, log, rootindex;
        double temp;
        log = 0;

    outer:
        for (span = FFT_SIZE >> 1; span; span >>= 1, log++)
        {
        inner:
            for (odd = span; odd < FFT_SIZE; odd++)
            {
                odd |= span;
                even = odd ^ span;

                temp = real[even] + real[odd];
                real[odd] = real[even] - real[odd];
                real[even] = temp;

                temp = img[even] + img[odd];
                img[odd] = img[even] - img[odd];
                img[even] = temp;

                rootindex = (even << log) & (FFT_SIZE - 1);
                if (rootindex)
                {
                    temp = real_twid[rootindex] * real[odd] -
                           img_twid[rootindex] * img[odd];
                    img[odd] = real_twid[rootindex] * img[odd] +
                               img_twid[rootindex] * real[odd];
                    real[odd] = temp;
                }
            }
        }
    }
}