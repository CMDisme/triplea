./game-app/run/.build/set-game-headed-build-number
#Set the java home to 15
./gradlew -Dorg.gradle.java.home=/Library/Java/JavaVirtualMachines/zulu-15.jdk/Contents/Home :game-app:game-headed:shadowJar
