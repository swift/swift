/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/JID/JID.h>

namespace Swift {

    class SWIFTEN_API LinkLocalServiceInfo {
        public:
            enum Status { Available, Away, DND };

            LinkLocalServiceInfo() : status(Available) {}

            const std::string& getFirstName() const { return firstName; }
            void setFirstName(const std::string& f) { firstName = f; }

            const std::string& getLastName() const { return lastName; }
            void setLastName(const std::string& l) { lastName = l; }

            const std::string& getEMail() const { return email; }
            void setEMail(const std::string& e) { email = e; }

            const JID& getJID() const { return jid; }
            void setJID(const JID& j) { jid = j; }

            const std::string& getMessage() const { return message; }
            void setMessage(const std::string& m) { message = m; }

            const std::string& getNick() const { return nick; }
            void setNick(const std::string& n) { nick = n; }

            Status getStatus() const { return status; }
            void setStatus(Status s) { status = s; }

            boost::optional<unsigned short> getPort() const { return port; }
            void setPort(unsigned short p) { port = p; }

            boost::optional<ByteArray> toTXTRecord() const;

            static LinkLocalServiceInfo createFromTXTRecord(const ByteArray& record);

        private:
            static ByteArray getEncoded(const std::string&);
            static std::pair<std::string,std::string> readEntry(const ByteArray&, size_t*);

        private:
            std::string firstName;
            std::string lastName;
            std::string email;
            JID jid;
            std::string message;
            std::string nick;
            Status status;
            boost::optional<unsigned short> port;
    };
}
