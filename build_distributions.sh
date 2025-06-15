./game-app/run/.build/set-game-headed-build-number

# Build with java 15 for compatability reasons
gradle -Dorg.gradle.java.home=/Library/Java/JavaVirtualMachines/zulu-15.jdk/Contents/Home :game-app:game-headed:shadowJar
gradle downloadAssets

rm -rf ./Windows
cp -r ./buildFiles/windows/. ./Windows/
mkdir ./Windows/bin
unzip ./game-app/game-headed/.assets/assets.zip -d ./Windows/assets
cp ./game-app/game-headed/build/libs/*2.7*.jar ./Windows/bin

rm -rf ./Releases
mkdir -p ./Releases/Windows
# zip the package together and exclude macos files
cd ./Windows;zip -r ../Releases/Windows/TripleA\ Chris\ Modified.zip . -x "__MACOSX" -x "*DS_Store" ; cd ..

rm -rf ./MacOS
mkdir -p ./MacOS/TripleA\ Chris\ Modified.app/Contents
cp -r ./buildFiles/macos/. ./MacOS/TripleA\ Chris\ Modified.app/Contents
mkdir ./MacOS/TripleA\ Chris\ Modified.app/Contents/Resources/app/bin
unzip ./game-app/game-headed/.assets/assets.zip -d ./MacOS/TripleA\ Chris\ Modified.app/Contents/Resources/app/assets
cp ./game-app/game-headed/build/libs/*2.7*.jar ./MacOS/TripleA\ Chris\ Modified.app/Contents/Resources/app/bin/

mkdir -p ./Releases/MacOS
cd ./MacOS;zip -r ../Releases/MacOS/TripleA\ Chris\ Modified.zip . -x "__MACOSX" -x "*DS_Store" ; cd ..
