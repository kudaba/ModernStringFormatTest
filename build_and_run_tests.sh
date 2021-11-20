#!/bin/bash

config=${1}
platform=${2}
toolset=${3}
extra=${4}

if [ "$config" = "" ]; then
    config=debug
fi

if [ "$platform" = "" ]; then
    platform=linux
fi

if [ "$toolset" = "" ]; then
    toolset=clang
fi

pushd projects
make -f modernstringformatsolution_${platform}_${toolset}.make config=${config} ${extra}
exitcode=$?
popd

if [ ${exitcode} = 0 ]; then
    if [ "$extra" != "clean" ]; then
		# try yo handle both lower case and camel case files. This can change depending on 
		# whether projects were generated on windows or linux (todo: fix that)
		# upper case the first letter of Release of Debug to match output paths

		exe=./.temp/bin/modernstringformatproject_${platform}_${toolset}_${config}
		if [ ! -f "$exe" ]; then
			config="$(tr '[:lower:]' '[:upper:]' <<< ${config:0:1})${config:1}"
			exe=./.temp/output/modernstringformatproject_${platform}_${toolset}_${config}
		fi

		$exe
		exit1=$?

		if [ ${exit1} != 0 ]; then
			exitcode=${exit1}
			echo modernstringformat_ failed with ${exit1}
		fi
    fi
fi

exit ${exitcode}
