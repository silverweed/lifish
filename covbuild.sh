#!/usr/bin/env bash

# Settings

PROJECT_NAME=lifish
EMAIL=silverweed1991@gmail.com
COVERITY_PATH=$HOME/Public/cov-analysis-linux64-7.6.0/bin

###########

getbuild() {
	git describe --always --long --dirty
}

getreadiness() {
	tail cov-int/build-log.txt | grep compilation | cut -f2 -d\( | cut -f1 -d%
}

PATH=$PATH:$COVERITY_PATH
make clean
rm -f ${PROJECT_NAME}.tgz
if (cov-build --dir cov-int make -j); then
	tar cvfz ${PROJECT_NAME}.tgz cov-int
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
					--form version="0.0" \
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
