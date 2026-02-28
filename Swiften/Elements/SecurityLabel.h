/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>
#include <vector>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
    class SWIFTEN_API SecurityLabel : public Payload {
        public:
            using ref = std::shared_ptr<SecurityLabel>;

            SecurityLabel();

            virtual ~SecurityLabel();

            const std::vector< std::string >& getEquivalentLabels() const {
                return equivalentLabels;
            }

            void setEquivalentLabels(const std::vector< std::string >& value) {
                this->equivalentLabels = value ;
            }

            void addEquivalentLabel(const std::string& value) {
                this->equivalentLabels.push_back(value);
            }

            const std::string& getForegroundColor() const {
                return foregroundColor;
            }

            void setForegroundColor(const std::string& value) {
                this->foregroundColor = value ;
            }

            const std::string& getDisplayMarking() const {
                return displayMarking;
            }

            void setDisplayMarking(const std::string& value) {
                this->displayMarking = value ;
            }

            const std::string& getBackgroundColor() const {
                return backgroundColor;
            }

            void setBackgroundColor(const std::string& value) {
                this->backgroundColor = value ;
            }

            const std::string& getLabel() const {
                return label;
            }

            void setLabel(const std::string& value) {
                this->label = value ;
            }


        private:
            std::vector< std::string > equivalentLabels;
            std::string foregroundColor;
            std::string displayMarking;
            std::string backgroundColor;
            std::string label;
    };
}
