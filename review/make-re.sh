#!/bin/sh

cd `dirname $0`
cd ..

for file in markdown/*.md; do
  echo $file
  ext=${file##*.}
  #echo $ext
  base=`basename $file .md`
  #echo $base
  md2review "$file" > "review/$base.re"
done

