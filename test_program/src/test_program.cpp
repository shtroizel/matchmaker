/*
Copyright (c) 2020, Eric Hyer
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/



#include <chrono>
#include <iostream>
#include <iomanip>
#include <random>
#include <sstream>


#include <matchmaker/matchmaker.h>


int main()
{
    int index{-1};
    bool found{false};

    while (true)
    {
        std::cout << "matchmaker (" << matchmaker::size() << ") $  ";

        std::string line;
        std::getline(std::cin, line);
        if (std::cin.eof() == 1)
        {
            std::cout << std::endl;
            break;
        }
        std::vector<std::string> words;
        std::stringstream ss(line);
        std::string token;
        while (std::getline(ss, token, ' '))
            words.push_back(token);

        if (words.size() == 1)
        {
            if (words[0] == ":abc")
            {
                for (int i = 0; i < matchmaker::size(); ++i)
                {
                    auto start = std::chrono::high_resolution_clock::now();
                    std::string const & str = matchmaker::at(i);
                    auto stop = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
                    std::cout << "       [" << i << "] :  '" << str << "' accessed in " << duration.count()
                              << " microseconds\n";
                }
                std::cout << std::flush;
                continue;
            }
            else if (words[0].size() && words[0][0] == ':')
            {
                std::string word{words[0].substr(1)};
                std::vector<int> completion;
                auto start = std::chrono::high_resolution_clock::now();
                matchmaker::complete(word, completion);
                auto stop = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
                std::cout << "completion (" << completion.size() << ") : ";
                for (auto const & c : completion)
                    std::cout << " " << matchmaker::at(c);
                std::cout << "\ncompletion done in " << duration.count() << " milliseconds" << std::endl;
                continue;
            }
        }

        for (auto const & word : words)
        {
            std::cout << "       [";
            auto start = std::chrono::high_resolution_clock::now();
            index = matchmaker::lookup(word, &found);
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
            std::cout << index << "] :  '" << matchmaker::at(index) << "'     lookup time: "
                      << duration.count() << " microseconds" << std::endl;
        }
        std::cout << std::endl;
    }

    return 0;
}
