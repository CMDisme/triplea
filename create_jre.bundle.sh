

# TO GET WINDOWS jre FOLDER:
# download some modern TripleA release, copy the jre folder into ./buildFiles/windows/jre

javaHomePath=/Library/Java/JavaVirtualMachines/zulu-23.jdk/Contents/Home/

jarMods=$(${javaHomePath}bin/java --list-modules  ./game-app/game-headed/build/libs/triplea-game-headed-2.7+14904.jar | sed 's/@.*//' | paste -sd, -)


mkdir -p jre.bundle/Contents
${javaHomePath}bin/jlink --module-path "${javaHomePath}jmods" --add-modules $jarMods --output jre.bundle/Contents/Home
rm -rf ./buildFiles/macos/Resources/jre.bundle
mv ./jre.bundle ./buildFiles/macos/Resources/