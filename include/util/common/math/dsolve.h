#pragma once

#include <functional>

#include <util/common/math/vec.h>

namespace math
{

    using dfunc3_t = std::function < v3 (double t, const v3 &x) >;

    using dfunc3s_t = std::function < v3 (double t, const v3 &x, const v3 &dx) >;

    struct dresult3
    {
        double t;
        v3 x;
    };

    struct dresult3s
    {
        double t;
        v3 x, dx;
    };

    struct rk4_coefs3
    {
        v3 k1, k2, k3, k4;
    };

    inline rk4_coefs3 _get_rk4_coefs3(const dfunc3_t &fn, double t, double dt, const v3 &x)
    {
        rk4_coefs3 c;
        c.k1 = fn(t, x) * dt;
        c.k2 = fn(t + dt / 2, x + c.k1 / 2) * dt;
        c.k3 = fn(t + dt / 2, x + c.k2 / 2) * dt;
        c.k4 = fn(t + dt, x + c.k3) * dt;
        return c;
    }

    inline rk4_coefs3 _get_rk4_coefs3s(const dfunc3s_t &fn, double t, double dt, const v3 &x, const v3 &dx)
    {
        rk4_coefs3 c;
        c.k1 = fn(t, x, dx) * dt;
        c.k2 = fn(t + dt / 2, x + dx * dt / 2, dx + c.k1 / 2) * dt;
        c.k3 = fn(t + dt / 2, x + dx * dt / 2 + c.k1 / 4 * dt, dx + c.k2 / 2) * dt;
        c.k4 = fn(t + dt, x + dx * dt + c.k2 / 2 * dt, dx + c.k3) * dt;
        return c;
    }

    // solves the passed vector differential equation
    // using Runge-Kutta (4) method
    // fn - the vector function
    // t  - the scalar parameter (time)
    // dt - the scalar parameter step (time delta)
    // x  - the initial condition
    // dx - the initial condition for the derivative
    inline dresult3s rk4_solve3s(dfunc3s_t fn, double t, double dt, const v3 &x, const v3 &dx)
    {
        rk4_coefs3 c = _get_rk4_coefs3s(fn, t, dt, x, dx);
        return
        {
            t + dt,
            x + dx * dt + (c.k1 + c.k2 + c.k3) / 6 * dt,
            dx + (c.k1 + 2 * c.k2 + 2 * c.k3 + c.k4) / 6
        };
    }

    // solves the passed vector differential equation
    // using Runge-Kutta (4) method
    // fn - the vector function
    // t  - the scalar parameter (time)
    // dt - the scalar parameter step (time delta)
    // x  - the initial condition
    inline dresult3 rk4_solve3(dfunc3s_t fn, double t, double dt, const v3 &x)
    {
        rk4_coefs3 c = _get_rk4_coefs3(fn, t, dt, x);
        return
        {
            t + dt,
            x + (c.k1 + 2 * c.k2 + 2 * c.k3 + c.k4) / 6
        };
    }
}
