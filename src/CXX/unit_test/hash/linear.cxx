/**
 *  \file
 *  \brief  Linear hashtable unit test
 *
 *  \date   2015/12/29
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


#include <libaccl/hash/linear.hxx>

#include <algorithm>
#include <iostream>
#include <exception>
#include <stdexcept>


/** Hashed data */
struct data_item {
    const std::string key;  /**< Key   */
    int               val;  /**< Value */

    /** Constructor */
    data_item(const std::string & k, int v):
        key(k), val(v)
    {}

};  // end of struct

typedef std::list<data_item> data_t;       /**< Data            */
typedef data_t::iterator     hash_item_t;  /**< Hash table item */


/** Hash function */
typedef size_t (*hash_fn_t)(const std::string &, size_t);

/** Key accessor */
class key_fn {
    public:

    inline const std::string & operator () (const hash_item_t & item) const {
        return item->key;
    }

};  // end of class key_fn

/** Hash table */
typedef libaccl::hash::linear<
        hash_item_t,
        hash_fn_t,
        std::string,
        key_fn>
    hashtab_t;


/** String sum function */
static size_t str_sum(const std::string & key) {
    size_t sum = 0;
    std::for_each(key.begin(), key.end(),
    [&sum](const char & c) {
        sum += c;
    });
    return sum;
}

/** Primary hash function (modular) */
static size_t primary_hash_fn(const std::string & key, size_t size) {
    return str_sum(key) % size;
}

/** Secondary hash function (square modular) */
static size_t secondary_hash_fn(const std::string & key, size_t size) {
    size_t sum = str_sum(key);
    return (sum * sum) % size;
}


/** Hash table test */
static int hashtab_test(size_t size) {
    int error_cnt = 0;

    std::cerr << "Hash table test BEGIN" << std::endl;

    hashtab_t tab(size, {primary_hash_fn, secondary_hash_fn});

    data_t data;
    data.emplace_back("Harry Potter and the Philosopher's Stone",  1);
    data.emplace_back("Harry Potter and the Chamber of Secrets",   2);
    data.emplace_back("Harry Potter and the Prisoner of Azkaban",  3);
    data.emplace_back("Harry Potter and the Goblet of Fire",       4);
    data.emplace_back("Harry Potter and the Order of the Phoenix", 5);
    data.emplace_back("Harry Potter and the Half-Blood Prince",    6);
    data.emplace_back("Harry Potter and the Deathly Hallows",      7);

    for (auto d = data.begin(); d != data.end(); ++d)
        tab.insert(d);

    for (auto d = data.begin(); d != data.end(); ++d)
        std::cout
            << d->key << ": "
            << tab.find(d->key) << " ("
            << tab[d->key]->val << ")"
            << std::endl;

    const std::string keys[] = {
        "Whatever string",
        "Harry Potter and the Prisoner of Azkaban",
        "Something different",
        "Harry Potter and the Order of the Phoenix",
        "Harry Potter and Hermione Granger",
    };

    for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); ++i)
        std::cout
            << keys[i] << ": " << tab.find(keys[i]) << std::endl;

    std::cerr << "Hash table test END" << std::endl;

    return error_cnt;
}


/** Unit test */
static int main_impl(int argc, char * const argv[]) {
    int exit_code = 64;  // pessimistic assumption

    size_t size = 5423;  // hash table size
    if (argc > 1) size = ::atoi(argv[1]);

    do {  // pragmatic do ... while (0) loop allowing for breaks
        exit_code = hashtab_test(size);
        if (0 != exit_code) break;

    } while (0);  // end of pragmatic loop

    std::cerr
        << "Exit code: " << exit_code
        << std::endl;

    return exit_code;
}

/** Unit test exception-safe wrapper */
int main(int argc, char * const argv[]) {
    int exit_code = 128;

    try {
        exit_code = main_impl(argc, argv);
    }
    catch (const std::exception & x) {
        std::cerr
            << "Standard exception caught: "
            << x.what()
            << std::endl;
    }
    catch (...) {
        std::cerr
            << "Unhandled non-standard exception caught"
            << std::endl;
    }

    return exit_code;
}
