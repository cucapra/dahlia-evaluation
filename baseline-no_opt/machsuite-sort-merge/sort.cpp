#include "sort.h"

  int max1 =0; int min1 = 0; int number1 = 0; int total1 = 0;
  int max3 =0; int min3 = 0; int number3 = 0; int total3 = 0;
void merge(TYPE a[SIZE], int start, int m, int stop){
    TYPE temp[SIZE];
    int i, j, k;

        int count1 = 0;
        number1 += 1;
    merge_label1 : for(i=start; i<=m; i++){
          count1 += 1;
        temp[i] = a[i];
    }
        total1 += count1;
        if (count1 >= max1)
            max1 = count1;
        if (count1 <= min1)
            min1 = count1;

    merge_label2 : for(j=m+1; j<=stop; j++){
        temp[m+1+stop-j] = a[j];
    }

    i = start;
    j = stop;

        int count3 = 0;
        number3 += 1;
    merge_label3 : for(k=start; k<=stop; k++){
          count3 += 1;
        TYPE tmp_j = temp[j];
        TYPE tmp_i = temp[i];
        if(tmp_j < tmp_i) {
            a[k] = tmp_j;
            j--;
        } else {
            a[k] = tmp_i;
            i++;
        }
    }
        total3 += count3;
        if (count3 >= max3)
            max3 = count3;
        if (count3 <= min3)
            min3 = count3;
}

void sort(TYPE a[SIZE]) {
#pragma HLS INTERFACE s_axilite port=a
    int start, stop;
    int i, m, from, mid, to;

    start = 0;
    stop = SIZE;

  int max =0; int min = 0; int number = 0; int total = 0;
        int count = 0;
        number += 1;
    mergesort_label1 : for(m=1; m<stop-start; m+=m) {
        mergesort_label2 : for(i=start; i<stop; i+=m+m) {
          count += 1;
            from = i;
            mid = i+m-1;
            to = i+m+m-1;
            if(to < stop){
                merge(a, from, mid, to);
            }
            else{
                merge(a, from, mid, stop);
            }
        }
    }
        total += count;
        if (count >= max)
            max = count;
        if (count <= min)
            min = count;
        printf("1 max: %d, min: %d, total: %d, number: %d, avg: %d \n",max1, min1, total1, number1, total1/number1);
        printf("3 max: %d, min: %d, total: %d, number: %d, avg: %d \n",max3, min3, total3, number3, total3/number3);
        printf("max: %d, min: %d, total: %d, number: %d, avg: %d \n",max, min, total, number, total/number);
}
