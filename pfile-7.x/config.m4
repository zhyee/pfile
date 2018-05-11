dnl $Id$
dnl config.m4 for extension pfile

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(pfile, for pfile support,
dnl Make sure that the comment is aligned:
dnl [  --with-pfile             Include pfile support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(pfile, whether to enable pfile support,
dnl Make sure that the comment is aligned:
[  --enable-pfile           Enable pfile support])

if test "$PHP_PFILE" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-pfile -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/pfile.h"  # you most likely want to change this
  dnl if test -r $PHP_PFILE/$SEARCH_FOR; then # path given as parameter
  dnl   PFILE_DIR=$PHP_PFILE
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for pfile files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       PFILE_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$PFILE_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the pfile distribution])
  dnl fi

  dnl # --with-pfile -> add include path
  dnl PHP_ADD_INCLUDE($PFILE_DIR/include)

  dnl # --with-pfile -> check for lib and symbol presence
  dnl LIBNAME=pfile # you may want to change this
  dnl LIBSYMBOL=pfile # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $PFILE_DIR/$PHP_LIBDIR, PFILE_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_PFILELIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong pfile lib version or lib not found])
  dnl ],[
  dnl   -L$PFILE_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(PFILE_SHARED_LIBADD)

  PHP_NEW_EXTENSION(pfile, pfile.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
