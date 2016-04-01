/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/SASL/WindowsAuthentication.h>

#include <iomanip>

#include <Secext.h>

#include <Swiften/Base/Log.h>
#include <Swiften/Base/String.h>

#define ASSIGN_ERROR(status, errorCode) \
{ \
    errorCode = std::make_shared<boost::system::error_code>(status, boost::system::system_category()); \
    SWIFT_LOG(debug) << std::hex << "status: 0x" << status << ": " << errorCode->message() << std::endl; \
}

#define ASSIGN_SEC_ERROR(status, errorCode) \
{ \
    if (status == SEC_E_OK) \
    { \
        SWIFT_LOG(debug) << "success" << std::endl; \
    } \
    else { \
        ASSIGN_ERROR(status, errorCode); \
    } \
}

namespace Swift {

std::shared_ptr<boost::system::error_code> getUserNameEx(std::string& userName, std::string& clientName, std::string& serverName) {
    ULONG length = 512;
    DWORD status = ERROR_MORE_DATA;
    bool firstCall = true;
    std::shared_ptr<boost::system::error_code> errorCode;

    while (status == ERROR_MORE_DATA) {
        std::vector<wchar_t> value(length);

        /* length after this call will contain the required length if current length is not enough - so the next call should succeed */
        if (GetUserNameExW(NameSamCompatible, vecptr(value), &length)) {
            std::size_t position;

            userName = convertWStringToString(std::wstring(vecptr(value), length));
            SWIFT_LOG(debug) << "User Name: " << userName << std::endl;

            position = userName.find("\\");
            clientName = userName.substr(position + 1);
            SWIFT_LOG(debug) << "Client name: " << clientName << std::endl;

            serverName = userName.substr(0, position);
            SWIFT_LOG(debug) << "Server name: " << serverName << std::endl;

            break;
        }

        status = GetLastError();
        if ((firstCall == false) || (status != ERROR_MORE_DATA)) {
            ASSIGN_ERROR(status, errorCode);
            break;
        }

        firstCall = false;
    }

    return errorCode;
}

std::shared_ptr<boost::system::error_code> acquireCredentialsHandle(PCredHandle credentialsHandle) {
    SECURITY_STATUS status;
    std::shared_ptr<boost::system::error_code> errorCode;
    TimeStamp validity;

    status = AcquireCredentialsHandle(
            NULL, /* NULL indicates credentials of the user under whose security context it is executing */
            "Kerberos",
            SECPKG_CRED_OUTBOUND, /* client credential */
            NULL,
            NULL, /* use default credentials */
            NULL, /* not used */
            NULL, /* not used */
            credentialsHandle,
            &validity);
    ASSIGN_SEC_ERROR(status, errorCode);

    return errorCode;
}

std::shared_ptr<boost::system::error_code> freeCredentialsHandle(PCredHandle credentialsHandle) {
    SECURITY_STATUS status;
    std::shared_ptr<boost::system::error_code> errorCode;

    status = FreeCredentialsHandle(credentialsHandle);
    ASSIGN_SEC_ERROR(status, errorCode);

    return errorCode;
}

std::shared_ptr<boost::system::error_code> initializeSecurityContext(const boost::optional<ByteArray>& inputToken, const std::string& servicePrincipalNameString, const PCredHandle credentialsHandle, bool haveContextHandle, PCtxtHandle contextHandle, ULONG contextRequested, ULONG* contextSupported, bool* haveCompleteContext, SafeByteArray& outputToken) {
    SECURITY_STATUS status;
    std::shared_ptr<boost::system::error_code> errorCode;
    SecBufferDesc input;
    SecBufferDesc output;
    SecBuffer inputTokenBuffer;
    SecBuffer outputTokenBuffer;
    TimeStamp validity;

    *haveCompleteContext = false;

    input.ulVersion = 0;
    input.cBuffers = 1;
    input.pBuffers = &inputTokenBuffer;

    inputTokenBuffer.BufferType = SECBUFFER_TOKEN;
    inputTokenBuffer.cbBuffer = 0;
    inputTokenBuffer.pvBuffer = NULL;
    if (inputToken && inputToken->size()) {
        inputTokenBuffer.cbBuffer = inputToken->size();
        inputTokenBuffer.pvBuffer = (void *) vecptr(*inputToken);
    }

    output.ulVersion = 0;
    output.cBuffers = 1;
    output.pBuffers = &outputTokenBuffer;

    outputTokenBuffer.BufferType = SECBUFFER_TOKEN;
    outputTokenBuffer.cbBuffer = 0;
    outputTokenBuffer.pvBuffer = NULL;

    status = InitializeSecurityContext(
            credentialsHandle, /* previously acquired handle */
            haveContextHandle ? contextHandle : NULL, /* use partial context on subsequent calls */
            const_cast<char *>(servicePrincipalNameString.c_str()),
            contextRequested | ISC_REQ_ALLOCATE_MEMORY,
            0, /* not used */
            SECURITY_NETWORK_DREP,
            haveContextHandle ? &input : NULL,
            0, /* not used */
            contextHandle,
            &output,
            contextSupported,
            &validity);
    ASSIGN_SEC_ERROR(status, errorCode); /* errorCode set here will only be returned to caller if there was a non-success status */
    if ((status == SEC_I_COMPLETE_AND_CONTINUE) || (status == SEC_I_COMPLETE_NEEDED)) {
        /* The Windows documentation suggests that this function is used only for Digest and only on the server side, but still asks to call this function for Kerberos clients. Most likely this function will never be called, but including it for compliance with documentation. */
        errorCode = completeAuthToken(contextHandle, &output);
        if (!errorCode) {
            /* success, move on */
        }
        else {
            freeContextBuffer(outputTokenBuffer.pvBuffer);
            return errorCode;
        }
    }
    if ((status == SEC_E_OK) || (status == SEC_I_COMPLETE_NEEDED)) {
        *haveCompleteContext = true;
    }
    if ((status == SEC_E_OK) || (status == SEC_I_COMPLETE_AND_CONTINUE) || (status == SEC_I_COMPLETE_NEEDED) || (status == SEC_I_CONTINUE_NEEDED)) {
        outputToken = createSafeByteArray (static_cast<unsigned char *>(outputTokenBuffer.pvBuffer), outputTokenBuffer.cbBuffer);
        SWIFT_LOG(debug) << "outputToken.size(): " << outputToken.size() << std::endl;
        freeContextBuffer(outputTokenBuffer.pvBuffer);

        return std::shared_ptr<boost::system::error_code>(); /* success */
    }

    return errorCode;
}

std::shared_ptr<boost::system::error_code> deleteSecurityContext(PCtxtHandle contextHandle) {
    SECURITY_STATUS status;
    std::shared_ptr<boost::system::error_code> errorCode;

    status = DeleteSecurityContext(contextHandle);
    ASSIGN_SEC_ERROR(status, errorCode);

    return errorCode;
}

std::shared_ptr<boost::system::error_code> completeAuthToken(const PCtxtHandle contextHandle, PSecBufferDesc token) {
    SECURITY_STATUS status;
    std::shared_ptr<boost::system::error_code> errorCode;

    status = CompleteAuthToken(
            contextHandle, /* partial context */
            token);
    ASSIGN_SEC_ERROR(status, errorCode);

    return errorCode;
}

std::shared_ptr<boost::system::error_code> freeContextBuffer(PVOID contextBuffer) {
    SECURITY_STATUS status;
    std::shared_ptr<boost::system::error_code> errorCode;

    if (contextBuffer == NULL) {
        return errorCode;
    }

    status = FreeContextBuffer(contextBuffer);
    ASSIGN_SEC_ERROR(status, errorCode);

    return errorCode;
}

std::shared_ptr<boost::system::error_code> decryptMessage(const PCtxtHandle contextHandle, const ByteArray& message, SafeByteArray& decrypted) {
    /* Following https://msdn.microsoft.com/en-us/library/windows/desktop/aa380496%28v=vs.85%29.aspx */

    SECURITY_STATUS status;
    std::shared_ptr<boost::system::error_code> errorCode;
    SecBufferDesc inOut;
    SecBuffer messageBuffer[2];
    SafeByteArray inputMessage;
    ULONG qualityOfProtection;

    inOut.ulVersion = SECBUFFER_VERSION;
    inOut.cBuffers = 2;
    inOut.pBuffers = messageBuffer;

    inputMessage = createSafeByteArray (message); /* Make a copy as DecryptMessage decrypts the input in place, overwriting it */
    messageBuffer[0].BufferType = SECBUFFER_STREAM;
    messageBuffer[0].cbBuffer = inputMessage.size();
    messageBuffer[0].pvBuffer = static_cast<void *>(vecptr(inputMessage));

    messageBuffer[1].BufferType = SECBUFFER_DATA;
    messageBuffer[1].cbBuffer = 0;
    messageBuffer[1].pvBuffer = NULL;

    SWIFT_LOG(debug) << "inputMessage.size(): " << inputMessage.size() << std::endl;

    status = DecryptMessage(
            contextHandle,
            &inOut,
            0, /* Don't maintain sequence numbers */
            &qualityOfProtection);
    ASSIGN_SEC_ERROR(status, errorCode);
    if (status == SEC_E_OK) {
        if (qualityOfProtection == SECQOP_WRAP_NO_ENCRYPT) {
            SWIFT_LOG(debug) << "Message was signed only" << std::endl;
        }
        else {
            SWIFT_LOG(debug) << "Message was encrypted" << std::endl;
        }

        SWIFT_LOG(debug) << "messageBuffer[1].cbBuffer: " << messageBuffer[1].cbBuffer << std::endl;

        decrypted = createSafeByteArray (static_cast<unsigned char *>(messageBuffer[1].pvBuffer), messageBuffer[1].cbBuffer);
    }

    return errorCode;
}

std::shared_ptr<boost::system::error_code> encryptMessage(const PCtxtHandle contextHandle, const SecPkgContext_Sizes& sizes, const SafeByteArray& message, SafeByteArray& output) {
    /* Following https://msdn.microsoft.com/en-us/library/windows/desktop/aa380496%28v=vs.85%29.aspx */

    SECURITY_STATUS status;
    std::shared_ptr<boost::system::error_code> errorCode;
    SecBufferDesc inOut;
    SecBuffer messageBuffer[3];
    SafeByteArray securityTrailer(sizes.cbSecurityTrailer);
    SafeByteArray blockSize(sizes.cbBlockSize);
    SafeByteArray inputMessage;

    inOut.ulVersion = SECBUFFER_VERSION;
    inOut.cBuffers = 3;
    inOut.pBuffers = messageBuffer;

    messageBuffer[0].BufferType = SECBUFFER_TOKEN;
    messageBuffer[0].cbBuffer = sizes.cbSecurityTrailer;
    messageBuffer[0].pvBuffer = vecptr(securityTrailer);

    inputMessage = createSafeByteArray (vecptr(message), message.size()); /* Make a copy as EncryptMessage encrypts the input in place, overwriting it */
    messageBuffer[1].BufferType = SECBUFFER_DATA;
    messageBuffer[1].cbBuffer = inputMessage.size();
    messageBuffer[1].pvBuffer = (void *) vecptr(inputMessage);

    messageBuffer[2].BufferType = SECBUFFER_PADDING;
    messageBuffer[2].cbBuffer = sizes.cbBlockSize;
    messageBuffer[2].pvBuffer = vecptr(blockSize);

    SWIFT_LOG(debug) << "sizes.cbSecurityTrailer: " << sizes.cbSecurityTrailer << std::endl;
    SWIFT_LOG(debug) << "inputMessage.size(): " << inputMessage.size() << std::endl;
    SWIFT_LOG(debug) << "sizes.cbBlockSize: " << sizes.cbBlockSize << std::endl;

    status = EncryptMessage(
            contextHandle,
            SECQOP_WRAP_NO_ENCRYPT,
            &inOut,
            0); /* Don't maintain sequence numbers */
    ASSIGN_SEC_ERROR(status, errorCode);
    if (status == SEC_E_OK) {
        unsigned char* pointer;

        SWIFT_LOG(debug) << "messageBuffer[0].cbBuffer: " << messageBuffer[0].cbBuffer << std::endl;
        SWIFT_LOG(debug) << "messageBuffer[1].cbBuffer: " << messageBuffer[1].cbBuffer << std::endl;
        SWIFT_LOG(debug) << "messageBuffer[2].cbBuffer: " << messageBuffer[2].cbBuffer << std::endl;

        output.resize(messageBuffer[0].cbBuffer + messageBuffer[1].cbBuffer + messageBuffer[2].cbBuffer);
        pointer = vecptr(output);
        for (size_t i = 0; i < inOut.cBuffers; i++) {
            if (messageBuffer[i].cbBuffer) {
                memcpy(pointer, messageBuffer[i].pvBuffer, messageBuffer[i].cbBuffer);
                pointer += messageBuffer[i].cbBuffer;
            }
        }
    }

    return errorCode;
}

std::shared_ptr<boost::system::error_code> queryContextAttributes(const PCtxtHandle contextHandle, ULONG attribute, PVOID buffer) {
    SECURITY_STATUS status;
    std::shared_ptr<boost::system::error_code> errorCode;

    status = QueryContextAttributes(
            contextHandle,
            attribute,
            buffer);
    ASSIGN_SEC_ERROR(status, errorCode);

    return errorCode;
}

}
