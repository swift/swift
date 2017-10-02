/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#define SECURITY_WIN32
#include <Windows.h>
#include <Sspi.h>

#include <Swiften/Base/API.h>
#include <boost/signals2.hpp>
#include <Swiften/Base/SafeByteArray.h>

namespace Swift {
    /**
     * Retrieves the names & Windows server domain of the user associated
     * with the calling thread.
     *
     * @param userName Will return the user name in the form "DOMAIN\user"
     * @param clientName Will return the client name in the form "user"
     * @param serverName Will return the server name in the form "DOMAIN"
     *
     * @return NULL for success, otherwise the error code returned by
     * Windows.
     */
    SWIFTEN_API std::shared_ptr<boost::system::error_code> getUserNameEx(std::string& userName, std::string& clientName, std::string& serverName);

    /**
     * Retrieves the handle to preexisting client credentials for the
     * Kerberos security package that were established through a system
     * logon.
     * freeCredentialsHandle() should be called if this function is
     * successful and when credentials are no longer needed.
     *
     * @param credentialsHandle Pointer to the returned credentials handle.
     *
     * @return NULL for success, otherwise the error code returned by
     * Windows.
     */
    SWIFTEN_API std::shared_ptr<boost::system::error_code> acquireCredentialsHandle(PCredHandle credentialsHandle);

    /**
     * Releases the credentials handle obtained by the
     * acquireCredentialsHandle() function.
     * freeCredentialsHandle() should be called when credentials are no
     * longer needed.
     *
     * @param credentialsHandle Pointer to the credentials handle.
     *
     * @return NULL for success, otherwise the error code returned by
     * Windows.
     */
    SWIFTEN_API std::shared_ptr<boost::system::error_code> freeCredentialsHandle(PCredHandle credentialsHandle);

    /**
     * Builds the security context between the client and remote peer.
     * Kerberos security package that were established through a system
     * logon.
     *
     * @param inputToken NULL or empty on the first call, otherwise the
     *    token returned by the server.
     * @param servicePrincipalNameString Service principal name of the
     *    server.
     * @param credentialsHandle Pointer to the credentials handle acquired
     *    before.
     * @param haveContextHandle False on the first call to this function,
     *    true otherwise.
     * @param contextHandle Pointer to the context handle returned on the
     *    first call and passed on subsequent calls.
     * @param contextRequested Context related requests by the caller. See
     *    the Windows API InitializeSecurityContext for allowed values.
     * @param contextSupported Pointer to context related attributes
     *    returned when context is completely established (when
     *    haveCompleteContext contains true). See the Windows API
     *    InitializeSecurityContext for allowed values.
     * @param haveCompleteContext Pointer to boolean - this will only be
     *    true on return when the context is completely established and
     *    there is no need to call this function again.
     * @param outputToken Returned security token to be sent to the server,
     *    may be empty.
     *
     * @return NULL for success, otherwise the error code returned by
     * Windows.
     */
    SWIFTEN_API std::shared_ptr<boost::system::error_code> initializeSecurityContext(const boost::optional<ByteArray>& inputToken, const std::string& servicePrincipalNameString, const PCredHandle credentialsHandle, bool haveContextHandle, PCtxtHandle contextHandle, ULONG contextRequested, ULONG* contextSupported, bool* haveCompleteContext, SafeByteArray& outputToken);

    /**
     * Releases the context handle obtained by the
     * initializeSecurityContext() function.
     * deleteSecurityContext() should be called when the context is no
     * longer needed.
     *
     * @param contextHandle Pointer to the context handle.
     *
     * @return NULL for success, otherwise the error code returned by
     * Windows.
     */
    SWIFTEN_API std::shared_ptr<boost::system::error_code> deleteSecurityContext(PCtxtHandle contextHandle);

    /**
     * Completes an authentication token for a partial security context.
     *
     * @param contextHandle Pointer to the context handle.
     * @param token authentication token.
     *
     * @return NULL for success, otherwise the error code returned by
     * Windows.
     */
    SWIFTEN_API std::shared_ptr<boost::system::error_code> completeAuthToken(const PCtxtHandle contextHandle, PSecBufferDesc token);

    /**
     * Frees a memory buffer allocated by the security package.
     *
     * @param contextBuffer Pointer to buffer to be freed.
     *
     * @return NULL for success, otherwise the error code returned by
     * Windows.
     */
    SWIFTEN_API std::shared_ptr<boost::system::error_code> freeContextBuffer(PVOID contextBuffer);

    /**
     * Decrypt message (assumes that sequence numbers are not maintained).
     *
     * @param contextHandle Pointer to the context handle.
     * @param message Message to decrypt.
     * @param decrypted Returned decrypted message.
     *
     * @return NULL for success, otherwise the error code returned by
     * Windows.
     */
    SWIFTEN_API std::shared_ptr<boost::system::error_code> decryptMessage(const PCtxtHandle contextHandle, const ByteArray& message, SafeByteArray& decrypted);

    /**
     * Produces a header or trailer for the message but does not encrypt it
     * (also assumes that sequence numbers are not maintained).
     *
     * @param contextHandle Pointer to the context handle.
     * @param sizes SecPkgContext_Sizes obtained for the context.
     * @param message Input message.
     * @param output Returned output message.
     *
     * @return NULL for success, otherwise the error code returned by
     * Windows.
     */
    SWIFTEN_API std::shared_ptr<boost::system::error_code> encryptMessage(const PCtxtHandle contextHandle, const SecPkgContext_Sizes& sizes, const SafeByteArray& message, SafeByteArray& output);

    /**
     * Queries the security package for attributes of the security context.
     *
     * @param contextHandle Pointer to the context handle.
     * @param attribute Attribute to query. See the Windows API
     *    QueryContextAttributes for allowed values.
     * @param buffer Pointer to a structure that receives the output.
     *    The type of structure depends on the queried attribute and
     *    memory for it must be allocated by caller. If the SSP allocates
     *    any memory required to hold some members, that memory should be
     *    freed using the function freeContextBuffer(). See the Windows
     *    API QueryContextAttributes for details.
     *
     * @return NULL for success, otherwise the error code returned by
     * Windows.
     */
    SWIFTEN_API std::shared_ptr<boost::system::error_code> queryContextAttributes(const PCtxtHandle contextHandle, ULONG attribute, PVOID buffer);

}
