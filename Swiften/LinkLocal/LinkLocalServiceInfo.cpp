/*
 * Copyright (c) 2010-2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/LinkLocal/LinkLocalServiceInfo.h>

#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <Swiften/Base/Algorithm.h>
#include <Swiften/Base/Concat.h>
#include <Swiften/Base/Log.h>

namespace Swift {

boost::optional<ByteArray> LinkLocalServiceInfo::toTXTRecord() const {
    try {
        ByteArray result(getEncoded("txtvers=1"));
        if (!firstName.empty()) {
            append(result, getEncoded("1st=" + firstName));
        }
        if (!lastName.empty()) {
            append(result, getEncoded("last=" + lastName));
        }
        if (!email.empty()) {
            append(result, getEncoded("email=" + email));
        }
        if (jid.isValid()) {
            append(result, getEncoded("jid=" + jid.toString()));
        }
        if (!message.empty()) {
            append(result, getEncoded("msg=" + message));
        }
        if (!nick.empty()) {
            append(result, getEncoded("nick=" + nick));
        }
        if (port) {
            append(result, getEncoded("port.p2pj=" + std::string(std::to_string(*port))));
        }

        switch (status) {
            case Available: append(result, getEncoded("status=avail")); break;
            case Away: append(result, getEncoded("status=away")); break;
            case DND: append(result, getEncoded("status=dnd")); break;
        }

        return result;
    }
    catch (const std::exception& e) {
        SWIFT_LOG(warning) << "Failed to create TXT record for link local service info: " << e.what() << std::endl;
        return boost::none;
    }
}

ByteArray LinkLocalServiceInfo::getEncoded(const std::string& s) {
    ByteArray sizeByte;
    sizeByte.resize(1);
    sizeByte[0] = boost::numeric_cast<unsigned char>(s.size());
    return concat(sizeByte, createByteArray(s));
}

LinkLocalServiceInfo LinkLocalServiceInfo::createFromTXTRecord(const ByteArray& record) {
    LinkLocalServiceInfo info;
    size_t i = 0;
    size_t recordCount = record.size();
    while (i < recordCount) {
        std::pair<std::string,std::string> entry = readEntry(record, &i);
        if (entry.first.empty()) {
            break;
        }
        else if (entry.first == "1st") {
            info.setFirstName(entry.second);
        }
        else if (entry.first == "last") {
            info.setLastName(entry.second);
        }
        else if (entry.first == "email") {
            info.setEMail(entry.second);
        }
        else if (entry.first == "jid") {
            info.setJID(JID(entry.second));
        }
        else if (entry.first == "msg") {
            info.setMessage(entry.second);
        }
        else if (entry.first == "nick") {
            info.setNick(entry.second);
        }
        else if (entry.first == "port.p2pj") {
            try {
                info.setPort(boost::numeric_cast<unsigned short>(boost::lexical_cast<int>(entry.second)));
            }
            catch (const boost::bad_lexical_cast&) {
            }
            catch (const boost::numeric::bad_numeric_cast&) {
            }
        }
        else if (entry.first == "status") {
            if (entry.second == "away") {
                info.setStatus(Away);
            }
            else if (entry.second == "dnd") {
                info.setStatus(DND);
            }
        }
    }
    return info;
}

std::pair<std::string,std::string> LinkLocalServiceInfo::readEntry(const ByteArray& record, size_t* index) {
    size_t& i = *index;
    std::string key;
    std::string value;

    size_t entryEnd = i + 1 + record[i];
    ++i;
    bool inKey = true;
    while (i < entryEnd && i < record.size()) {
        if (inKey) {
            if (record[i] == '=') {
                inKey = false;
            }
            else {
                key += static_cast<char>(record[i]);
            }
        }
        else {
            value += static_cast<char>(record[i]);
        }
        ++i;
    }
    return std::make_pair(key, value);
}

}
