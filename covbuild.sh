#!/usr/bin/env bash

getversion() {
	paste -d. <(grep -m1 MAJOR CMakeLists.txt|cut -f2 -d' '|cut -f1 -d\)) \
		  <(grep -m1 MINOR CMakeLists.txt|cut -f2 -d' '|cut -f1 -d\))
}

# Settings

PROJECT_NAME=lifish
EMAIL=silverweed1991@gmail.com
[[ `uname` == "FreeBSD" ]] \
	&& COVERITY_PATH=$HOME/cov-analysis-freebsd64-8.5.0.1/bin \
	|| COVERITY_PATH=$HOME/Public/cov-analysis-linux64-2019.03/bin
VERSION=$(getversion)

###########

while [[ $# > 0 ]]; do
	case $1 in
	-s|--skip-build)
		SKIP_BUILD=1
		shift
		;;
	-c|--skip-clean)
		SKIP_CLEAN=1
		shift
		;;
	*)
		echo "Usage: $0 [-s, --skip-build] [-c, --skip-clean]" >&2
		exit 1
		;;
	esac
done

getbuild() {
	git describe --always --long --dirty
}

getreadiness() {
	tail cov-int/build-log.txt | grep compilation | cut -f2 -d\( | cut -f1 -d% | tail -1
}

PATH=$PATH:$COVERITY_PATH
if [[ $SKIP_BUILD != 1 ]]; then
	[[ $SKIP_CLEAN != 1 ]] && make clean
	rm -f ${PROJECT_NAME}.tgz
	if ($COVERITY_PATH/cov-build --dir cov-int make -j 4); then
		tar cvfz ${PROJECT_NAME}.tgz cov-int
	fi
fi

if [[ $? == 0 ]]; then
	R=$(getreadiness)
	if ((R < 85)); then
		echo "[ ERROR ] only ${R}% compilation units are ready: build will fail."
		exit 2
	else
		echo "[ OK ] ${R}% of the compilation units are ready."
	fi
	echo "Submit new build? ($(du -sh ${PROJECT_NAME}.tgz | cut -f1))"
	select ANS in "Submit" "Abort"; do
		case $ANS in
			Submit)
				echo Submitting... >&2
				set -x
				curl	--form token=$(< ./cov-token) \
					--form email=$EMAIL \
					--form file=@./${PROJECT_NAME}.tgz \
					--form version="${VERSION}" \
					--form description="${PROJECT_NAME} build rev.$(getbuild)" \
					https://scan.coverity.com/builds?project=${PROJECT_NAME}
				set +x
				echo Done. >&2
				exit 0
				;;
			*)
				echo Aborted. >&2
				exit 1
				;;
		esac
	done
fi
