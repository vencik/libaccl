#ifndef libaccl__hash__linear_hxx
#define libaccl__hash__linear_hxx

/**
 *  \file
 *  \brief  Linear hashtable
 *
 *  \date   2015/12/28
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
#include <list>
#include <initializer_list>
#include <stdexcept>
#include <cstdlib>


namespace libaccl {
namespace hash {

namespace impl {

/** Self-keyed hashtable item key accessor */
template <typename Item_t>
class identity_key {
    public:

    inline const Item_t & operator () (const Item_t & item) const {
        return item;
    }

};  // end of template class identity_key

}  // end of namespace impl


/**
 *  \brief  Hashtable with linear collision resolution
 *
 *  The table is implemented as a vector of item slots.
 *  Each slot is either empty (not used ever), available (previously used, but
 *  not any more) or currently used.
 *  Slot index is calculated by (multiple) hash functions (as long as a function
 *  returns index of a slot in use).
 *  If all hash functions fail to find an unused slot then the next one in row
 *  that is unused is taken for the collision resolution.
 *  Note that it is recommended to use at least two hash functions.
 *
 *  Items need keys which may or may not be part of them (even themselves).
 *  The \c Key_fn is used to access an item key.
 *  Note that the key must be available throughout the table item life; however,
 *  it may be reconstructed on demand (although that may not be much efficient).
 *  Keys must be comparable using \c == operator.
 *
 *  Note that the \c Item_t item type must have default constructor and is NOT
 *  destroyed on removal from table.
 *  It should be considered more a handle, unless it is a primitive type.
 *  Use pointers or iterators to aggregate objects.
 *
 *  \tparam  Item_t   Item type (must be comparable with \c ==)
 *  \tparam  Hash_fn  Hash functor, accepts \c Key_t and \c size_t (table size)
 *  \tparam  Key_t    Key type (the item type by default)
 *  \tparam  Key_fn   Key accessor (item identity by default)
 */
template <
    typename Item_t,
    class    Hash_fn,
    typename Key_t   = Item_t,
    class    Key_fn  = impl::identity_key<Item_t> >
class linear {
    private:

    /** Table slot */
    struct slot {
        /** Table slot type */
        enum type_t {
            EMPTY = 0,  /**< Empty slot (unused yet)          */
            AVAIL,      /**< Available slot (previously used) */
            USED        /**< Slot in use                      */
        };  // end of enum type_t

        type_t type;  /**< Slot type        */
        Item_t item;  /**< Item in the slot */

        /** Constructor */
        slot(): type(EMPTY) {}

        /** Item assignment */
        Item_t & operator = (const Item_t & it) {
            type = USED;
            return item = it;
        }

    };  // end of struct slot

    std::vector<Hash_fn> m_hash_fn;   /**< Hash functors      */
    std::vector<slot>    m_tab;       /**< Hash table         */
    size_t               m_item_cnt;  /**< Current item count */
    const size_t         m_capacity;  /**< Table capacity     */
    const Key_fn         m_key_fn;    /**< Key accessor       */

    /**
     *  \brief  Check item indices
     *
     *  The function implements \ref get_index indices resolution.
     *
     *  \param  key     Item key
     *  \param  index   Current index
     *  \param  insert  Insert index
     *  \param  find    Search index
     *
     *  \return \c true iff required indices are set (\ref get_index may return)
     */
    bool check_index(
        const Key_t & key,
        size_t        index,
        ssize_t *   & insert,
        ssize_t *   & find)
    const {
        switch (m_tab[index].type) {
            case slot::EMPTY:
                if (insert) *insert = index;  // insert to empty slot
                if (find)   *find   = -1;     // had been here if present

                return 1;  // that's it

            case slot::AVAIL:
                if (insert) *insert = index;  // insert to available slot

                if (!find) return 1;  // no reason to continue

                // Continue looking for the item
                insert = NULL;
                break;

            case slot::USED:
                if (m_key_fn(m_tab[index].item) == key) {
                    if (insert) *insert = -1;     // already exists
                    if (find)   *find   = index;  // gotcha!

                    return 1;  // all done
                }
        }

        return 0;  // continue
    }

    /**
     *  \brief  Find item index
     *
     *  The function unifies computation of insert and search index.
     *  It is capable of efficiently finding either or both.
     *
     *  \param  key     Item key
     *  \param  insert  Insert index (optional)
     *  \param  find    Search index (optional)
     */
    void get_index(
        const Key_t & key,
        ssize_t     * insert = NULL,
        ssize_t     * find   = NULL)
    const {
        if (!(m_item_cnt < m_capacity)) {
            if (insert) *insert = -1;  // overfill

            if (!find) return;  // nothing left to do

            insert = NULL;  // look for item
        }

        size_t index = 0;

        // (Multiple) hashing
        for (size_t i = 0; i < m_hash_fn.size(); ++i) {
            index = m_hash_fn[i](key, m_tab.size());

            if (check_index(key, index, insert, find)) return;
        }

        // Collision string
        const size_t begin_ix = index;
        while (!check_index(key, index, insert, find)) {
            if (!(++index < m_tab.size())) index = 0;

            // Whole table run through
            if (begin_ix == index) {
                if (insert) *insert = -1;
                if (find)   *find   = -1;
                break;
            }
        }
    }

    /**
     *  \brief  Find index for new item
     *
     *  \param  key  Item key
     *
     *  \return Table index or -1 in case the table is overfilled
     */
    inline ssize_t insert_index(const Key_t & key) const {
        ssize_t index; get_index(key, &index);
        return index;
    }

    /**
     *  \brief  Find index of an existing item
     *
     *  \param  key  Item key
     *
     *  \return Table index or -1 if no such item exists
     */
    inline ssize_t find_index(const Key_t & key) const {
        ssize_t index = 0; get_index(key, NULL, &index);
        return index;
    }

    public:

    /**
     *  \brief  Constructor
     *
     *  Note that the \c size parameter should be selected with care.
     *  Depending on the hash functions, the table may benefit from prime-number
     *  size (using modular hashing) etc.
     *
     *  Also, practical tests suggest that the table should not be filled
     *  more than to about 80% of its capacity, since the number of collisions
     *  then rises steeply and thus the table performance drops significantly.
     *
     *  \tparam Key_fn_args  Key functor constructor argument types
     *  \param  size         Table maximal size
     *  \param  hash_fn      Hash functions
     *  \param  capacity     Table capacity (85 % of \c size by default)
     *  \param  key_fn_args  Key functor constructor arguments
     */
    template <typename... Key_fn_args>
    linear(
        size_t                                 size,
        const std::initializer_list<Hash_fn> & hash_fn,
        size_t                                 capacity = 0,
        Key_fn_args...                         key_fn_args)
    :
        m_hash_fn  ( hash_fn                  ),
        m_tab      ( size                     ),
        m_item_cnt ( 0                        ),
        m_capacity ( capacity ? : 0.85 * size ),
        m_key_fn   ( key_fn_args...           )
    {
        // Check capacity sanity
        if (m_capacity > m_tab.size())
            throw std::logic_error(
                "libaccl::hash::linear: "
                "invalid capacity");
    }

    /** Table size getter */
    size_t size() const { return m_tab.size(); }

    /** Table capacity getter */
    size_t capacity() const { return m_capacity; }

    /** Item count getter */
    size_t item_cnt() const { return m_item_cnt; }

    /**
     *  \brief  Insert item
     *
     *  \param  item  Item
     *
     *  \return Item index or -1 in case of table overfill or duplicity
     */
    ssize_t insert(const Item_t & item) {
        ssize_t index = insert_index(m_key_fn(item));

        if (0 <= index) m_tab[index] = item;

        return index;
    }

    /**
     *  \brief  Find item
     *
     *  \param  key  Item key
     *
     *  \return Item index or -1 if no such item exists
     */
    ssize_t find(const Key_t & key) const { return find_index(key); }

    /**
     *  \brief  Item exists
     *
     *  \param  key  Item key
     *
     *  \return \c true iff the item exists in the table
     */
    bool exists(const Key_t & key) const { return -1 != find_index(key); }

    /**
     *  \brief  Get item
     *
     *  Returns reference to existing item or newly inserted if it doesn't
     *  exist, yet.
     *  Throws an exception on table overfill.
     *
     *  \param  key  Item key
     *
     *  \return Item
     */
    Item_t & operator [] (const Key_t & key) {
        // Note that insert index is resolved at least as fast as search index
        ssize_t ins_ix, find_ix; get_index(key, &ins_ix, &find_ix);

        if (0 <= find_ix) return m_tab[find_ix].item;  // found

        if (0 > ins_ix)
            throw std::runtime_error(
                "libaccl::hash::linear::[]: "
                "table overfill");

        return m_tab[ins_ix] = Item_t();  // insert new item
    }

};  // end of template class linear

}}  // end of namespace libaccl::hash

#endif  // end of #ifndef libaccl__hash__linear_hxx
