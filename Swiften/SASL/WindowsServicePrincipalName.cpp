/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/SASL/WindowsServicePrincipalName.h>

#include <vector>

#include <Ntdsapi.h>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Base/String.h>

namespace Swift {

WindowsServicePrincipalName::WindowsServicePrincipalName(const std::string& serviceName) : serviceClass_(L"xmpp"), instancePort_(0) {
    serviceName_ = convertStringToWString(serviceName);
}

WindowsServicePrincipalName::~WindowsServicePrincipalName() {
}

void WindowsServicePrincipalName::setServiceClass(const std::string& serviceClass) {
    serviceClass_ = convertStringToWString(serviceClass);
}

void WindowsServicePrincipalName::setInstanceName(const std::string& instanceName) {
    instanceName_ = convertStringToWString(instanceName);
}

void WindowsServicePrincipalName::setReferrer(const std::string& referrer) {
    referrer_ = convertStringToWString(referrer);
}

std::string WindowsServicePrincipalName::toString() {
    DWORD length = 512;
    DWORD status = ERROR_BUFFER_OVERFLOW;
    bool firstCall = true;
    std::string str;

    while (status == ERROR_BUFFER_OVERFLOW) {
        std::vector<wchar_t> value(length);

        /* length after this call will contain the required length if current length is not enough - so the next call should succeed */
        status = dsMakeSpn(&length, vecptr(value));
        if (status == ERROR_SUCCESS) {
            str = convertWStringToString(std::wstring(vecptr(value), length-1 /* trailing 0 character */));
            break;
        }

        if ((firstCall == false) || (status != ERROR_BUFFER_OVERFLOW)) {
            std::stringstream errorString;
            boost::system::error_code errorCode(status, boost::system::system_category());

            errorString << "Error creating Service Principal Name: status: 0x" << std::hex << status << ": " << errorCode.message();

            /* Any other error will be a programming error */
            throw std::runtime_error(errorString.str());
        }

        firstCall = false;
    }

    SWIFT_LOG(debug) << "SPN: " << str << std::endl;
    return str;
}

DWORD WindowsServicePrincipalName::dsMakeSpn(DWORD* length, wchar_t* value) {
    DWORD status;

#ifdef UNICODE
    SWIFT_LOG(debug) << "UNICODE is defined" << std::endl;
#else
    SWIFT_LOG(debug) << "UNICODE is not defined" << std::endl;
#endif

    SWIFT_LOG(debug) << "serviceClass_: " << convertWStringToString(serviceClass_.c_str()) << std::endl;
    SWIFT_LOG(debug) << "serviceName_: " << convertWStringToString(serviceName_.c_str()) << std::endl;
    SWIFT_LOG(debug) << "instanceName_: " << convertWStringToString(instanceName_.c_str()) << std::endl;
    SWIFT_LOG(debug) << "referrer_: " << convertWStringToString(referrer_.c_str()) << std::endl;
    SWIFT_LOG(debug) << "instancePort_: " << instancePort_ << std::endl;
    SWIFT_LOG(debug) << "length: " << *length << std::endl;

    /* Call the Unicode function because that is recommended:
https://msdn.microsoft.com/en-us/library/windows/desktop/ff381407%28v=vs.85%29.aspx */
    status =  DsMakeSpnW(
            serviceClass_.c_str(),
            serviceName_.c_str(),
            instanceName_.empty() ? NULL : instanceName_.c_str(),
            instancePort_,
            referrer_.empty() ? NULL : referrer_.c_str(),
            length,
            value);
    if (status != ERROR_SUCCESS) {
        boost::system::error_code errorCode(status, boost::system::system_category());

        SWIFT_LOG(debug) << std::hex << "status: 0x" << status << ": " << errorCode.message() << std::endl;
    }

    return status;
}

}
