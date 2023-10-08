#!/bin/bash

CMAKE_BIN=
WORKING_DIR=$PWD
GLFW_FOLDER="deps/glfw"
SHOULD_CLEAN=
SHOULD_BUILD_DEPS=
GLFW_LIB_PATH="$WORKING_DIR/deps/glfw/build/src"
LIBS_FOLDER=$WORKING_DIR/libs
SRC_FOLDER=$WORKING_DIR/src
INCLUDE_FOLDER=$WORKING_DIR/include
COMPILER=
PLATFORM=$(uname -a)
ARCH=$(uname -m)
INCLUDE_CMD="-I""$INCLUDE_FOLDER"
LIB_CMD="-L""$LIBS_FOLDER"
COMPILER_FLAGS="-Wno-write-strings"
BIN_NAME=blight

BEGIN=$(date +%s)
while getopts 'C:chd' opt; do
  case "$opt" in
     d)
	    SHOULD_BUILD_DEPS="yes"
	    ;;
     c)
	    SHOULD_CLEAN="yes"
	   ;;
     C)
	     COMPILER="$OPTARG"
	     echo "Gonna use "$(basename $COMPILER) " in path "$(dirname $COMPILER). 
	     ;;
    h)
      echo "Usage: $(basename $0) "
      echo "-d				Also compile dependencies"
      echo "-c				Remove build folder before compiling"
      echo "-C  <compiler-fullpath>	Uses the compiler provided by the fullpath"
      exit 1
      ;;
  esac
done

searchBinary() {
TARGET="$1"

	if [ -z "$TARGET" ]; then return -1; fi
	echo $(which $TARGET | cut -d " " -f 2)
	return 0
}

echo Working directory: $WORKING_DIR

if [ -z "$COMPILER" ]; then
	echo -n "Searching for gcc..."
	COMPILER=$(searchBinary "gcc")
	if [ "$?" != 0 ]; then
		echo "Not found."
		echo -n "Searching for clang..."
		COMPILER=$(searchBinary "clang")
		if [ "$?" != 0 ]; then
			echo "Not found."
			echo -n "Searching for mingw..."
			COMPILER=$(searchBinary "mingw")
			if [ "$?" != 0]; then
				echo "Not found."
				echo -n "Searching for Visual Studio compiler..."
				PATH=
				for partition in $(dl -l | cut -d " " -f 1); do 
					PATH=$(find $partition -name cl.exe)
					if [ ! -z $PATH ]; then break; fi
				done
				echo "Not implemented found $PATH"
				exit
			else 
				echo "mingw found."
			fi
		else 
			echo "clang found."	
		fi
	else 
		echo "gcc found."
	fi	
fi

if [[ ! "$WORKING_DIR" == *blight* ]]; then
	echo "[ERROR]: Wrong folder... Leaving"
	exit
fi

if [ ! -z "$SHOULD_BUILD_DEPS" ]; then
	if [ -z "$CMAKE_BIN" ]; then
		echo -n "Searching for cmake..."
		CMAKE_BIN=$(which cmake | cut -d " " -f 2)
	fi

	if [ -z "$CMAKE_BIN" ]; then
		echo "cmake not found"
		echo "[ERROR]: Could not found cmake"
		exit
	else 
		echo "cmake found"
	fi

	echo "[INFO]: Building glfw..."
	sleep 1
	mkdir -p $WORKING_DIR/$GLFW_FOLDER/build
	pushd $WORKING_DIR/$GLFW_FOLDER/build > /dev/null 2>&1
	$CMAKE_BIN -DCMAKE_BUILD_TYPE=Release -S .. -B . 
	$CMAKE_BIN --build .
	cp $WORKING_DIR/$GLFW_FOLDER/build/src/libglfw3.* $LIBS_FOLDER
	cp -r $WORKING_DIR/$GLFW_FOLDER/include/GLFW $INCLUDE_FOLDER
	echo "[INFO]: Done..."
	popd > /dev/null 2>&1 
fi


if [ ! -z "$SHOULD_CLEAN" ]; then
	echo "[INFO]: Removing build folder"
	rm -rf $WORKING_DIR/build
fi

echo "[INFO]: Compiling blight" 
sleep 1

mkdir -p $WORKING_DIR/build
pushd $WORKING_DIR/build > /dev/null 2>&1
for file in $(find ../src -name *.c -o -name *.cpp); do
	$COMPILER $COMPILER_FLAGS $file -o $(basename ${file%.*}).o -c "$INCLUDE_CMD" "$LIB_CMD"
done

OBJ_LIST=$(find $WORKING_DIR/build -name "*.o" -printf "%p ")
$COMPILER $COMPILER_FLAGS $OBJ_LIST -o $BIN_NAME $INCLUDE_CMD $LIB_CMD -lglfw3 -lm -lGL
popd > /dev/null 2>&1

RUNTIME=$(( $(date +%s) - $BEGIN ))
RUNTIME_MSG=

if [ "$RUNTIME" -gt "1" ]; then
	RUNTIME_MSG="$RUNTIME seconds"
else
	RUNTIME_MSG="$RUNTIME second"
fi

echo "Finished execution in: $RUNTIME_MSG"