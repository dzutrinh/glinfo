#!/usr/bin/env bash
if [ "$(uname)" == "Darwin" ]; then
	echo "Software MESA is not support on OSX"
elif [ "$(uname -s)" == "Linux" ];then
	LIBGL_ALWAYS_SOFTWARE=1 bin/glinfo -c
fi
