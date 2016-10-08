rm -rf out
rm AllBmpArrayCheck.h BmpDataArray.h
#rm BmpDataPath.c
mkdir -p out/BmpDataArray
cp pic out -rf
sync
cd ../../tools/bmp2c
make clean
make
cp bmptoc bmptodata ../../project/demo/out
cd ../../project/demo/out
find pic -iname "*.png" -type f -exec convert {} `basename {} .png`.bmp \;
find pic -iname "*.bmp" -type f -exec bmptodata {} 0 \;
bmptoc pic 0
find pic -iname "*.png" -type f -exec rm {} \;
find pic -iname "*.bmp" -type f -exec rm {} \;
cp BmpDataArray/BmpDataArray.h ../
cp BmpDataArray/AllBmpArrayCheck.h ../
#cp BmpDataPath.c ../
cd ../../../
sync
