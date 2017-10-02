/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
    class SWIFTEN_API UserLocation : public Payload {
        public:

            UserLocation();

            virtual ~UserLocation();

            const boost::optional< std::string >& getArea() const {
                return area;
            }

            void setArea(const boost::optional< std::string >& value) {
                this->area = value ;
            }

            const boost::optional< double >& getAltitude() const {
                return altitude;
            }

            void setAltitude(const boost::optional< double >& value) {
                this->altitude = value ;
            }

            const boost::optional< std::string >& getLocality() const {
                return locality;
            }

            void setLocality(const boost::optional< std::string >& value) {
                this->locality = value ;
            }

            const boost::optional< double >& getLatitude() const {
                return latitude;
            }

            void setLatitude(const boost::optional< double >& value) {
                this->latitude = value ;
            }

            const boost::optional< double >& getAccuracy() const {
                return accuracy;
            }

            void setAccuracy(const boost::optional< double >& value) {
                this->accuracy = value ;
            }

            const boost::optional< std::string >& getDescription() const {
                return description;
            }

            void setDescription(const boost::optional< std::string >& value) {
                this->description = value ;
            }

            const boost::optional< std::string >& getCountryCode() const {
                return countryCode;
            }

            void setCountryCode(const boost::optional< std::string >& value) {
                this->countryCode = value ;
            }

            const boost::optional< boost::posix_time::ptime >& getTimestamp() const {
                return timestamp;
            }

            void setTimestamp(const boost::optional< boost::posix_time::ptime >& value) {
                this->timestamp = value ;
            }

            const boost::optional< std::string >& getFloor() const {
                return floor;
            }

            void setFloor(const boost::optional< std::string >& value) {
                this->floor = value ;
            }

            const boost::optional< std::string >& getBuilding() const {
                return building;
            }

            void setBuilding(const boost::optional< std::string >& value) {
                this->building = value ;
            }

            const boost::optional< std::string >& getRoom() const {
                return room;
            }

            void setRoom(const boost::optional< std::string >& value) {
                this->room = value ;
            }

            const boost::optional< std::string >& getCountry() const {
                return country;
            }

            void setCountry(const boost::optional< std::string >& value) {
                this->country = value ;
            }

            const boost::optional< std::string >& getRegion() const {
                return region;
            }

            void setRegion(const boost::optional< std::string >& value) {
                this->region = value ;
            }

            const boost::optional< std::string >& getURI() const {
                return uri;
            }

            void setURI(const boost::optional< std::string >& value) {
                this->uri = value ;
            }

            const boost::optional< double >& getLongitude() const {
                return longitude;
            }

            void setLongitude(const boost::optional< double >& value) {
                this->longitude = value ;
            }

            const boost::optional< double >& getError() const {
                return error;
            }

            void setError(const boost::optional< double >& value) {
                this->error = value ;
            }

            const boost::optional< std::string >& getPostalCode() const {
                return postalCode;
            }

            void setPostalCode(const boost::optional< std::string >& value) {
                this->postalCode = value ;
            }

            const boost::optional< double >& getBearing() const {
                return bearing;
            }

            void setBearing(const boost::optional< double >& value) {
                this->bearing = value ;
            }

            const boost::optional< std::string >& getText() const {
                return text;
            }

            void setText(const boost::optional< std::string >& value) {
                this->text = value ;
            }

            const boost::optional< std::string >& getDatum() const {
                return datum;
            }

            void setDatum(const boost::optional< std::string >& value) {
                this->datum = value ;
            }

            const boost::optional< std::string >& getStreet() const {
                return street;
            }

            void setStreet(const boost::optional< std::string >& value) {
                this->street = value ;
            }

            const boost::optional< double >& getSpeed() const {
                return speed;
            }

            void setSpeed(const boost::optional< double >& value) {
                this->speed = value ;
            }


        private:
            boost::optional< std::string > area;
            boost::optional< double > altitude;
            boost::optional< std::string > locality;
            boost::optional< double > latitude;
            boost::optional< double > accuracy;
            boost::optional< std::string > description;
            boost::optional< std::string > countryCode;
            boost::optional< boost::posix_time::ptime > timestamp;
            boost::optional< std::string > floor;
            boost::optional< std::string > building;
            boost::optional< std::string > room;
            boost::optional< std::string > country;
            boost::optional< std::string > region;
            boost::optional< std::string > uri;
            boost::optional< double > longitude;
            boost::optional< double > error;
            boost::optional< std::string > postalCode;
            boost::optional< double > bearing;
            boost::optional< std::string > text;
            boost::optional< std::string > datum;
            boost::optional< std::string > street;
            boost::optional< double > speed;
    };
}
