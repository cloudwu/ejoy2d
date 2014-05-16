start Android project
====

For Linux ,

* Install android-sdk
* Install android-ndk
* Install ant
* Install glew 1.9
* Install freetype 2

this project use ant build tools to generate apk file.

#### 1. update your android project

```
    <sdk>/tools/android update project -p <project> -t <target>
```

#### 2. build it

```
    $ cd ejoy2d/android
    $ sh build_native.sh
```

it will generate apk "ejoy2d/android/bin/ejoy2d-debug.apk".

install the apk in phone or simulator to show examples.


