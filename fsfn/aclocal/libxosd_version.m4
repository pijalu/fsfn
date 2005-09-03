dnl AM_CHECK_LIBXOSD
dnl Checks for LIBXOSD (>= 1.0.0)
AC_DEFUN(AM_CHECK_LIBXOSD,
[
  if test ! x$xosd_libdir = x; then
      LIBS="$LIBS -L$xosd_libdir"
  fi
  if test ! x$xosd_incdir = x; then
      CPPFLAGS="$CPPFLAGS -I$xosd_incdir"
  fi

  if test "x$enable_xosd" = "xyes"
  then
    dnl
    dnl Check its version
    dnl
    AC_MSG_CHECKING(for version of libxosd)
    CFLAGS="$CFLAGS $LIBXOSD_CFLAGS"
    LIBS="$LIBS $LIBXOSD_LIBS"
    dnl Check for version >= 2.2.0
    AC_TRY_LINK(
      [
        #include <xosd.h>
      ],
      [
        void test()
        {
          xosd* t = xosd_create(0);
	  xosd_set_shadow_colour(t, "BLUE");
        }
      ],
      [
        AC_MSG_RESULT([>= 2.2.0, ok])
        AC_DEFINE(HAVE_LIBXOSD_VERSION, 20200)
      ],
      [
      dnl Check for version >= 2.0.0
      AC_TRY_LINK(
	[
	  #include <xosd.h>
	],
	[
	  void test()
	  {
	    xosd* t = xosd_create(0);
	  }
	],
	[
	  AC_MSG_RESULT([>= 2.0.0, ok])
	  AC_DEFINE(HAVE_LIBXOSD_VERSION, 20000)
	],
	[
	  dnl Check for version >= 1.0.0 and < 2.0.0
	  AC_TRY_LINK(
	    [
	      #include <xosd.h>
	    ],
	    [
	      void test()
	      {
		xosd* t = xosd_init("font", "colour", 0, XOSD_top, 0, 0, 0);
	      }
	    ],
	    [
	      AC_MSG_RESULT([>= 1.0.0, ok])
	      AC_DEFINE(HAVE_LIBXOSD_VERSION, 10000)
	    ],
	    [
	      dnl Check for version >= 0.7.0 and < 1.0.0
	      dnl xosd < 1.0.0 has no xosd-config script. Lets hope, that is installed to the usual place.
	      CFLAGS="$CFLAGS -g -O2 -I/usr/X11R6/include -I/usr/include"
	      LIBS="$LIBS -L/usr/lib -L/usr/X11R6/lib -lxosd -lpthread -lXt -lXext -lX11 -lSM -lICE"
	      AC_TRY_LINK(
		[
		  #include <xosd.h>
		],
		[
		  void test()
		  {
		    xosd* t = xosd_init("font", "colour", 0, XOSD_top, 0, 0);
		  }
		],
		[
		  AC_MSG_RESULT([>= 0.7.0, ok])
		  AC_DEFINE(HAVE_LIBXOSD, 1, [Defined to 1 if you have the xosd library])
		  AC_DEFINE(HAVE_LIBXOSD_VERSION, 700, [Define to version number of the xosd library.
		  Numbering scheme:   700 >= 0.7.0 ; 10000 >= 1.0.0 ; 20000 >= 2.0.0 ; 20200 >= 2.2.0])
		],
		[
		  AC_MSG_RESULT([< 0.7.0, failed])
		  echo "*** The version of XOSD library installed is not 0.7.0 or"
		  echo "*** above, make sure the correct version is installed."
		  exit 1
		]
	       )
	    ]
	   )
	]
      )
    ]
  )
  fi
])
