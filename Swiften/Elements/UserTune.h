/*
 * Copyright (c) 2014-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
    class SWIFTEN_API UserTune : public Payload {
        public:

            UserTune();

            virtual ~UserTune();

            const boost::optional< unsigned int >& getRating() const {
                return rating;
            }

            void setRating(const boost::optional< unsigned int >& value) {
                this->rating = value ;
            }

            const boost::optional< std::string >& getTitle() const {
                return title;
            }

            void setTitle(const boost::optional< std::string >& value) {
                this->title = value ;
            }

            const boost::optional< std::string >& getTrack() const {
                return track;
            }

            void setTrack(const boost::optional< std::string >& value) {
                this->track = value ;
            }

            const boost::optional< std::string >& getArtist() const {
                return artist;
            }

            void setArtist(const boost::optional< std::string >& value) {
                this->artist = value ;
            }

            const boost::optional< std::string >& getURI() const {
                return uri;
            }

            void setURI(const boost::optional< std::string >& value) {
                this->uri = value ;
            }

            const boost::optional< std::string >& getSource() const {
                return source;
            }

            void setSource(const boost::optional< std::string >& value) {
                this->source = value ;
            }

            const boost::optional< unsigned int >& getLength() const {
                return length;
            }

            void setLength(const boost::optional< unsigned int >& value) {
                this->length = value ;
            }


        private:
            boost::optional< unsigned int > rating;
            boost::optional< std::string > title;
            boost::optional< std::string > track;
            boost::optional< std::string > artist;
            boost::optional< std::string > uri;
            boost::optional< std::string > source;
            boost::optional< unsigned int > length;
    };
}
