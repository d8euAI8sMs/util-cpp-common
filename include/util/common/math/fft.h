#pragma once

#include <math.h>

#include <util/common/math/complex.h>

inline void fourier(math::complex <> * data, int n, int is)
{
    int i, j, istep;
    int m, mmax;
    double r, r1, theta, w_r, w_i, temp_r, temp_i;
    double pi = 3.1415926;

    r = pi*is;
    j = 0;
    for (i = 0; i < n; i++)
    {
        if (i < j)
        {
            temp_r = data[j].re;
            temp_i = data[j].im;
            data[j].re = data[i].re;
            data[j].im = data[i].im;
            data[i].re = temp_r;
            data[i].im = temp_i;
        }
        m = n >> 1;
        while (j >= m) { j -= m; m = (m + 1) / 2; }
        j += m;
    }
    mmax = 1;
    while (mmax < n)
    {
        istep = mmax << 1;
        r1 = r / (double) mmax;
        for (m = 0; m < mmax; m++)
        {
            theta = r1*m;
            w_r = (double) cos((double) theta);
            w_i = (double) sin((double) theta);
            for (i = m; i < n; i += istep)
            {
                j = i + mmax;
                temp_r = w_r*data[j].re - w_i*data[j].im;
                temp_i = w_r*data[j].im + w_i*data[j].re;
                data[j].re = data[i].re - temp_r;
                data[j].im = data[i].im - temp_i;
                data[i].re += temp_r;
                data[i].im += temp_i;
            }
        }
        mmax = istep;
    }
    if (is > 0)
        for (i = 0; i < n; i++)
        {
        data[i].re /= (double) n;
        data[i].im /= (double) n;
        }

}
