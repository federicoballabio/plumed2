#! /usr/bin/env bash

if [ "$1" = --description ] ; then
  echo "compile a .cpp file into a shared library"
  exit 0
fi

if [ "$1" = --options ] ; then
  echo "--description --options"
  exit 0
fi

source "$PLUMED_ROOT"/src/config/compile_options.sh

if [ $# != 1 ] || [[ "$1" != *.cpp ]] ;
then
  echo "ERROR"
  echo "type 'plumed mklib file.cpp'"
  exit 1
fi


file="$1"
obj="${file%%.cpp}".o
lib="${file%%.cpp}".$soext

if [ ! -f "$file" ]
then
  echo "ERROR: I cannot find file $file"
  exit 1
fi

if grep -q '^#include "\(bias\|colvar\|function\|sasa\|vatom\)\/ActionRegister.h"' "$file"; then
   >&2 echo 'WARNING: using a legacy ActionRegister.h include path, please use <<#include "core/ActionRegister.h">>'
   sed 's%^#include ".*/ActionRegister.h"%#include "core/ActionRegister.h"%g' < "$file" > "tmp_${file}"
   file="tmp_${file}"
fi

if grep -q '^#include "\(cltools\)\/CLToolRegister.h"' "$file"; then
   >&2  echo 'WARNING: using a legacy  CLToolRegister.h include path, please use <<#include "core/CLToolRegister.h">>'
   sed 's%^#include ".*/CLToolRegister.h"%#include "core/CLToolRegister.h"%g' < "$file" > "tmp_${file}"
   file="tmp_${file}"
fi

rm -f "$obj" "$lib"

echo "$compile" "$obj" "$file"
echo "$link_installed" "$lib" "$obj"
if test "$PLUMED_IS_INSTALLED" = yes ; then
  eval "$compile" "$obj" "$file" && eval "$link_installed" "$lib" "$obj"
else
  eval "$compile" "$obj" "$file" && eval "$link_uninstalled" "$lib" "$obj"
fi



