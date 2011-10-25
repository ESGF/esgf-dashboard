#AC_FIND_HEADER(header_name, header, path)
#-----------------------------------------
#Find header file in path and create CFLAGS
AC_DEFUN([AC_FIND_HEADER],
[
	find=""
	dnl I have to set TMP="" since these variables have a global visibility
	TMP=""
	dnl Check on the third parameter
	path_search="none";
	if test "x$3" = "x"; then
		AC_MSG_WARN([No user defined paths found. Searching in the default path])
		path_search="/usr/include"
	else path_search="$3";
	fi
	for dir in $path_search; do
#AC_MSG_CHECKING([for $2 in $dir])
		AC_CHECK_FILE([$dir/$2],
				[
				  TMP="-I$dir"
				  find="$dir"
				  break
				])
# 		if test -f "$dir/$2"; then
# 			TMP="-I$dir"
# 			find="yes"
# 			AC_MSG_RESULT(yes)
# 			break
# 		else AC_MSG_RESULT(no)
# 		fi
	done
AC_SUBST([$1_CFLAGS], [$TMP])
AC_SUBST([$1_FIND], [$find])
])

#AC_FIND_LIB(lib, path)
#----------------------------------------
#Find lib file in path and create LDFLAGS
AC_DEFUN([AC_FIND_LIB],
[
	lib_la="lib$1.la"
	lib_a="lib$1.a"
	lib_so="lib$1.so"
        lib_dylib="lib$1.dylib"
	lib="-l$1"
	find=""
	dnl I have to set TMP="" since these variables have a global visibility
	TMP=""
	dnl Check on the second parameter
	path_search="none";
	if test "x$2" = "x"; then
		AC_MSG_WARN([No user defined paths found. Searching in the default path])
		path_search="/usr/lib"
	else path_search="$2";
	fi
	for dir in $path_search; do
		LIB=$dir/$lib_la
		TMP="-L$dir $lib"
		find="$dir"
		AC_CHECK_FILE([$LIB],
				[
				  break
				])
		LIB=$dir/$lib_so
		AC_CHECK_FILE([$LIB],
				[
				  break
				])
                LIB=$dir/$lib_dylib
                AC_CHECK_FILE([$LIB],
                                [
                                  break
                                ])
		LIB=$dir/$lib_a
		AC_CHECK_FILE([$LIB],
				[
				  break
				])
		LIB=""
		TMP=""
		find=""
	done
AC_SUBST([$1_LDFLAGS], [$TMP])
AC_SUBST([$1_LIB_FIND], [$find])
AC_SUBST([$1_LIB_FILE], [$LIB])
])
