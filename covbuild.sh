#!/bin/bash

PATH=$PATH:$HOME/Public/cov-analysis-linux64-7.6.0/bin 
make clean
rm -f lifish.tgz
if (cov-build --dir cov-int make -j); then
	tar cvfz lifish.tgz cov-int
fi

if [[ $? == 0 ]]; then
	echo "Submit new build?"
	select ANS in "Submit" "Abort"; do
		case $ANS in
			Submit)
				echo Submitting...
				set -x
				curl	--form token=$(< ./cov-token) \
					--form email=silverweed1991@gmail.com \
					--form file=@./lifish.tgz \
					--form version="0.0" \
					--form description="Lifish build" \
					https://scan.coverity.com/builds?project=lifish
				set +x
				echo Done.
				exit 0
				;;
			*)
				echo Aborted.
				;;
		esac
	done
fi
