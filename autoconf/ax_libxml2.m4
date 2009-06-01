# Author: Remko Tronçon

AC_DEFUN([AX_LIBXML],
[
	AC_ARG_WITH(
		[libxml],
		AC_HELP_STRING([--with-libxml=@<:@ARG@:>@],[Path to LibXML installation]),
		[
			WITH_LIBXML="$withval"
			if test "$withval" != "no" -a "$withval" != "yes"; then
				LIBXML_CPPFLAGS="-I$withval/include/libxml2"
				LIBXML_LDFLAGS="-L$withval/lib"
				CPPFLAGS="$CPPFLAGS $LIBXML_CPPFLAGS"
				LDFLAGS="$LDFLAGS $LIBXML_LDFLAGS"
			fi
		],
		[WITH_LIBXML="yes"])

	if test "$WITH_LIBXML" != "no"; then
		AC_CHECK_HEADER(libxml/parser.h, 
			[
				HAVE_LIBXML=yes
				# TODO: Check if this lib exists
				LIBXML_LIBS="-lxml2"
			], 
			[])
	fi
])
