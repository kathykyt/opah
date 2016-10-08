cd $(cd `dirname $0`; pwd)
rm -rf out
rm AllBmpArrayCheck.h BmpDataArray.h
mkdir -p out/BmpDataArray
cp pic out -rf
sync
cp ../../tools/bmptodata out/
cp ../../tools/bmptoc out/
cd out
#find pic -iname "*.png" -type f -exec convert.im6 {} `basename {} .png`.bmp \;
find pic -iname "*.png" -type f -exec convert {} `basename {} .png`.bmp \;
find pic -iname "*.bmp" -type f -exec ./bmptodata {} 0 \;
./bmptoc pic 0

find pic -iname "*.png" -type f -exec rm {} \;
find pic -iname "*.bmp" -type f -exec rm {} \;
cp BmpDataArray/BmpDataArray.h ../
cp BmpDataArray/AllBmpArrayCheck.h ../
cd ../../../
sync
