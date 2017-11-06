#pragma once

#include <functional>

#include <util/common/math/vec.h>

namespace math
{

    template < typename _v3 = v3 < > >
    using dfunc3_t = std::function < _v3 (double t, const _v3 &x) >;

    template < typename _v3 = v3 < > >
    using dfunc3s_t = std::function < _v3 (double t, const _v3 &x, const _v3 &dx) >;

    template < typename _v3 = v3 < > >
    struct dresult3
    {
        double t;
        _v3 x;
    };

    template < typename _v3 = v3 < > >
    struct dresult3s
    {
        double t;
        _v3 x, dx;
    };

    template < typename _v3 = v3 < > >
    struct rk4_coefs3
    {
        _v3 k1, k2, k3, k4;
    };

    template < typename _v3 = v3 < > >
    inline rk4_coefs3 < _v3 > _get_rk4_coefs3(const dfunc3_t < _v3 > &fn, double t, double dt, const _v3 &x)
    {
        rk4_coefs3 < _v3 > c;
        c.k1 = fn(t, x) * dt;
        c.k2 = fn(t + dt / 2, x + c.k1 / 2) * dt;
        c.k3 = fn(t + dt / 2, x + c.k2 / 2) * dt;
        c.k4 = fn(t + dt, x + c.k3) * dt;
        return c;
    }

    template < typename _v3 = v3 < > >
    inline bool _get_rk4_coefs3a(const dfunc3_t < _v3 > &fn, double t, double dt, const _v3 &x, double max_eps, rk4_coefs3 < _v3 > & c)
    {
        c.k1 = fn(t, x) * dt;
        c.k2 = fn(t + dt / 2, x + c.k1 / 2) * dt;
        if ((sqnorm(c.k1 - c.k2) / (1e-16 + sqnorm(c.k1 + c.k2))) > max_eps * max_eps) return false;
        c.k3 = fn(t + dt / 2, x + c.k2 / 2) * dt;
        if ((sqnorm(c.k1 - c.k3) / (1e-16 + sqnorm(c.k1 + c.k3))) > max_eps * max_eps) return false;
        c.k4 = fn(t + dt, x + c.k3) * dt;
        if ((sqnorm(c.k1 - c.k4) / (1e-16 + sqnorm(c.k1 + c.k4))) > max_eps * max_eps) return false;
        return true;
    }

    template < typename _v3 = v3 < > >
    inline rk4_coefs3 < _v3 > _get_rk4_coefs3s(const dfunc3s_t < _v3 > &fn, double t, double dt, const _v3 &x, const _v3 &dx)
    {
        rk4_coefs3 < _v3 > c;
        c.k1 = fn(t, x, dx) * dt;
        c.k2 = fn(t + dt / 2, x + dx * dt / 2, dx + c.k1 / 2) * dt;
        c.k3 = fn(t + dt / 2, x + dx * dt / 2 + c.k1 / 4 * dt, dx + c.k2 / 2) * dt;
        c.k4 = fn(t + dt, x + dx * dt + c.k2 / 2 * dt, dx + c.k3) * dt;
        return c;
    }

    // solves the passed vector differential equation
    // of 2nd order using Runge-Kutta (4) method
    // fn - the vector function
    // t  - the scalar parameter (time)
    // dt - the scalar parameter step (time delta)
    // x  - the initial condition
    // dx - the initial condition for the derivative
    template < typename _v3 = v3 < > >
    inline dresult3s < _v3 > rk4_solve3s(const dfunc3s_t < _v3 > & fn, double t, double dt, const _v3 &x, const _v3 &dx)
    {
        rk4_coefs3 < _v3 > c = _get_rk4_coefs3s < _v3 > (fn, t, dt, x, dx);
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
    template < typename _v3 = v3 < > >
    inline dresult3 < _v3 > rk4_solve3(const dfunc3_t < _v3 > & fn, double t, double dt, const _v3 &x)
    {
        rk4_coefs3 < _v3 > c = _get_rk4_coefs3 < _v3 > (fn, t, dt, x);
        return
        {
            t + dt,
            x + (c.k1 + 2 * c.k2 + 2 * c.k3 + c.k4) / 6
        };
    }

    // solves the passed vector differential equation
    // using Runge-Kutta (4) method with adaptive step
    // fn - the vector function
    // t  - the scalar parameter (time)
    // dt - the scalar parameter step (time delta)
    // x  - the initial condition
    // dt_min - the minimal possible parameter step
    // eps - the max allowed relative change of the `fn`:
    //       `norm(fn(a) - fn(b)) / norm(fn(a) + fn(b)) < eps`
    template < typename _v3 = v3 < > >
    inline dresult3 < _v3 > rk4_solve3a(const dfunc3_t < _v3 > & fn, double t, double dt, const _v3 &x, double dt_min, double eps)
    {
        rk4_coefs3 < _v3 > c;
        bool success;
        dt *= 10;
        do
        {
            dt /= 10;
            success = _get_rk4_coefs3a < _v3 > (fn, t, dt, x, eps, c);
        } while ((dt > dt_min) && (!success));
        return
        {
            t + dt,
            x + (c.k1 + 2 * c.k2 + 2 * c.k3 + c.k4) / 6
        };
    }

    // solves the passed vector differential equation
    // at the given interval using Runge-Kutta (4) method
    // fn - the vector function
    // t1 - the scalar parameter (time) - interval start
    // t2 - the scalar parameter (time) - interval end
    // dt - the scalar parameter step (time delta), always positive
    // x  - the initial condition
    // note: t1 may be greater than t2, but dt must always be positive
    template < typename _v3 = v3 < > >
    inline dresult3 < _v3 > rk4_solve3i(const dfunc3_t < _v3 > & fn, double t1, double t2, double dt, const _v3 &x)
    {
        size_t iters = (size_t)(abs(t1 - t2) / dt);
        if (t1 > t2) { dt = -dt; }
        dresult3 < _v3 > r = { t1, x };
        for (size_t i = 0; i < iters; ++i)
        {
            r = rk4_solve3(fn, r.t, dt, r.x);
            r.t = t1 + (i + 1) * dt; // to minimize rounding errors
        }
        return r;
    }

    // solves the passed vector differential equation
    // at the given interval using Runge-Kutta (4) method
    // with adaptive step
    // fn - the vector function
    // t1 - the scalar parameter (time) - interval start
    // t2 - the scalar parameter (time) - interval end
    // dt - the scalar parameter step (time delta), always positive
    // x  - the initial condition
    // note: t1 may be greater than t2, but dt must always be positive
    // dt_min - the minimal possible parameter step
    // eps - the max allowed relative change of the `fn`:
    //       `norm(fn(a) - fn(b)) / norm(fn(a) + fn(b)) < eps`
    template < typename _v3 = v3 < > >
    inline dresult3 < _v3 > rk4_solve3ia(const dfunc3_t < _v3 > & fn, double t1, double t2, double dt, const _v3 &x, double dt_min, double eps)
    {
        if (t1 > t2) { dt = -dt; }
        dresult3 < _v3 > r = { t1, x };
        while ((t1 > t2) ? (r.t > t2) : (r.t < t2))
        {
            r = rk4_solve3a(fn, r.t, dt, r.x, dt_min, eps);
        }
        return r;
    }

    // solves the passed vector differential equation
    // of 2nd order at the given interval using Runge-Kutta (4) method
    // fn - the vector function
    // t1 - the scalar parameter (time) - interval start
    // t2 - the scalar parameter (time) - interval end
    // dt - the scalar parameter step (time delta), always positive
    // x  - the initial condition
    // dx - the initial condition for the derivative
    // note: t1 may be greater than t2, but dt must always be positive
    template < typename _v3 = v3 < > >
    inline dresult3s < _v3 > rk4_solve3si(const dfunc3s_t < _v3 > & fn, double t1, double t2, double dt, const _v3 &x, const _v3 &dx)
    {
        size_t iters = (size_t)(abs(t1 - t2) / dt);
        if (t1 > t2) { dt = -dt; }
        dresult3s < _v3 > r = { t1, x, dx };
        for (size_t i = 0; i < iters; ++i)
        {
            r = rk4_solve3s(fn, r.t, dt, r.x, r.dx);
            r.t = t1 + (i + 1) * dt; // to minimize rounding errors
        }
        return r;
    }
}
