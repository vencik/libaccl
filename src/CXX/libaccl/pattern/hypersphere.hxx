#ifndef libaccl__pattern__hypersphere_hxx
#define libaccl__pattern__hypersphere_hxx

/**
 *  \file
 *  \brief  Hypersphere pattern
 *
 *  The algorithm below produces points in N-dimensional discrete
 *  Euclidean space that belong to an N-dimensional sphere (aka hypersphere)
 *  with specified centre and radius.
 *
 *  The algorithm is based on the midpoint circle algorithm.
 *  The key idea is that in N-dimensional discrete Euclidean space,
 *  N-dimensional hypersphere with centre C and radius R consists of finite
 *  number of (N-1)-dimensional hyperspherical "slices" with centres
 *  within interval C_d +- R (in dimension d) and radii computed by
 *  the midpoint circle algorithm.
 *
 *  An example in 2D (1st quadrant only):
 *
 *  \code
 *
 *  [][][][]                    slice 11, radius  3
 *  [][][][][][]                slice 10, radius  5
 *  [][][][][][][][]            slice  9, radius  7
 *  [][][][][][][][][]          slice  8, radius  8
 *  [][][][][][][][][][]        slice  7, radius  9
 *  [][][][][][][][][][]        slice  6, radius  9
 *  [][][][][][][][][][][]      slice  5, radius 10
 *  [][][][][][][][][][][]      slice  4, radius 10
 *  [][][][][][][][][][][][]    slice  3, radius 11
 *  [][][][][][][][][][][][]    slice  2, radius 11
 *  [][][][][][][][][][][][]    slice  1, radius 11
 *  [][][][][][][][][][][][]    slice  0, radius 11
 *  0 1 2 3 4 5 6 7 8 9 10 11
 *
 *  \endcode
 *
 *  The points on perimeter of the circle above are computed by the midpoint
 *  algorithm.
 *  Now, if you imagine that each of the slices above is a circle
 *  (in dimension Z, with radius defined by distance of the perimeter point
 *  to the vertical axis), you get points of 3D sphere with radius 11.
 *  This approach is applicable in higher dimensions, too.
 *
 *  Note that 1D circle is a mere line (the slice in the 2D exampe above).
 *  In 0D, i.e. Euclidean space containing one single point, the point
 *  itself is a 0D hypersphere (with radius 0).
 *
 *  Based on the idea illustrated above, the algorithm goes as follows:
 *  1/ For dimension d, get points C_d from range C +- R as centres of
 *     (N-1)-dimensional hyperspherical slices of the resulting N-dimensional
 *     hypersphere
 *  2/ If in 1D, return the points above (recursion fixed point)
 *  3/ For each slice above, compute radius using the midpoint circle algorithm
 *     (as if drawing a 2D circle in dimensions d and d')
 *  4/ Apply the algorithm recursively to all the slices above in N-1 dimensions
 *     (over dimension d' for step 1)
 *  5/ Return union of the results obtained in previous step
 *
 *  See https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
 *
 *  \date   2015/12/05
 *  \author Vaclav Krpec  <vencik@razdva.cz>
 *
 *
 *  LEGAL NOTICE
 *
 *  Copyright (c) 2015, Vaclav Krpec
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the distribution.
 *
 *  3. Neither the name of the copyright holder nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
 *  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "libaccl/pattern/points.hxx"


namespace libaccl {
namespace pattern {

/**
 *  \brief  Hypersphere pattern
 *
 *  Hypersphere with centre in 0 in (N-dimensional) discrete Euclidean space.
 *  Radius is specified as a parameter.
 *  The hypersphere may be hollow and/or layered; one may specify thickness
 *  of each layer (circumference-to-centre).
 *
 *  \tparam  Base_t  Base numeric type (integral)
 */
template <typename Base_t>
class hypersphere: public points<Base_t> {
    private:

    /**
     *  \brief  Compute hypersphere points
     *
     *  \param  centre  Hypersphere centre
     *  \param  radius  Hypersphere radius
     *  \param  d       Slicing dimension
     */
    void compute(
        const std::vector<Base_t> & centre,
        const Base_t              & radius,
        size_t                      d = 0)
    {
        assert(0 <= radius);
        assert(d < centre.size());

        // 1D, create layers
        if (centre.size() - 1 == d) {
            auto slice_point = centre;
            for (Base_t d_diff = radius; d_diff > 0; ++d_diff) {
                // Right side
                slice_point[d] = centre[d] + d_diff;
                point(slice_point);

                // Left side
                slice_point[d] = centre[d] - d_diff;
                point(slice_point);
            }

            point(centre);

            return;  // recursion fixed point
        }

        // Midpoint circle algorithm for slice radius computation
        Base_t slice_radius = radius;
        Base_t d_diff       = 0;
        Base_t criterion    = 1 - slice_radius;

        auto slice_centre = centre;
        while (d_diff <= slice_radius) {
            // 1st octant
            slice_centre[d] = centre[d] + d_diff;
            compute(slice_centre, slice_radius, d + 1);

            // 2nd octant (by diagonal symmetry)
            slice_centre[d] = centre[d] + slice_radius;
            compute(slice_centre, d_diff, d + 1);

            // 8th octant
            slice_centre[d] = centre[d] - d_diff;
            compute(slice_centre, slice_radius, d + 1);

            // 7th octant (by diagonal symmetry)
            slice_centre[d] = centre[d] - slice_radius;
            compute(slice_centre, d_diff, d + 1);

            Base_t delta = ++d_diff;
            if (criterion > 0) delta -= --slice_radius;
            delta <<= 2;
            criterion += delta + 1;
        }
    }

    public:

    /**
     *  \brief  Constructor
     *
     *  \param  dimension  Space dimension
     *  \param  radius     Hypersphere radius
     */
    hypersphere(const Base_t & radius) {
        const std::vector<Base_t> zero(dimension, 0);
        compute(zero, radius);
    }

};  // end of template class hypersphere

}}  // end of namespace libaccl::pattern

#endif  // end of #ifndef libaccl__pattern__hypersphere_hxx
