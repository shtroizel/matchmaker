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



// 'aoeu' replaced by letters deliminated by '/'
// 'snth' replaced by letters deliminated by '_'



#include <iostream>
#include <string>
#include <vector>

#include <matchmaker/generated_letters/aoeu/snth.h>

#ifdef STAGE_0
#include <matchmaker/generated_matchables_stage_0/aoeu/snth.h>
#elif defined STAGE_1
#include <matchmaker/generated_matchables_stage_1/aoeu/snth.h>
#else
INVALID_STAGE_COMPILE_ERROR
#endif


namespace matchmaker
{
    int size_snth()
    {
        return word_snth::variants_by_string().size();
    }


    std::string const & at_snth(int index)
    {
        if (index < 0 || index >= size_snth())
        {
            std::cout << "at_snth(" << index << ") out of bounds with size_snth() of: "
                      << size_snth() << std::endl;
            static std::string const empty_str;
            return empty_str;
        }

        std::string const & str = word_snth::from_by_string_index(index).as_string();
        return str;
    }


    int lookup_snth(std::string const & word, bool * found)
    {
        word_snth::Type w{word_snth::lookup(word, found)};

        if (nullptr != found)
            *found = w.as_string() == word;

        return w.as_by_string_index();
    }


    void parts_of_speech_snth(int index, std::vector<std::string const *> & pos)
    {
        pos.clear();
        if (index < 0 || index >= size_snth())
        {
            std::cout << "parts_of_speech_snth(" << index << ") out of bounds with size_snth() of: "
                      << size_snth() << std::endl;
            return;
        }
        for (auto const & p : word_snth::from_by_string_index(index).as_pos_snth_vect())
            pos.push_back(&p.as_pos_desc_snth().as_string());
    }


    void synonyms_snth(int index, std::vector<int> & syn)
    {
        syn.clear();
        if (index < 0 || index >= size_snth())
        {
            std::cout << "synonyms_snth(" << index << ") out of bounds with size_snth() of: "
                      << size_snth() << std::endl;
            return;
        }
        for (int synonym : word_snth::from_by_string_index(index).as_syn_vect())
            syn.push_back(synonym);
    }


    void antonyms_snth(int index, std::vector<int> & ant)
    {
        ant.clear();
        if (index < 0 || index >= size_snth())
        {
            std::cout << "antonyms_snth(" << index << ") out of bounds with size_snth() of: "
                      << size_snth() << std::endl;
            return;
        }
        for (int antonym : word_snth::from_by_string_index(index).as_ant_vect())
            ant.push_back(antonym);
    }
}
