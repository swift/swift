/*
 * Copyright (c) 2013-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <algorithm>
#include <vector>

#include <boost/bind.hpp>

#include <Swiften/Base/API.h>

namespace Swift {
    template<typename T>
    class SWIFTEN_API Listenable {
        public:
            void addListener(T* listener) {
                listeners.push_back(listener);
            }

            void removeListener(T* listener) {
                listeners.erase(std::remove(listeners.begin(), listeners.end(), listener), listeners.end());
            }

        protected:
            template<typename F>
            void notifyListeners(F event) {
                for (typename std::vector<T*>::iterator i = listeners.begin(); i != listeners.end(); ++i) {
                    event(*i);
                }
            }

            template<typename F, typename A1>
            void notifyListeners(F f, const A1& a1) {
                notifyListeners(boost::bind(f, _1, a1));
            }

            template<typename F, typename A1, typename A2>
            void notifyListeners(F f, const A1& a1, const A2& a2) {
                notifyListeners(boost::bind(f, _1, a1, a2));
            }

            template<typename F, typename A1, typename A2, typename A3>
            void notifyListeners(F f, const A1& a1, const A2& a2, const A3& a3) {
                notifyListeners(boost::bind(f, _1, a1, a2, a3));
            }

        private:
            std::vector<T*> listeners;
    };
}

