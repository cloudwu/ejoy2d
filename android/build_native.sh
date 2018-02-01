DIR=$(cd "$( dirname "$0")"; pwd)
APP_ROOT="$DIR"/..
if [ -f "$DIR"/assets/game.zip ]; then
    rm -rf "$DIR"/assets/game.zip
fi
if [ -d "$DIR"/assets/examples ]; then
    rm -rf "$DIR"/assets/examples
fi
if [ -d "$DIR"/assets/ejoy2d ]; then
    rm -rf "$DIR"/assets/ejoy2d
fi
mkdir -p "$DIR"/assets
chmod 755 "$DIR"/assets
cp -rf "$APP_ROOT"/examples "$DIR"/assets/
cp -rf "$APP_ROOT"/ejoy2d "$DIR"/assets/

cd "$DIR"/assets
zip -r game.zip . && rm -rf examples ejoy2d
cd -

if [ -d "$DIR"/bin ]; then
    rm -rf "$DIR"/bin
fi

ndk-build -C "." $* "NDK_MODULE_PATH=jni/ndk_modules"
ant debug -q
#adb uninstall com.example.ejoy2d
#adb install bin/ejoy2d-debug.apk
#adb logcat
