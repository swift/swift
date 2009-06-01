# Author: Remko Tronçon

AC_DEFUN([AX_OPENSSL],
[
	AC_ARG_WITH(
		[openssl],
		AC_HELP_STRING([--with-openssl=@<:@ARG@:>@],[Path to OpenSSL installation]),
		[
			WITH_OPENSSL="$withval"
			if test "$withval" != "no" -a "$withval" != "yes"; then
				OPENSSL_CPPFLAGS="-I$withval/include"
				case $host in
					*-*-cygwin*|*-mingw32*)
						OPENSSL_LIBS="-L$withval/lib/MinGW -leay32 $withval/lib/MinGW/ssleay32.a"
						;;
					*)
						OPENSSL_LIBS="-L$withval/lib -lssl -lcrypto"
				esac
				CPPFLAGS="$CPPFLAGS $OPENSSL_CPPFLAGS"
				LDFLAGS="$LDFLAGS $OPENSSL_LIBS"
			fi
		],
		[
			WITH_OPENSSL="yes"
			case $host in
				*-*-cygwin*|*-mingw32*)
					AC_MSG_ERROR([--with-openssl parameter is required on Windows])
				;;
				*)
					OPENSSL_LIBS="-lssl -lcrypto"
			esac
		])

	if test "$WITH_OPENSSL" != "no"; then
		AC_CHECK_HEADER(openssl/ssl.h, [HAVE_OPENSSL=yes], [])
	fi
])
