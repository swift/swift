/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef __DNS_ANDROID_H__
#define __DNS_ANDROID_H__

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! Result codes for dns_android_server_get */
typedef enum {
  DNS_ANDROID_SUCCESS = 0,
  DNS_ANDROID_FAILURE
} dns_android_result;

/*!
  \brief Retrieve the number of dns servers available.
  \return Zero in case of error.
*/
size_t dns_android_server_count();

/*!
  \brief Retrieve the address of a dns server
  \param index The server index between zero and the number returned by the count function
  \param buffer Output buffer into which the null terminated dns server string is written
  \param length Size in bytes available in the output buffer
  \return Error code. The buffer contents are invalid if an error occurs.
*/
dns_android_result dns_android_server_get(size_t index, char* buffer, size_t length);

#ifdef __cplusplus
}
#endif

#endif // __DNS_ANDROID_H__
