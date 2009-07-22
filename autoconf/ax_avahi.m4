# Author: Remko Tronçon

AC_DEFUN([AX_AVAHI],
[
	AC_ARG_WITH(
		[avahi],
		AC_HELP_STRING([--with-avahi=@<:@ARG@:>@],[Path to Avahi installation]),
		[
			WITH_AVAHI="$withval"
			if test "$withval" != "no" -a "$withval" != "yes"; then
				AVAHI_CPPFLAGS="-I$withval/include"
				AVAHI_LDFLAGS="-L$withval/lib"
				CPPFLAGS="$CPPFLAGS $AVAHI_CPPFLAGS"
				LDFLAGS="$LDFLAGS $AVAHI_LDFLAGS"
			fi
		],
		[WITH_AVAHI="yes"])

	if test "$WITH_AVAHI" != "no"; then
		AC_CHECK_HEADER(avahi-client/client.h,
			[
				HAVE_AVAHI=yes
				# TODO: Check if this lib exists
				AVAHI_LIBS="-lavahi-client"
			], 
			[])
	fi
])
