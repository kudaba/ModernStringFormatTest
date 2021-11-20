#!/bin/bash
./bin/Sharpmake/linux-x64/Sharpmake.Application "/sources(@'sharpmake/main.cs') /verbose /generateDebugSolution $@"
errorlevel=$?

if [ ! $errorlevel -eq 0 ]; then
	echo -e "\e[31mProject Generation failed\e[0m"
  	read -p "Press [Enter] key to exit..."
    errorlevel=1
fi

exit ${errorlevel}
