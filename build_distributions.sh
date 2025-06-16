./game-app/run/.build/set-game-headed-build-number
# Build with java 15 for compatability reasons
gradle -Dorg.gradle.java.home=/Library/Java/JavaVirtualMachines/zulu-15.jdk/Contents/Home :game-app:game-headed:shadowJar

buildVersion=$(git rev-list --count HEAD)2

gradle downloadAssets

rm -rf ./Windows
cp -r ./buildFiles/windows/. ./Windows/
mkdir ./Windows/bin
unzip ./game-app/game-headed/.assets/assets.zip -d ./Windows/assets
cp ./game-app/game-headed/build/libs/*2.7+${buildVersion}.jar ./Windows/bin

rm -rf ./Releases
mkdir -p ./Releases/Windows
# zip the package together and exclude macos files
cd ./Windows;zip -r ../Releases/Windows/TripleA\ Chris\ Modified.zip . -x "__MACOSX" -x "*DS_Store" ; cd ..

rm -rf ./MacOS
mkdir -p ./MacOS/TripleA\ Chris\ Modified.app/Contents/MacOS
cp -r ./buildFiles/macos/. ./MacOS/TripleA\ Chris\ Modified.app/Contents
mkdir ./MacOS/TripleA\ Chris\ Modified.app/Contents/Resources/app/bin

# Create MacOS Launcher file
cat << 'EOF' > ./MacOS/TripleA\ Chris\ Modified.app/Contents/MacOS/launcher
#!/bin/bash
DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$DIR/../Resources/app"
ICON="$DIR/../Resources/app.icns"
EOF
cat << EOF >> ./MacOS/TripleA\ Chris\ Modified.app/Contents/MacOS/launcher
"\$DIR/../Resources/jre.bundle/Contents/Home/bin/java" -Xmx4G --add-opens java.desktop/com.apple.eawt.event=ALL-UNNAMED -Xdock:name="TripleA Chris Modified" -Xdock:icon="$ICON" -jar "./bin/triplea-game-headed-2.7+${buildVersion}.jar" "$@"
EOF
chmod 755 ./MacOS/TripleA\ Chris\ Modified.app/Contents/MacOS/launcher

unzip ./game-app/game-headed/.assets/assets.zip -d ./MacOS/TripleA\ Chris\ Modified.app/Contents/Resources/app/assets
cp ./game-app/game-headed/build/libs/*2.7+${buildVersion}.jar ./MacOS/TripleA\ Chris\ Modified.app/Contents/Resources/app/bin/

mkdir -p ./Releases/MacOS
cd ./MacOS;zip -r ../Releases/MacOS/TripleA\ Chris\ Modified.zip . -x "__MACOSX" -x "*DS_Store" ; cd ..
