/**
 *  \file
 *  \brief  Hypersphere pattern unit test
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


#include <libaccl/pattern/hypersphere.hxx>

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <exception>
#include <stdexcept>


/** Hypersphere pattern test */
static int hypersphere_test(
    size_t                   dimension,
    const std::vector<int> & layers)
{
    std::cerr << "Hypersphere pattern test BEGIN" << std::endl;

    int error_cnt = 0;

    // Draw 2D hypersphere (i.e. a circle)
    libaccl::pattern::hypersphere<int> circle(dimension, layers);

    std::for_each(circle.begin(), circle.end(),
    [](const libaccl::pattern::hypersphere<int>::set_t::value_type & x) {
        std::cout << '[';

        std::for_each(x.first.begin(), x.first.end(), [](int x_i) {
            std::cout << x_i << ' ';
        });

        std::cout << "] " << x.second << std::endl;
    });

    // We plot 2D circles
    if (2 == dimension) {
        int radius = layers[0];

        std::cout << "# ";
        for (int x = -radius - 1; x < radius + 2; ++x)
            std::cout << ' ' << ::abs(x) % 10;
        std::cout << std::endl;

        for (int y = radius + 1; y >= -radius - 1; --y) {
            std::cout << '#' << ::abs(y) % 10;

            for (int x = -radius - 1; x < radius + 2; ++x) {
                std::vector<int> point(2);
                point[0] = x;
                point[1] = y;

                if (point & circle)
                    std::cout
                        << std::setfill('0') << std::setw(2)
                        << circle.get_payload(point);

                else if (0 == x % 10) std::cout << " |";
                else if (0 == y % 10) std::cout << "--";
                else if (x ==  y)     std::cout << " /";
                else if (x == -y)     std::cout << " \\";
                else std::cout << " .";
            }

            std::cout << ' ' << ::abs(y) % 10 << std::endl;
        }

        std::cout << "# ";
        for (int x = -radius - 1; x < radius + 2; ++x)
            std::cout << ' ' << ::abs(x) % 10;
        std::cout << std::endl;
    }

    std::cerr << "Hypersphere pattern test END" << std::endl;

    return error_cnt;
}


/** Unit test */
static int main_impl(int argc, char * const argv[]) {
    int exit_code = 64;  // pessimistic assumption

    size_t dimension = 2;  // hypersphere dimension
    if (argc > 1) dimension = ::atoi(argv[1]);

    // Hypersphere layers
    std::vector<int> layers;
    for (int i = 2; i < argc; ++i)
        layers.push_back(::atoi(argv[i]));

    // Full hypersphere by default
    if (layers.empty()) layers.push_back(12);

    do {  // pragmatic do ... while (0) loop allowing for breaks
        exit_code = hypersphere_test(dimension, layers);
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
