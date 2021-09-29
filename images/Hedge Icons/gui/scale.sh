#!/bin/bash
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
#	curl POST -F img_file=@$LINE -F name=$FILE -F cf=true_color_alpha -F format=c_array -F submit=Convert https://lvgl.io/tools/img_conv_core.php > .$CNAME.c
	curl POST -F img_file=@$LINE -F name=$FILE -F cf=raw_alpha -F format=c_array -F submit=Convert https://lvgl.io/tools/img_conv_core.php > .$CNAME.c
	sed -i "s/.h = ,/.h = $RESOLUTION,/g" .$CNAME.c
	sed -i "s/.w = ,/.w = $RESOLUTION,/g" .$CNAME.c
done < png-list.txt
#
# get a list of png file with *"px.c"
#
find | grep px.c > pxc-list.txt

while read LINE
do
	CNAME=`echo $LINE | cut -d"." -f2`
	FILE=$(basename $LINE)
	echo "cp $LINE ../../../src/gui$CNAME.c"
	cp $LINE ../../../src/gui$CNAME.c
done < pxc-list.txt
#
# delete all .c and low res files
#
rm *.txt
find . -name '*.c' -exec rm -rf {} \;

