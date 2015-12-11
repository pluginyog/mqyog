dnl $Id$
dnl config.m4 for extension mqyog

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(mqyog, for mqyog support,
dnl Make sure that the comment is aligned:
dnl [  --with-mqyog             Include mqyog support])

dnl Otherwise use enable:

 PHP_ARG_ENABLE(mqyog, whether to enable mqyog support,
dnl Make sure that the comment is aligned:
 [  --enable-mqyog           Enable mqyog support])

if test "$PHP_MQYOG" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-mqyog -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/mqyog.h"  # you most likely want to change this
  dnl if test -r $PHP_MQYOG/$SEARCH_FOR; then # path given as parameter
  dnl   MQYOG_DIR=$PHP_MQYOG
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for mqyog files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       MQYOG_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$MQYOG_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the mqyog distribution])
  dnl fi

  dnl # --with-mqyog -> add include path
  dnl PHP_ADD_INCLUDE($MQYOG_DIR/include)

  dnl # --with-mqyog -> check for lib and symbol presence
  dnl LIBNAME=mqyog # you may want to change this
  dnl LIBSYMBOL=mqyog # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $MQYOG_DIR/lib, MQYOG_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_MQYOGLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong mqyog lib version or lib not found])
  dnl ],[
  dnl   -L$MQYOG_DIR/lib -lm
  dnl ])
  dnl
  dnl PHP_SUBST(MQYOG_SHARED_LIBADD)

  PHP_NEW_EXTENSION(mqyog, mqyog.c, $ext_shared)
fi
