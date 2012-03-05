#! /bin/bash
#  -*- Mode: Shell-script -*-
# ----------------------------------------------------------------------
timestamp=$(set -- \
  Time-stamp:        "2012-02-25 13:07:56 bkorb"
  echo ${2} | sed 's/:[0-9][0-9] .*//;s/[^0-9]//g')
##
## Author:          Bruce Korb <bkorb@gnu.org>
## Time-stamp:      "2010-08-13 16:36:20 bkorb"
##
##  This file is part of AutoGen.
##  AutoGen Copyright (c) 1992-2012 by Bruce Korb - all rights reserved
##
## AutoGen is free software: you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by the
## Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## AutoGen is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
## See the GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License along
## with this program.  If not, see <http://www.gnu.org/licenses/>.

declare -r prog=$(basename ${0} .sh)
declare -r progdir=$(\cd $(dirname $0) >/dev/null && pwd -P)
declare -r program=${progdir}/$(basename ${0})
declare -r progpid=$$

die() {
    echo "mkconfig fatal error:  $*" >&2
    kill -TERM $progpid
    sleep 1
    kill -9 $progpid
    exit 1
}

init() {
    ${VERBOSE:-false} && set -x
    test ${#progdir} -le 1 && \
        die "Cannot locate source directory for $prog"
    TMPDIR=$(\cd ${TMPDIR:-/tmp} >/dev/null && pwd -P)
    tmpag=$(mktemp -d ${TMPDIR}/agcfg-XXXXXX)
    test -d "${tmpag}" || die "Cannot make directory ${tmpag}"
    declare logf=${tmpag}/$prog-log.txt
    exec 4> ${logf}
    BASH_XTRACEFD=4
    set -x
    trap "echo preserving temp directory: ${tmpag}" EXIT

    cd ${progdir}
    nl=$'\n'
}

copy_files() {
    declare f= g=
    cd ${progdir}
    git ls-files | while read f
    do
        case "$f" in
        mkconfig.sh | noag-boot.sh ) : ;;
        */* ) g=$(dirname $f)
            test -d $tmpag/$g || {
                mkdir -p $tmpag/$g || die "cannot mkdir $tmpag/$g"
            }
            ;;
        esac
        cp -fp $f $tmpag/$f || die "cannot copy $f"
    done

    cd ${tmpag}

    find * -type f | xargs chmod u+w
    find * -type d | xargs chmod 755
}

run_bootstrap() {
    declare bstr=$(set -- c*f*g/bootstrap ; echo $1)
    test -f ${bstr} || die "Cannot locate bootstrap file"
    PS4='>bs> ' sh $bstr || die "error:  bootstrap failed"
}

get_gen_list() {
    GENLIST=''
    PROTOLIST=''

    for f in $(find * -type f | sort | egrep -v '^stamp-h|/\.fsm\.')
    do
        test -f ${progdir}/${f} && continue

        case "$f" in
        */proto.h   )
            PROTOLIST="${PROTOLIST}${f}${nl}" ;;

        autoopts/*autoopts*.in )
            GENLIST="${GENLIST}${f}${nl}" ;;

        configure   | \
        config*.tmp | \
        *.in        | \
        aclocal*    | \
        autom4*     ) echo "skipping $f" >&2 ;;

        * )
            GENLIST="${GENLIST}${f}${nl}"
            ;;
        esac
    done
}

mk_noag_boot() {
    #  Make sure the new file removes the current collection of files
    #
    declare cfgfile=$(mktemp ${TMPDIR}/noag-boot-XXXXXX.sh)
    exec 5> ${cfgfile}

    echo '#! /bin/sh' >&5
    sed -n '/^##/p;/^die()/q' ${program} >&5

    cat <<- '_EOF_' >&5
	#
	#  DO NOT EDIT THIS FILE
	#
	#  It is generated by ${prog}.  Make necessary changes there.
	#
	# ----------------------------------------------------------------------
	die() {
	  echo "noag-boot error: $*"
	  exit 1
	}

	SVDIR=`pwd`
	cd `echo $0 | sed 's;/[^/]*$;;'`
	SRCDIR=`pwd`

	test -f ./configure.ac || \
	  die cannot locate top source directory for AutoGen

	if [ -z "${VERBOSE}" ]
	then VERBOSE=false ; VERBOSE_ARG=""
	else VERBOSE=true  ; VERBOSE_ARG="-x" ; set -x ; fi
	export VERBOSE VERBOSE_ARG

	#  Make sure we have all the needed directories and none of the
	#  files we've got in this archive
	#
	_EOF_

    for f in ${GENLIST} ${PROTOLIST}
    do echo $f ; done | \
        columns --spread=1 -I10 --first='for f in ' --line=" \\" >&5

    cat <<- '_EOF_' >&5
	do
	  d=`dirname $f`
	  if test -d $d
	  then rm -f $f
	  else mkdir -p $d
	  fi
	done

	_EOF_

    start_mark='\(DO NOT EDIT THIS FILE\|EDIT THIS FILE WITH CAUTION\)'
    end_mark='\(and the template file\|It has been extracted by getdefs\)'

    # Strip out variable parts of generated files.
    #
    for f in ${GENLIST}
    do
        sed -i -e "/${start_mark}/,/${end_mark}/d" \
            -e "s@${tmpag}@\${tmpdir}@g" \
            -e "/Character mapping generated/d" \
            -e "/Generated header for gperf generated source/d" \
            -e "/extracted from .* near line/d" ${f} || continue
    done

    sed -i "/\/opt-text-/d" agen5/stamp-texi

    for f in ${PROTOLIST}
    do
        sed -i '/^ \* Generated/d' ${f}
    done

    touch -t ${timestamp} ${GENLIST}

    readonly preamble=\
'(uudecode -o /dev/stdout | gunzip -c > %s\n) <''< \_EOArchive_\n'

    for f in ${GENLIST} ${PROTOLIST}
    do
        test -s $f || {
            echo touch $f
            continue
        }

        printf "${preamble}" $f
        gzip --best -c $f | uuencode -m /dev/stdout
        echo _EOArchive_
        echo
    done >&5

    exec 5>&-

    chmod +x ${cfgfile}
    mv -f ${cfgfile} ${progdir}/noag-boot.sh
}

init
copy_files
run_bootstrap
get_gen_list
mk_noag_boot

trap "echo rm -rf ${tmpag}" EXIT
exit  0
# mkconfig.sh ends here
