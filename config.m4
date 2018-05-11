dnl $Id$
dnl config.m4 for extension rockfile

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(rockfile, for rockfile support,
dnl Make sure that the comment is aligned:
dnl [  --with-rockfile             Include rockfile support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(rockfile, whether to enable rockfile support,
dnl Make sure that the comment is aligned:
[  --enable-rockfile           Enable rockfile support])

if test "$PHP_ROCKFILE" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-rockfile -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/rockfile.h"  # you most likely want to change this
  dnl if test -r $PHP_ROCKFILE/$SEARCH_FOR; then # path given as parameter
  dnl   ROCKFILE_DIR=$PHP_ROCKFILE
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for rockfile files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       ROCKFILE_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$ROCKFILE_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the rockfile distribution])
  dnl fi

  dnl # --with-rockfile -> add include path
  dnl PHP_ADD_INCLUDE($ROCKFILE_DIR/include)

  dnl # --with-rockfile -> check for lib and symbol presence
  dnl LIBNAME=rockfile # you may want to change this
  dnl LIBSYMBOL=rockfile # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $ROCKFILE_DIR/$PHP_LIBDIR, ROCKFILE_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_ROCKFILELIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong rockfile lib version or lib not found])
  dnl ],[
  dnl   -L$ROCKFILE_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(ROCKFILE_SHARED_LIBADD)

  PHP_NEW_EXTENSION(rockfile, rockfile.c, $ext_shared)
fi
