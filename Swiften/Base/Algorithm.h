/*
 * Copyright (c) 2011-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <algorithm>
#include <list>
#include <map>
#include <string>
#include <vector>

namespace Swift {

    /*
     * Generic erase()
     */
    namespace Detail {
        struct VectorCategory {};
        struct ListCategory {};
        struct MapCategory {};

        template<typename T>
        struct ContainerTraits;

        template<typename A, typename B>
        struct ContainerTraits< std::vector<A, B> > {
            typedef VectorCategory Category;
        };

        template<>
        struct ContainerTraits< std::string > {
            typedef VectorCategory Category;
        };

        template<typename A, typename B>
        struct ContainerTraits< std::list<A, B> > {
            typedef ListCategory Category;
        };

        template<typename A, typename B, typename C, typename D>
        struct ContainerTraits< std::map<A, B, C, D> > {
            typedef MapCategory Category;
        };

        template<typename C, typename V>
        void eraseImpl(C& c, const V& v, VectorCategory) {
            c.erase(std::remove(c.begin(), c.end(), v), c.end());
        }

        template<typename C, typename V>
        void eraseImpl(C& c, const V& v, ListCategory) {
            c.remove(v);
        }

        template<typename C, typename V>
        void eraseImpl(C& c, const V& v, MapCategory) {
          for (typename C::iterator it = c.begin(); it != c.end(); ) {
                if (it->second == v) {
                    c.erase(it++);
                }
                else {
                    ++it;
                }
            }
        }

        template<typename C, typename P>
        void eraseIfImpl(C& c, const P& p, MapCategory) {
          for (typename C::iterator it = c.begin(); it != c.end(); ) {
                if (p(*it)) {
                    c.erase(it++);
                }
                else {
                    ++it;
                }
            }
        }

        template<typename C, typename P>
        void eraseIfImpl(C& c, const P& p, VectorCategory) {
            c.erase(std::remove_if(c.begin(), c.end(), p), c.end());
        }
    }

    template<typename C, typename V>
    void erase(C& container, const V& value) {
        Detail::eraseImpl(container, value, typename Detail::ContainerTraits<C>::Category());
    }

    template<typename C, typename P>
    void eraseIf(C& container, const P& predicate) {
        Detail::eraseIfImpl(container, predicate, typename Detail::ContainerTraits<C>::Category());
    }

    template<typename Source, typename Target>
    void append(Target& target, const Source& source) {
        target.insert(target.end(), source.begin(), source.end());
    }

    template<typename Source, typename Target>
    void assign(Target& target, const Source& source) {
        target.assign(source.begin(), source.end());
    }

    template<typename A, typename B, typename C, typename D>
    B get(const std::map<A, B, C, D>& map, const A& key, const B& defaultValue) {
        typename std::map<A, B, C, D>::const_iterator i = map.find(key);
        if (i != map.end()) {
            return i->second;
        }
        else {
            return defaultValue;
        }
    }

    template<typename Container>
    void safeClear(Container& c) {
        std::fill(c.begin(), c.end(), 0);
        c.clear();
    }

    /*
     * Functors
     */
    template<typename K, typename V>
    class PairFirstEquals {
        public:
            PairFirstEquals(const K& value) : value(value) {
            }

            bool operator()(const std::pair<K,V>& pair) const {
                return pair.first == value;
            }

        private:
            K value;
    };

    template<typename K, typename V>
    class PairSecondEquals {
        public:
            PairSecondEquals(const V& value) : value(value) {
            }

            bool operator()(const std::pair<K,V>& pair) const {
                return pair.second == value;
            }

        private:
            V value;
    };

    template <typename Map>
    bool key_compare(Map const& lhs, Map const& rhs) {

        auto pred = [](decltype(*lhs.begin()) a, decltype(a) b) { return a.first == b.first; };

        return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin(), pred);
    }
}
