# Author: Remko Tronçon

AC_DEFUN([AX_QT],
[
	AC_ARG_WITH(
		[qt],
		AC_HELP_STRING([--with-qt=@<:@ARG@:>@],[Path to Qt installation]),
		[
			WITH_QT="$withval"
			if test "$withval" != "no" -a "$withval" != "yes"; then
        QT_PATH="$withval/bin"
      else
        QT_PATH="$PATH"
			fi
		],
		[
      WITH_QT="yes"
      QT_PATH="$PATH"
    ])

	if test "$WITH_QT" != "no"; then
    AC_PATH_PROG(QMAKE, qmake, [], $QT_PATH)
    if test "$QMAKE"; then
      HAVE_QT=yes
    fi
	fi
])
