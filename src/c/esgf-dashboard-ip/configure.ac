#AC_PREREQ([2.60])
AC_INIT(esgf-dashboard-ip,1.0.0, sandro.fiore@unisalento.it)
AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC
AC_PROG_CC_C99

AM_PROG_CC_STDC

AM_PROG_CC_C_O

# Checks for libraries and header files.

AC_ARG_WITH(postgres_header_path,
           [  --with-postgres-header-path=PATH          Location of libpq-fe.h],
           [
                postgres_header_path="$withval"
           ])
AC_ARG_WITH(postgres_lib_path,
           [  --with-postgres-lib-path=PATH             Location of libpq.a],
           [
                postgres_lib_path="$withval"
           ])
AC_ARG_WITH(postgres_path,
           [  --with-postgres-path=PATH                 Location of postgresql],
           [
                postgres_path="$withval"
           ])
postgresql="yes"

AC_ARG_ENABLE(postgresql,
           [  --disable-postgresql                      Disables postgresql. (Enabled by default)],
           [
                postgresql="no"
           ],
           [
                AC_FIND_HEADER(postgres, libpq-fe.h, [$postgres_header_path $postgres_path/include /usr/local/pgsql/include /usr/include /usr/include/pgsql /usr/local/include /usr/local/include/pgsql])
                if test "x$postgres_FIND" != "x"; then
                        AC_FIND_LIB(pq, [$postgres_lib_path $postgres_path/lib /usr/local/pgsql/lib /usr/lib /usr/lib/pgsql /usr/local/lib /usr/local/lib/pgsql])
                        if test "x$pq_LIB_FIND" != "x"; then
                                postgresql="yes"
                        else
                                postgresql="no"
                                AC_MSG_WARN(POSTGRESQL library not found!)
                        fi
                else
                        postgresql="no"
                        AC_MSG_WARN(POSTGRESQL header file not found!)
                fi
           ]
           )

AC_ARG_WITH([xml2-config], [  --with-xml2-config      libxml2 config program],
if test "x${with_xml2_config}" = "xno" ; then
  XML2_CONFIG=
else
  XML2_CONFIG="${with_xml2_config}"
fi
,
  XML2_CONFIG=
)
if test "x${XML2_CONFIG}" != "x" ; then
   if test ! -x "${XML2_CONFIG}" ; then
     AC_MSG_ERROR([Unusable or missing xml2-config: ${XML2_CONFIG}])
   fi
else
  AC_PATH_PROG([XML2_CONFIG], [xml2-config], , [${PATH}])
  if test "x${XML2_CONFIG}" = "x" ; then
    AC_MSG_ERROR([Cannot configure without xml2-config])
  fi
fi


AC_SUBST(XML2_CONFIG)



AC_ARG_WITH(geoip_prefix_path,
           [  --with-geoip-prefix-path=PATH          Location of GeoIP],
           [
                geoip_prefix_path="$withval"
           ])

GEOIP_PREFIX_PATH=$"${geoip_prefix_path}"
AC_SUBST(GEOIP_PREFIX_PATH)

#AM_CONDITIONAL(DBMS_POSTGRES, test "x$postgresql" = "xyes")

INSTALLATION_PATH=$"${prefix}"
AC_SUBST(INSTALLATION_PATH)

AC_CONFIG_FILES([Makefile
                 src/Makefile  bin/ip.service]) # bin/ip.service.local])
AC_OUTPUT
