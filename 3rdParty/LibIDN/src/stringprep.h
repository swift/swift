/* stringprep.h --- Header file for stringprep functions.
 * Copyright (C) 2002, 2003, 2004, 2005, 2006, 2007, 2008  Simon Josefsson
 *
 * This file is part of GNU Libidn.
 *
 * GNU Libidn is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * GNU Libidn is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GNU Libidn; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 *
 */

#ifndef STRINGPREP_H
# define STRINGPREP_H

# include <stddef.h>		/* size_t */
# include <unistd.h>		/* ssize_t */
# include <idn-int.h>		/* uint32_t */

/* Libidn Windows DLL.  Only needed when this file is used in Visual
   Studio.  Export and import happens automatically in MinGW. */
# ifndef IDNA_API
#  if defined(_MSC_VER) && !defined(IDNA_STATIC)
#   ifdef IDNA_EXPORTS
#    define IDNA_API __declspec(dllexport)
#   else
#    define IDNA_API __declspec(dllimport)
#   endif
#  else
#   define IDNA_API
#  endif
# endif

# ifdef __cplusplus
extern "C"
{
# endif

# define STRINGPREP_VERSION "1.11"

/* Error codes. */
  typedef enum
  {
    STRINGPREP_OK = 0,
    /* Stringprep errors. */
    STRINGPREP_CONTAINS_UNASSIGNED = 1,
    STRINGPREP_CONTAINS_PROHIBITED = 2,
    STRINGPREP_BIDI_BOTH_L_AND_RAL = 3,
    STRINGPREP_BIDI_LEADTRAIL_NOT_RAL = 4,
    STRINGPREP_BIDI_CONTAINS_PROHIBITED = 5,
    /* Error in calling application. */
    STRINGPREP_TOO_SMALL_BUFFER = 100,
    STRINGPREP_PROFILE_ERROR = 101,
    STRINGPREP_FLAG_ERROR = 102,
    STRINGPREP_UNKNOWN_PROFILE = 103,
    /* Internal errors. */
    STRINGPREP_NFKC_FAILED = 200,
    STRINGPREP_MALLOC_ERROR = 201
  } Stringprep_rc;

/* Flags used when calling stringprep(). */
  typedef enum
  {
    STRINGPREP_NO_NFKC = 1,
    STRINGPREP_NO_BIDI = 2,
    STRINGPREP_NO_UNASSIGNED = 4
  } Stringprep_profile_flags;

/* Steps in a stringprep profile. */
  typedef enum
  {
    STRINGPREP_NFKC = 1,
    STRINGPREP_BIDI = 2,
    STRINGPREP_MAP_TABLE = 3,
    STRINGPREP_UNASSIGNED_TABLE = 4,
    STRINGPREP_PROHIBIT_TABLE = 5,
    STRINGPREP_BIDI_PROHIBIT_TABLE = 6,
    STRINGPREP_BIDI_RAL_TABLE = 7,
    STRINGPREP_BIDI_L_TABLE = 8
  } Stringprep_profile_steps;

# define STRINGPREP_MAX_MAP_CHARS 4

  struct Stringprep_table_element
  {
    uint32_t start;
    uint32_t end;		/* 0 if only one character */
    uint32_t map[STRINGPREP_MAX_MAP_CHARS];	/* NULL if end is not 0 */
  };
  typedef struct Stringprep_table_element Stringprep_table_element;

  struct Stringprep_table
  {
    Stringprep_profile_steps operation;
    Stringprep_profile_flags flags;
    const Stringprep_table_element *table;
  };
  typedef struct Stringprep_table Stringprep_profile;

  struct Stringprep_profiles
  {
    const char *name;
    const Stringprep_profile *tables;
  };
  typedef struct Stringprep_profiles Stringprep_profiles;

  extern IDNA_API const Stringprep_profiles stringprep_profiles[];

/* Profiles */
  extern IDNA_API const Stringprep_table_element stringprep_rfc3454_A_1[];
  extern IDNA_API const Stringprep_table_element stringprep_rfc3454_B_1[];
  extern IDNA_API const Stringprep_table_element stringprep_rfc3454_B_2[];
  extern IDNA_API const Stringprep_table_element stringprep_rfc3454_B_3[];
  extern IDNA_API const Stringprep_table_element stringprep_rfc3454_C_1_1[];
  extern IDNA_API const Stringprep_table_element stringprep_rfc3454_C_1_2[];
  extern IDNA_API const Stringprep_table_element stringprep_rfc3454_C_2_1[];
  extern IDNA_API const Stringprep_table_element stringprep_rfc3454_C_2_2[];
  extern IDNA_API const Stringprep_table_element stringprep_rfc3454_C_3[];
  extern IDNA_API const Stringprep_table_element stringprep_rfc3454_C_4[];
  extern IDNA_API const Stringprep_table_element stringprep_rfc3454_C_5[];
  extern IDNA_API const Stringprep_table_element stringprep_rfc3454_C_6[];
  extern IDNA_API const Stringprep_table_element stringprep_rfc3454_C_7[];
  extern IDNA_API const Stringprep_table_element stringprep_rfc3454_C_8[];
  extern IDNA_API const Stringprep_table_element stringprep_rfc3454_C_9[];
  extern IDNA_API const Stringprep_table_element stringprep_rfc3454_D_1[];
  extern IDNA_API const Stringprep_table_element stringprep_rfc3454_D_2[];

  /* Nameprep */

  extern IDNA_API const Stringprep_profile stringprep_nameprep[];

# define stringprep_nameprep(in, maxlen)			\
  stringprep(in, maxlen, 0, stringprep_nameprep)

# define stringprep_nameprep_no_unassigned(in, maxlen)			\
  stringprep(in, maxlen, STRINGPREP_NO_UNASSIGNED, stringprep_nameprep)

  /* SASL */

  extern IDNA_API const Stringprep_profile stringprep_saslprep[];
  extern IDNA_API const Stringprep_profile stringprep_plain[];
  extern IDNA_API const Stringprep_profile stringprep_trace[];

# define stringprep_plain(in, maxlen)		\
  stringprep(in, maxlen, 0, stringprep_plain)

  /* Kerberos */

  extern IDNA_API const Stringprep_profile stringprep_kerberos5[];

# define stringprep_kerberos5(in, maxlen)		\
  stringprep(in, maxlen, 0, stringprep_kerberos5)

  /* XMPP */

  extern IDNA_API const Stringprep_profile stringprep_xmpp_nodeprep[];
  extern IDNA_API const Stringprep_profile stringprep_xmpp_resourceprep[];
  extern IDNA_API const Stringprep_table_element stringprep_xmpp_nodeprep_prohibit[];

# define stringprep_xmpp_nodeprep(in, maxlen)		\
  stringprep(in, maxlen, 0, stringprep_xmpp_nodeprep)
# define stringprep_xmpp_resourceprep(in, maxlen)		\
  stringprep(in, maxlen, 0, stringprep_xmpp_resourceprep)

  /* iSCSI */

  extern IDNA_API const Stringprep_profile stringprep_iscsi[];

# define stringprep_iscsi(in, maxlen)		\
  stringprep(in, maxlen, 0, stringprep_iscsi)

  /* API */

  extern IDNA_API int stringprep_4i (uint32_t * ucs4, size_t * len,
				     size_t maxucs4len,
				     Stringprep_profile_flags flags,
				     const Stringprep_profile * profile);
  extern IDNA_API int stringprep_4zi (uint32_t * ucs4, size_t maxucs4len,
				      Stringprep_profile_flags flags,
				      const Stringprep_profile * profile);
  extern IDNA_API int stringprep (char *in, size_t maxlen,
				  Stringprep_profile_flags flags,
				  const Stringprep_profile * profile);

  extern IDNA_API int stringprep_profile (const char *in,
					  char **out,
					  const char *profile,
					  Stringprep_profile_flags flags);

  extern IDNA_API const char *stringprep_strerror (Stringprep_rc rc);

  extern IDNA_API const char *stringprep_check_version (const char
							*req_version);

/* Utility */

  extern IDNA_API int stringprep_unichar_to_utf8 (uint32_t c, char *outbuf);
  extern IDNA_API uint32_t stringprep_utf8_to_unichar (const char *p);

  extern IDNA_API uint32_t *stringprep_utf8_to_ucs4 (const char *str,
						     ssize_t len,
						     size_t * items_written);
  extern IDNA_API char *stringprep_ucs4_to_utf8 (const uint32_t * str,
						 ssize_t len,
						 size_t * items_read,
						 size_t * items_written);

  extern IDNA_API char *stringprep_utf8_nfkc_normalize (const char *str,
							ssize_t len);
  extern IDNA_API uint32_t *stringprep_ucs4_nfkc_normalize (uint32_t * str,
							    ssize_t len);

  extern IDNA_API const char *stringprep_locale_charset (void);
  extern IDNA_API char *stringprep_convert (const char *str,
					    const char *to_codeset,
					    const char *from_codeset);
  extern IDNA_API char *stringprep_locale_to_utf8 (const char *str);
  extern IDNA_API char *stringprep_utf8_to_locale (const char *str);

# ifdef __cplusplus
}
# endif

#endif				/* STRINGPREP_H */
