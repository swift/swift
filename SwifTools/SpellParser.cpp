/*
 * Copyright (c) 2011-2013 Vlad Voicu
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <SwifTools/SpellParser.h>

#include <string>

#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/spirit/include/lex_lexertl.hpp>

namespace lex = boost::spirit::lex;

namespace Swift {

template <typename Lexer>
struct word_count_tokens : lex::lexer<Lexer>
{
    word_count_tokens()
    {
    // define tokens (regular expresions) to match strings
    // order is important
        this->self.add
            ("w{3}.[^ ]+", ID_WWW)
            ("http:\\/\\/[^ ]+", ID_HTTP)
            ("\\w{1,}['?|\\-?]?\\w{1,}", ID_WORD)
            (".", ID_CHAR);
    }
};

struct counter
{
    typedef bool result_type;
    // the function operator gets called for each of the matched tokens
    template <typename Token>
    bool operator()(Token const& t, PositionPairList& wordPositions, std::size_t& position) const
    {
        switch (t.id()) {
            case ID_WWW:
                position += static_cast<std::size_t>(t.value().size());
                break;
            case ID_HTTP:
                position += static_cast<std::size_t>(t.value().size());
                break;
            case ID_WORD:       // matched a word
                wordPositions.push_back(boost::tuples::make_tuple(position, position + static_cast<std::size_t>(t.value().size())));
                position += static_cast<std::size_t>(t.value().size());
                break;
            case ID_CHAR:       // match a simple char
                ++position;
                break;
        }
        return true;        // always continue to tokenize
    }
};

void SpellParser::check(const std::string& fragment, PositionPairList& wordPositions) {
    std::size_t position = 0;
    // create the token definition instance needed to invoke the lexical analyzer
    word_count_tokens<lex::lexertl::lexer<> > word_count_functor;
    char const* first = fragment.c_str();
    char const* last = &first[fragment.size()];
    lex::tokenize(first, last, word_count_functor, boost::bind(counter(), _1, boost::ref(wordPositions), boost::ref(position)));
}

}
