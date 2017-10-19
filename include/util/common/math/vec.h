#pragma once

#include <cmath>

#include <util/common/math/scalar.h>

namespace math
{

    /*****************************************************/
    /*                       v3                          */
    /*****************************************************/

    template < typename _data_t = double >
    struct v3
    {

    	_data_t x, y, z;

        template < typename _second_t >
        v3 (const v3 < _second_t > & source)
            : x(static_cast < _data_t > (source.x))
            , y(static_cast < _data_t > (source.y))
            , z(static_cast < _data_t > (source.z))
        {
        }

        template < typename _second_t >
        v3 (_second_t x, _second_t y = {}, _second_t z = {})
            : x(static_cast < _data_t > (x))
            , y(static_cast < _data_t > (y))
            , z(static_cast < _data_t > (z))
        {
        }

        v3 ()
            : x({}), y({}), z({})
        {
        }

    	const _data_t & operator[] (size_t i) const
    	{
    		return (i == 0) ? x : ((i == 1) ? y : z);
    	}

    	_data_t & operator[] (size_t i)
    	{
    		return (i == 0) ? x : ((i == 1) ? y : z);
    	}

        template < size_t i >
        const _data_t & at() const;

        template < size_t i >
        _data_t & at();

        template < > const _data_t & at < 0 > () const { return x; }
        template < > const _data_t & at < 1 > () const { return y; }
        template < > const _data_t & at < 2 > () const { return z; }

        template < > _data_t & at < 0 > () { return x; }
        template < > _data_t & at < 1 > () { return y; }
        template < > _data_t & at < 2 > () { return z; }
    };

    template < typename _data_t >
    inline double norm(const v3 < _data_t > &first)
    {
        return std::sqrt((sqnorm(first.x) + sqnorm(first.y) + sqnorm(first.z)));
    }

    template < typename _data_t >
    inline double sqnorm(const v3 < _data_t > &first)
    {
        return (sqnorm(first.x) + sqnorm(first.y) + sqnorm(first.z));
    }

    template < typename _data_t >
    inline v3 < _data_t > conjugate(const v3 < _data_t > &first)
    {
        return { conjugate(first.x), conjugate(first.y), conjugate(first.z) };
    }

    template < typename _data_t, typename _second_t >
    inline v3 < _data_t > operator+(const v3 < _data_t > &first, const v3 < _second_t > &second)
    {
    	return{ first.x + second.x, first.y + second.y, first.z + second.z};
    }

    template < typename _data_t, typename _second_t >
    inline v3 < _data_t > operator-(const v3 < _data_t > &first, const v3 < _second_t > &second)
    {
    	return{ first.x - second.x, first.y - second.y, first.z - second.z };
    }

    template < typename _data_t >
    inline v3 < _data_t > operator-(const v3 < _data_t > &first)
    {
    	return{ -first.x, -first.y, -first.z };
    }

    template < typename _data_t, typename _second_t >
    inline v3 < _data_t > operator*(const v3 < _data_t > &first, _second_t n)
    {
    	return{ n * first.x, n * first.y, n * first.z };
    }

    template < typename _data_t, typename _second_t >
    inline v3 < _data_t > operator*(_second_t n, const v3 < _data_t > &first)
    {
    	return first * n;
    }

    // cross product
    template < typename _data_t, typename _second_t >
    inline v3 < _data_t > operator^(const v3 < _data_t > &first, const v3 < _second_t > &second)
    {
    	return{
    		first.y * second.z - first.z * second.y,
    		first.z * second.x - first.x * second.z,
    		first.x * second.y - first.y * second.x
    	};
    }

    // dot product
    template < typename _data_t, typename _second_t >
    inline _data_t operator*(const v3 < _data_t > &first, const v3 < _second_t > &second)
    {
        v3 < _second_t > cjg = conjugate(second);
    	return first.x * cjg.x + first.y * cjg.y + first.z * cjg.z;
    }

    template < typename _data_t, typename _second_t >
    inline v3 < _data_t > operator/(const v3 < _data_t > &first, _second_t n)
    {
    	return{ first.x / n, first.y / n, first.z / n };
    }

    // Performs transformation from (r, theta, phi) to (x, y, z)
    // where r >= 0, theta in [-pi/2, pi/2], phi in [-pi, pi]
    template < typename _data_t >
    inline v3 < _data_t > spherical2rect(const v3 < _data_t > &polar, bool deg = false)
    {
    	_data_t m = static_cast < _data_t > (deg ? (std::acos(0) * 2) / 180 : 1);
    	return{
    		polar.x * std::cos(m * polar.y) * std::cos(m * polar.z),
    		polar.x * std::cos(m * polar.y) * std::sin(m * polar.z),
    		polar.x * std::sin(m * polar.y)
    	};
    }

    // Performs transformation to (r, theta, phi) from (x, y, z)
    template < typename _data_t >
    inline v3 < _data_t > rect2spherical(const v3 < _data_t > &rect, bool deg = false)
    {
    	_data_t m = static_cast < _data_t > (deg ? 180 / (std::acos(0) * 2) : 1);
    	double r = norm(rect);
    	return{
    		// >= 0
    		r,
    		// in [-pi/2, pi/2] as r >= 0 that gives atan2(y <=> 0, x >= 0)
    		m * std::atan2(rect.z, std::sqrt(rect.x * rect.x + rect.y * rect.y)),
    		// in [-pi, pi] as that gives atan2(y <=> 0, x <=> 0)
    		m * std::atan2(rect.y, rect.x)
    	};
    }

    /*****************************************************/
    /*                       m3                          */
    /*****************************************************/

    template < typename _data_t = double >
    using m3 = v3 < v3 < _data_t > > ;

    // Matrix * Vector
    template < typename _data_t, typename _second_t >
    inline v3 < _data_t > operator*(const m3 < _data_t > &first, const v3 < _second_t > &second)
    {
    	return
    	{
    		first.at<0>().x * second.x + first.at<0>().y * second.y + first.at<0>().z * second.z,
    		first.at<1>().x * second.x + first.at<1>().y * second.y + first.at<1>().z * second.z,
    		first.at<2>().x * second.x + first.at<2>().y * second.y + first.at<2>().z * second.z
    	};
    }

    // Matrix */ Number
    template < typename _data_t, typename _second_t >
    inline m3 < _data_t > operator*(const m3 < _data_t > &first, _second_t a)
    {
    	return{ first.at<0>() * a, first.at<1>() * a, first.at<2>() * a };
    }

    template < typename _data_t, typename _second_t >
    inline m3 < _data_t > operator*(_second_t a, const m3 < _data_t > &first)
    {
    	return first * a;
    }

    template < typename _data_t, typename _second_t >
    inline m3 < _data_t > operator/(const m3 < _data_t > &first, _second_t a)
    {
    	return{ first.at<0>() / a, first.at<1>() / a, first.at<2>() / a };
    }

    // transpose
    template < typename _data_t >
    inline m3 < _data_t > operator~(const m3 < _data_t > &first)
    {
    	return
    	{
    		{ first.at<0>().x, first.at<1>().x, first.at<2>().x },
    		{ first.at<0>().y, first.at<1>().y, first.at<2>().y },
    		{ first.at<0>().z, first.at<1>().z, first.at<2>().z },
    	};
    }

    // invert
    template < typename _data_t >
    inline m3 < _data_t > operator!(const m3 < _data_t > &first)
    {
    	// 0. Inverse matrix is given as A^-1 = adj(A) / det(A)
    	// where adj(A) is adjugate matrix:
    	// adj(A){j,i} = { [(-1)^(i+j)] * M{i,j} },
    	// M{i,j} is an (i,j) minor.
    	m3 < _data_t > inv; double det = 0;
    	// 1. Calculate adj(A){j,i} and accumulate det(A)
    	for (int i = 0; i < 3; i++) // rows
    	{
    		for (int j = 0; j < 3; j++) // cols
    		{
    			// 1.1 Calculate M{i,j} of matrix m3\{i,j} -- 2x2 minor
    			// Using a trick
    			double det2[2] = { 1, 1 }; int d = 0;
    			for (int k1 = 0; k1 < 3; k1++) // rows
    			{
    				if (k1 == i) continue;
    				for (int k2 = 0; k2 < 3; k2++) // cols
    				{
    					if (k2 == j) continue;
    					det2[d % 2] *= first[k1][k2];
    					d++;
    				}
    				d = 1;
    			}
    			// 1.2 Calculate (-1)^(i+j)*M{i,j}
    			inv[j][i] = (((i + j) % 2) == 0 ? 1 : -1)
    				* (det2[0] - det2[1]);
    			// 1.3 Accumulate det(m3) -- 1st row expansion
    			if (i == 0)
    			{
    				det += (((i + j) % 2) == 0 ? 1 : -1)
    					* first[i][j] * (det2[0] - det2[1]);
    			}
    		}
    	}
    	// 2. Return result
    	return inv / det;
    }

    // matrix product
    template < typename _data_t, typename _second_t >
    inline m3 < _data_t > operator*(const m3 < _data_t > &first, const m3 < _data_t > &second)
    {
    	auto t = ~second;
    	return
    	{
    		{ first.at<0>() * t.at<0>(), first.at<0>() * t.at<1>(), first.at<0>() * t.at<2>() },
    		{ first.at<1>() * t.at<0>(), first.at<1>() * t.at<1>(), first.at<1>() * t.at<2>() },
    		{ first.at<2>() * t.at<0>(), first.at<2>() * t.at<1>(), first.at<2>() * t.at<2>() }
    	};
    }

    template < typename _data_t = double >
    inline m3 < _data_t > identity()
    {
    	return{
    			{ _data_t(1), _data_t(0), _data_t(0) },
    			{ _data_t(0), _data_t(1), _data_t(0) },
    			{ _data_t(0), _data_t(0), _data_t(1) }
    	};
    }

    template < typename _data_t = double >
    inline m3 < _data_t > rotate_x(double angle)
    {
    	return{
    			{ 1, 0, 0 },
    			{ 0, std::cos(angle), - std::sin(angle) },
    			{ 0, std::sin(angle), std::cos(angle) }
    	};
    }

    template < typename _data_t = double >
    inline m3 < _data_t > rotate_y(double angle)
    {
    	return{
    			{ std::cos(angle), 0, std::sin(angle) },
    			{ 0, 1, 0 },
    			{ - std::sin(angle), 0, std::cos(angle) }
    	};
    }

    template < typename _data_t = double >
    inline m3 < _data_t > rotate_z(double angle)
    {
    	return{
    			{ std::cos(angle), - std::sin(angle), 0 },
    			{ std::sin(angle), std::cos(angle), 0 },
    			{ 0, 0, 1 }
    	};
    }

    // Perform the given transformation in the specified base
    // Suppose that the base consists of rows,
    // each is a base vector.
    template < typename _data_t, typename _second_t >
    inline m3 < _data_t > transform(const m3 < _data_t > &base, const m3 < _second_t > &transform)
    {
    	auto base_t = ~base;
    	return base_t * transform * !base_t;
    }

    // Calculate transformation to align the given axis
    // with the given direction. Or, in other words, calculate
    // rotation matrix to rotate the axis to the given direction
    // in orthonormal base.
    template < typename _data_t, typename _second_t >
    inline m3 < _data_t > align_axe(const v3 < _data_t > &axe, const v3 < _second_t > &direction)
    {
    	v3 < _data_t > ax_n = axe / norm(axe); // normalized
    	v3 < _data_t > d_n = direction / norm(direction); // normalized
    	// normal vector -- the rotation axis
    	v3 < _data_t > n_n = (direction ^ axe) / norm(direction ^ axe); // normalized

    	// cosine of the angle between the axis and the direction
    	double alpha = ax_n * d_n;

    	// normal vector to two others
    	v3 < _data_t > v_n = d_n - alpha * ax_n;
    	v_n = v_n / norm(v_n); // normalized

    	m3 < _data_t > base = { ax_n, v_n, n_n };

    	return transform(base, rotate_z(std::acos(alpha)));
    }

    // Calculate transformation to align the given axis
    // with the given directions. Or, in other words, calculate
    // a complete transformation matrix to rotate the given axis
    // to the given directions.
    template < typename _data_t, typename _second_t >
    inline m3 < _data_t > align_axis(const m3 < _data_t > &base0, int axe1, int axe2,
    	const v3 < _second_t > &direction1, const v3 < _second_t > &direction2)
    {
    	// Rotate the axis1 to the direction1 first
    	m3 < _data_t > align_m1 = align_axe(base0[axe1], direction1);
    	// Go into the new base
    	m3 < _data_t > aligned_base = ~(align_m1 * (~base0));

    	// Calculate rotation
    	double align_2_rot_angle =
    		// If triple product is positive
    		(((aligned_base[axe2] ^ direction2) * aligned_base[axe1]) > 0 ? 1 : -1) *
    		std::acos(aligned_base[axe2] * direction2 / norm(direction2));

    	return transform(aligned_base, rotate_z(align_2_rot_angle)) * align_m1;
    }
}
