#!/bin/bash
#
# sudo apt-get install php php-gd
#
# get a file list with "px.png"
#
find | grep "px.png" > png-list.txt
#
# read file line by line an convert files
# to c files with online image converter
# from LVGL.io
#
while read LINE
do
	CNAME=`echo $LINE | cut -d"." -f2`
	FILE=${CNAME##*/}
	RESOLUTION=`echo $FILE | cut -d"_" -f2 | cut -d"p" -f1`
	echo "$LINE -> $CNAME.c ($FILE) RESOLUTIOIN=$RESOLUTION"
	php ../../../lv_utils/img_conv_core.php "name=$FILE&img=$LINE&cf=raw_alpha&format=c_array"
	echo "cp $FILE.c $CNAME.c"
#	sed -i "s/.h = ,/.h = $RESOLUTION,/g" .$CNAME.c
#	sed -i "s/.w = ,/.w = 240,/g" .$CNAME.c
done < png-list.txt
#
# get a list of png file with *"96px.png"
#
find | grep px.c > pxc-list.txt

while read LINE
do
	FILE=$(basename $LINE)
	echo "$LINE -> $FILE"
	cp $LINE ../../../../src/gui/images/
done < pxc-list.txt
#
# delete all .c and low res files
#
# rm *.txt
# find . -name '*.c' -exec rm -rf {} \;
