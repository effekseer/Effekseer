export PKG_CONFIG_PATH=/Library/Frameworks/Mono.framework/Versions/Current/lib/pkgconfig
export AS="as -arch i386"
export CC="clang -arch i386 -mmacosx-version-min=10.6"

mkdir Mac/Effekseer.app/Contents/Resources/

(cd release;
mkbundle -o Effekseer EffekseerMP.exe --deps --sdk /Library/Frameworks/Mono.framework/Versions/Current;
otool -L Effekseer;)

cp release/Effekseer Mac/Effekseer.app/Contents/Resources/
cp release/libViewer.dylib Mac/Effekseer.app/Contents/Resources/
cp -r release/resources Mac/Effekseer.app/Contents/Resources/
cp -r release/scripts Mac/Effekseer.app/Contents/Resources/

chmod +x Mac/Effekseer.app/Contents/MacOS/script.sh
