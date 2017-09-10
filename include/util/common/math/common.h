#pragma once

#include <functional>
#include <cassert>

#ifndef M_PI
    #define M_PI 3.141592653589793238462643
#endif

namespace common
{



    // type aliases



    using continuous_t = std::function < double(double) > ;
    using sampled_t    = struct
                         {
                             double *samples;
                             size_t count;
                             double period;
                         };
    using bi_op_t      = std::function < double(size_t, double, double) > ;
    using un_op_t      = std::function < double(size_t, double) > ;

    using gaussian_t = struct
    {
        double a, s, t0;
    };

    using sinT_t = struct
    {
        double a, T, t0;
    };



    // operators



    inline bi_op_t identity_op()
    {
        return [] (size_t, double, double s) { return s; };
    }

    inline un_op_t identity_un_op()
    {
        return [] (size_t, double s) { return s; };
    }

    inline bi_op_t add_op()
    {
        return [] (size_t, double d, double s) { return d + s; };
    }

    inline bi_op_t mult_add_op(double factor)
    {
        return [factor] (size_t, double d, double s) { return d + s * factor; };
    }



    // signals




    inline double random(double left = -1, double right = 1)
    {
        return ((double) rand()) / RAND_MAX * (right - left) + left;
    }

    inline continuous_t sin(double magnitude, double frequency)
    {
        return [=] (double t) { return magnitude * std::sin(2 * M_PI * frequency * t); };
    }

    inline continuous_t gaussian(gaussian_t params)
    {
        return [=] (double t) { return params.a * std::exp(- (t - params.t0)*(t - params.t0) / params.s/params.s); };
    }

    inline continuous_t sin(sinT_t params)
    {
        return [=] (double t) { return params.a * std::sin((t - params.t0) * (2 * M_PI / params.T)); };
    }

    inline continuous_t noise(double left = -1, double right = 1, size_t trust = 20)
    {
        return [=] (double t)
        {
            double noise = 0;
            for (size_t i = 0; i < trust; i++)
            {
                noise += random(left, right);
            }
            return noise / trust;
        };
    }

    /**
     * Combines `n` functions together using the given `combiner`
     */
    inline continuous_t combine(size_t n, continuous_t *funcs, bi_op_t combiner = add_op())
    {
        return [=] (double t)
        {
            continuous_t &first = funcs[0];
            double result = first(t);
            for (size_t i = 1u; i < n; i++)
            {
                result = combiner(i, result, funcs[i](t));
            }
            return result;
        };
    }



    // samplers




    /**
     * Returns the signal power.
     */
    inline double sample(continuous_t &continuous,
                         sampled_t    &sampled,
                         size_t       samples = 0)
    {
        if (samples == 0) samples = sampled.count;
        assert(samples <= sampled.count);
        double power = 0, val;
        for (size_t i = 0; i < samples; i++)
        {
            val = continuous(i * sampled.period);
            sampled.samples[i] = val;
            power += val * val;
        }
        return power;
    }

    inline sampled_t allocate_sampled(size_t size, size_t count, double period)
    {
        return { new double[size], count, period };
    }

    inline sampled_t allocate_sampled(size_t size, double period)
    {
        return { new double[size], size, period };
    }

    inline void free_sampled(sampled_t sampled)
    {
        delete[] sampled.samples;
    }




    // mappers




    inline void map(sampled_t &dest,
                    sampled_t &source,
                    bi_op_t   op = identity_op())
    {
        assert(dest.count == source.count);
        assert(abs(dest.period - source.period) < 1e-15);
        for (size_t i = 0; i < dest.count; i++)
        {
            dest.samples[i] = op(i, dest.samples[i], source.samples[i]);
        }
    }

    inline void map(sampled_t &dest,
                    un_op_t   op)
    {
        for (size_t i = 0; i < dest.count; i++)
        {
            dest.samples[i] = op(i, dest.samples[i]);
        }
    }



    // special functions



    /**
     * Calculates the autocorrelation sequence of `input`
     */
    inline void autocorrelation(sampled_t &input,
                                sampled_t &output)
    {
        assert(input.count > output.count);
        assert(abs(input.period - output.period) < 1e-15);
        size_t n = input.count, m = output.count;
        for (size_t k = 0; k < m; k++)
        {
            output.samples[k] = 0;
            for (size_t i = 0; i < n - k; i++)
            {
                output.samples[k] += input.samples[i] * input.samples[i + k];
            }
            output.samples[k] /= (n - k);
        }
    }

    /**
     * Calculates the correlation sequence of `input1` and `input2`
     *
     * Parameters:
     *      reflect - reflect `input2` sequence
     */
    inline std::pair<size_t, double> correlation(
        sampled_t &input1,
        sampled_t &input2,
        sampled_t &output,
        bool reflect = false)
    {
        assert(input1.count == input2.count);
        assert(input1.count >= output.count);
        assert(abs(input1.period - input2.period) < 1e-15);
        assert(abs(input1.period - output.period) < 1e-15);
        size_t n = input1.count, m = output.count;
        double max = (std::numeric_limits<double>::min)(); size_t max_idx = 0;
        for (size_t k = 0; k < m; k++)
        {
            output.samples[k] = 0;
            for (size_t i = 0; i < n; i++)
            {
                size_t idx = (reflect ? (n - i - 1) : i);
                output.samples[k] += input1.samples[i] * input2.samples[(idx + k) % n];
            }
            output.samples[k] /= (n);
            if (output.samples[k] > max)
            {
                max = output.samples[k];
                max_idx = k;
            }
        }
        return { max_idx, max };
    }


    /**
     * Calculates cyclic convolution of `f` and `g`.
     *
     * Returns:
     *      The power of the convolution signal
     */
    inline double convolve(sampled_t &f,
                           sampled_t &g,
                           sampled_t &output,
                           un_op_t mapper = identity_un_op())
    {
        assert(f.count == g.count);
        assert(f.count <= output.count);
        assert(abs(f.period - g.period) < 1e-15);
        assert(abs(f.period - output.period) < 1e-15);
        size_t n = f.count;
        double power = 0;
        for (size_t i = 0; i < n; i++)
        {
            output.samples[i] = 0;
            for (size_t j = 0; j < n; j++)
            {
                int conv_idx = ((int) i - (int) j);
                if (conv_idx < 0) conv_idx = (n + conv_idx);
                output.samples[i] += f.samples[j] * g.samples[conv_idx];
            }
            output.samples[i] = mapper(i, output.samples[i]);
            power += output.samples[i] * output.samples[i];
        }
        return power;
    }

    /**
     * Calculates `i`-th sample of the cyclic
     * convolution of `f` and `g`.
     *
     * Returns:
     *      The `i`-th sample of convolution
     */
    inline double convolve(sampled_t &f,
                           sampled_t &g,
                           size_t i)
    {
        assert(f.count == g.count);
        assert(abs(f.period - g.period) < 1e-15);
        double result = 0;
        for (size_t j = 0; j < f.count; j++)
        {
            int conv_idx = ((int) i - (int) j);
            if (conv_idx < 0) conv_idx = (f.count + conv_idx);
            result += f.samples[j] * g.samples[conv_idx];
        }
        return result;
    }




    // utility



    inline void sort_all(double **arrays, size_t n_arrays, size_t n_elements, int order = 1 /* lower */)
    {
        assert(n_arrays > 0);
        double *keys = arrays[0];
        for (size_t i = 0; i < n_elements; i++)
        {
            for (size_t j = i + 1; j < n_elements; j++)
            {
                if (order * (keys[i] - keys[j]) > 0)
                {
                    for (size_t k = 0; k < n_arrays; k++)
                    {
                        std::swap(arrays[k][i], arrays[k][j]);
                    }
                }
            }
        }
    }
}
