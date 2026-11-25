/*
Copyright (c) 2020-2023, shtroizel
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



// 'a o e u' (without spaces) replaced by letters deliminated by '/' (aoeu)
// 's n t h' (without spaces) replaced by letters deliminated by '_' (snth)



#include <iostream>
#include <string>
#include <vector>

// this leaf owns its words: it defines their instances, which other leaves refer to by name
#define MATCHABLE_INSTANCE_VISIBILITY protected

#include <matchmaker/generated_symbols/aoeu/snth.h>
#include <matchmaker/generated_matchables/aoeu/snth.h>


int mm_count_snth()
{
    return word_snth::variants_by_string().size();
}


std::string const & mm_at_snth(int index)
{
    if (index < 0 || index >= mm_count_snth())
    {
        std::cout << "mm_at_snth(" << index << ") out of bounds with mm_count_snth() of: "
                  << mm_count_snth() << std::endl;
        static std::string const empty_str;
        return empty_str;
    }

    std::string const & str = word_snth::from_by_string_index_as_ref(index).as_string();
    return str;
}


matchable::VariantNames const & mm_names_snth()
{
    return word_snth::Iword_snth::variant_names();
}


int mm_ordinal_summation_snth(int index)
{
    if (index < 0 || index >= mm_count_snth())
    {
        std::cout << "mm_ordinal_summation(" << index << ") out of bounds with mm_count_snth() of: "
                  << mm_count_snth() << std::endl;
        return 0;
    }

    return word_snth::from_by_string_index_as_ref(index).as_ordinal_summation();
}


std::vector<int8_t> const & mm_flagged_parts_of_speech_snth(int index)
{
    static std::vector<int8_t> const empty{};
    if (index < 0 || index >= mm_count_snth())
    {
        std::cout << "mm_flagged_parts_of_speech_snth(" << index
                  << ") out of bounds with mm_count_snth() of: " << mm_count_snth() << std::endl;
        return empty;
    }
    return word_snth::from_by_string_index_as_ref(index).as_pos_vect();
}


bool mm_is_name_snth(int index)
{
    if (index < 0 || index >= mm_count_snth())
    {
        std::cout << "mm_is_name_snth(" << index << ") out of bounds with mm_count_snth() of: "
                  << mm_count_snth() << std::endl;
        return false;
    }

    return word_snth::from_by_string_index_as_ref(index).as_is_name();
}


bool mm_is_male_name_snth(int index)
{
    if (index < 0 || index >= mm_count_snth())
    {
        std::cout << "mm_is_male_name_snth(" << index << ") out of bounds with mm_count_snth() of: "
                  << mm_count_snth() << std::endl;
        return false;
    }

    return word_snth::from_by_string_index_as_ref(index).as_is_male_name();
}


bool mm_is_female_name_snth(int index)
{
    if (index < 0 || index >= mm_count_snth())
    {
        std::cout << "mm_is_female_name_snth(" << index << ") out of bounds with mm_count_snth() of: "
                  << mm_count_snth() << std::endl;
        return false;
    }

    return word_snth::from_by_string_index_as_ref(index).as_is_female_name();
}


bool mm_is_place_snth(int index)
{
    if (index < 0 || index >= mm_count_snth())
    {
        std::cout << "mm_is_place_snth(" << index << ") out of bounds with mm_count_snth() of: "
                  << mm_count_snth() << std::endl;
        return false;
    }

    return word_snth::from_by_string_index_as_ref(index).as_is_place();
}


bool mm_is_compound_snth(int index)
{
    if (index < 0 || index >= mm_count_snth())
    {
        std::cout << "mm_is_compound_snth(" << index << ") out of bounds with mm_count_snth() of: "
                  << mm_count_snth() << std::endl;
        return false;
    }

    return word_snth::from_by_string_index_as_ref(index).as_is_compound();
}


bool mm_is_acronym_snth(int index)
{
    if (index < 0 || index >= mm_count_snth())
    {
        std::cout << "mm_is_acronym_snth(" << index << ") out of bounds with mm_count_snth() of: "
                  << mm_count_snth() << std::endl;
        return false;
    }

    return word_snth::from_by_string_index_as_ref(index).as_is_acronym();
}


bool mm_is_phrase_snth(int index)
{
    if (index < 0 || index >= mm_count_snth())
    {
        std::cout << "mm_phrase_snth(" << index << ") out of bounds with mm_count_snth() of: "
                  << mm_count_snth() << std::endl;
        return false;
    }

    return word_snth::from_by_string_index_as_ref(index).as_is_phrase();
}


std::vector<::matchable::VariantRef> const & mm_synonyms_snth(int index)
{
    static std::vector<::matchable::VariantRef> const empty{};
    if (index < 0 || index >= mm_count_snth())
    {
        std::cout << "mm_synonyms_snth(" << index << ") out of bounds with mm_count_snth() of: "
                  << mm_count_snth() << std::endl;
        return empty;
    }
    return word_snth::from_by_string_index_as_ref(index).as_syn_vect();
}


std::vector<::matchable::VariantRef> const & mm_antonyms_snth(int index)
{
    static std::vector<::matchable::VariantRef> const empty{};
    if (index < 0 || index >= mm_count_snth())
    {
        std::cout << "mm_antonyms_snth(" << index << ") out of bounds with mm_count_snth() of: "
                  << mm_count_snth() << std::endl;
        return empty;
    }
    return word_snth::from_by_string_index_as_ref(index).as_ant_vect();
}


std::vector<::matchable::VariantRef> const & mm_embedded_snth(int index)
{
    static std::vector<::matchable::VariantRef> const empty{};
    if (index < 0 || index >= mm_count_snth())
    {
        std::cout << "mm_embedded_snth(" << index << ") out of bounds with mm_count_snth() of: "
                  << mm_count_snth() << std::endl;
        return empty;
    }
    return word_snth::from_by_string_index_as_ref(index).as_embedded_vect();
}


std::vector<::matchable::VariantRef> const & mm_definition_snth(int index)
{
    static std::vector<::matchable::VariantRef> const empty{};
    if (index < 0 || index >= mm_count_snth())
    {
        std::cout << "mm_definition_snth(" << index << ") out of bounds with mm_count_snth() of: "
                  << mm_count_snth() << std::endl;
        return empty;
    }
    return word_snth::from_by_string_index_as_ref(index).as_definition_vect();
}
