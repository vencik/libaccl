#ifndef libaccl__pattern__points_hxx
#define libaccl__pattern__points_hxx

/**
 *  \file
 *  \brief  Pattern of points
 *
 *  Points in N-dimensional discrete Euclidean space.
 *
 *  \date   2015/12/10
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

#include <vector>
#include <map>
#include <stdexcept>
#include <cstdlib>


namespace libaccl {
namespace pattern {

/**
 *  \brief  Pattern of points
 *
 *  Set of points.
 *
 *  \tparam  Base_t     Base numeric type (integral)
 *  \tparam  Payload_t  Point payload type
 */
template <typename Base_t, typename Payload_t>
class points {
    public:

    typedef std::vector<Base_t>          point_t;  /**< Point coordinates   */
    typedef std::map<point_t, Payload_t> set_t;    /**< Implementation type */

    private:

    set_t m_impl;  /**< Set of pattern points */

    protected:

    /**
     *  \brief  Add point to set
     *
     *  \param  point    Point coordinates (N-dimensional vector)
     *  \param  payload  Point payload
     */
    void set(
        const point_t   & point,
        const Payload_t & payload = Payload_t())
    {
        m_impl.emplace(point, payload);
    }

    public:

    /** Set size */
    size_t size() const { return m_impl.size(); }

    /** Pattern access */
    operator const set_t & () const { return m_impl; }

    /** Begin const iterator */
    typename set_t::const_iterator begin() const { return m_impl.begin(); }

    /** End const iterator */
    typename set_t::const_iterator end() const { return m_impl.end(); }

    /** Point getter (const) */
    typename set_t::const_iterator get(const point_t & point) const {
        return m_impl.find(point);
    }

    /** Point is in the set */
    friend bool operator & (const point_t & point, const points & set) {
        return set.end() != set.get(point);
    }

    /**
     *  \brief  Point payload getter (const)
     *
     *  The function provides point payload.
     *  If the point specified is not in the set, an exception is thrown.
     */
    const Payload_t & get_payload(const point_t & point) const {
        auto x = get(point);
        if (end() == x)
            throw std::runtime_error(
                "libaccl::points::get_payload: "
                "no such point");

        return x->second;
    }

};  // end of template class points

}}  // end of namespace libaccl::pattern

#endif  // end of #ifndef libaccl__pattern__points_hxx
