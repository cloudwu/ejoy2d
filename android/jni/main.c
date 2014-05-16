#include "winfw.h"
#include "platform_print.h"
#include <miniunz.h>

#include <jni.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <android_native_app_glue.h>

#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>

#define MAXFILENAME (1024)
#define UPDATE_INTERVAL 1       /* 10ms */

void font_init(const char* fontname);

/**
 * Our saved state data.
 */
struct saved_state {
    float angle;
    int32_t x;
    int32_t y;
};

/**
 * Shared state for our app.
 */
struct engine {
    struct android_app* app;

    int animating;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    int32_t width;
    int32_t height;
    struct saved_state state;
    const char* writeablePath;
};

/**
 * Initialize an EGL context for the current display.
 */
static int engine_init_display(struct engine* engine) {
    /* Here specify the attributes of the desired configuration.
     * Below, we select an EGLConfig with at least 8 bits per color
     * component compatible with on-screen windows
     */
    const EGLint attribs[] = {
        EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_BLUE_SIZE,       8,
        EGL_GREEN_SIZE,      8,
        EGL_RED_SIZE,        8,
        EGL_NONE
    };
    EGLint w,h,format;
    EGLint numConfigs;
    EGLConfig config;
    EGLSurface surface;
    EGLContext context;

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(display, 0, 0);

    /* Here, the application chooses the configuration it desires. In this
     * sample, we have a very simplified selection process, where we pick
     * the first EGLConfig that matches our criteria */
    eglChooseConfig(display, attribs, &config, 1, &numConfigs);

    /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
     * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
     * As soon as we picked a EGLConfig, we can safely reconfigure the
     * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

    ANativeWindow_setBuffersGeometry(engine->app->window, 0, 0, format);
    surface = eglCreateWindowSurface(display, config, engine->app->window, NULL);

    /* for opengles 2.0 */
    const EGLint ContextAttribList[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, ContextAttribList);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        pf_log("Unable to eglMakeCurrent");
        return -1;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);
    pf_log("device:width=%d,height=%d",w,h);

    engine->animating = 1;
    engine->display = display;
    engine->context = context;
    engine->surface = surface;
    engine->width = w;
    engine->height = h;
    engine->state.angle = 0;

    char fontname[MAXFILENAME+16] = "";
    snprintf(fontname,MAXFILENAME,"%s/examples/asset/kaiti_GB2312.ttf",
        engine->writeablePath);
    font_init(fontname);
    ejoy2d_win_init(engine->width, engine->height, engine->writeablePath);
    return 0;
}

/**
 * Just the current frame in the display.
 */
static void engine_draw_frame(struct engine* engine) {
    if (engine->display == NULL) {
        pf_log("engine_draw_frame : No display.");
        return;
    }

    ejoy2d_win_frame();
    eglSwapBuffers(engine->display, engine->surface);
}

/**
 * Tear down the EGL context currently associated with the display.
 */
static void engine_term_display(struct engine* engine) {
    if (engine->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (engine->context != EGL_NO_CONTEXT) {
            eglDestroyContext(engine->display, engine->context);
        }
        if (engine->surface != EGL_NO_SURFACE) {
            eglDestroySurface(engine->display, engine->surface);
        }
        eglTerminate(engine->display);
    }
    engine->animating = 0;
    engine->display = EGL_NO_DISPLAY;
    engine->context = EGL_NO_CONTEXT;
    engine->surface = EGL_NO_SURFACE;
}

/**
 * Process the next input event.
 */
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {
    struct engine* engine = (struct engine*)app->userData;
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        int32_t x = AMotionEvent_getX(event, 0);
        int32_t y = AMotionEvent_getY(event, 0);
        switch (AMotionEvent_getAction(event)) {
            case AMOTION_EVENT_ACTION_MOVE:
                ejoy2d_win_touch(x, y, TOUCH_MOVE);
                break;
            case AMOTION_EVENT_ACTION_DOWN:
                ejoy2d_win_touch(x, y, TOUCH_BEGIN);
                break;
            case AMOTION_EVENT_ACTION_UP:
                ejoy2d_win_touch(x, y, TOUCH_END);
                break;
        }
        engine->state.x = x;
        engine->state.y = y;
        return 1;
    }
    return 0;
}

/**
 * Process the next main command.
 */
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
    struct engine* engine = (struct engine*)app->userData;
    switch (cmd) {
        case APP_CMD_SAVE_STATE:
            // The system has asked us to save our current state.  Do so.
            engine->app->savedState = malloc(sizeof(struct saved_state));
            *((struct saved_state*)engine->app->savedState) = engine->state;
            engine->app->savedStateSize = sizeof(struct saved_state);
            break;
        case APP_CMD_INIT_WINDOW:
            // The window is being shown, get it ready.
            if (engine->app->window != NULL) {
                engine_init_display(engine);
                engine_draw_frame(engine);
            }
            break;
        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
            engine_term_display(engine);
            break;
        case APP_CMD_GAINED_FOCUS:
            // When our app gains focus, we start monitoring the accelerometer.
            engine->animating = 1;
            ejoy2d_win_resume();
            break;
        case APP_CMD_LOST_FOCUS:
            // Also stop animating.
            engine->animating = 0;
            engine_draw_frame(engine);
            break;
        case APP_CMD_CONFIG_CHANGED:
            if (engine->app->window != NULL
                && ((engine->width != ANativeWindow_getWidth(app->window))
                    || (engine->height != ANativeWindow_getHeight(app->window)))) {
                engine_handle_cmd(app, APP_CMD_TERM_WINDOW);
                engine_handle_cmd(app, APP_CMD_INIT_WINDOW);
            }
            break;
    }
}

int copyAssetFile(AAssetManager *mgr, const char* writeablePath) {
    struct stat sb;
    int32_t res = stat(writeablePath, &sb);
    if (0 == res && sb.st_mode & S_IFDIR) {
        pf_log("'files/' dir already in app's internal data storage.");
    }
    else if (ENOENT == errno) {
        res = mkdir(writeablePath, 0770);
    }
    if (0 == res) {
        const char *fname = "game.zip";
        char tofname[MAXFILENAME+16] = "";
        FILE *file = NULL;

        snprintf(tofname,MAXFILENAME,"%s/%s",writeablePath,fname);
        file = fopen(tofname, "rb");
        if (file) {
            fclose(file);
            pf_log("file already exist : %s",tofname);
            return 0;
        }

        // copy file is not exist
        AAsset *asset = AAssetManager_open(mgr, fname, AASSET_MODE_UNKNOWN);
        if (asset==NULL) {
            pf_log("no file : assets/%s",fname);
            return 1;
        }

        off_t fz = AAsset_getLength(asset);
        const void* buf = AAsset_getBuffer(asset);
        file = fopen(tofname, "wb");
        if (file) {
            fwrite(buf, sizeof(char), fz, file);
            fclose(file);
        }
        else {
            pf_log("open file error : %s",tofname);
        }
        AAsset_close(asset);
        res = unzip(tofname, writeablePath);
    }
    return res;
}

const char* getWriteablePath(ANativeActivity *activity) {
    if (activity->internalDataPath) {
        pf_log("writeablePath : %s",activity->internalDataPath);
        return activity->internalDataPath;
    }
    // android 2.3 must use jni.
    JavaVM* vm = activity->vm;
    JNIEnv* env = activity->env;
    jint res = (*vm)->AttachCurrentThread(vm,&env, 0);
    if (res!=0) {
        pf_log("getWriteablePath error.");
    }
    jclass clazz = (*env)->GetObjectClass(env,activity->clazz);
    jmethodID mid_getFilesDir = (*env)->GetMethodID(env,clazz, "getFilesDir", "()Ljava/io/File;");
    jobject obj_File = (*env)->CallObjectMethod(env,activity->clazz, mid_getFilesDir);
    jclass cls_File = (*env)->GetObjectClass(env,obj_File);
    jmethodID mid_getAbsolutePath = (*env)->GetMethodID(env,cls_File, "getAbsolutePath", "()Ljava/lang/String;");
    jstring path = (jstring)(*env)->CallObjectMethod(env,obj_File, mid_getAbsolutePath);
    const char* writeablePath = (*env)->GetStringUTFChars(env,path, NULL);
    pf_log("writeablePath by jni: %s",writeablePath);
    (*vm)->DetachCurrentThread(vm);
    return writeablePath;
}

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(struct android_app* state) {
    struct engine engine;
    uint32_t timestamp = 0;

    // Make sure glue isn't stripped.
    app_dummy();

    memset(&engine, 0, sizeof(engine));
    engine.writeablePath = getWriteablePath(state->activity);
    state->userData = &engine;
    state->onAppCmd = engine_handle_cmd;
    state->onInputEvent = engine_handle_input;
    engine.app = state;

    // init res : copy assets file to writeable path
    copyAssetFile(state->activity->assetManager,engine.writeablePath);

    if (state->savedState != NULL) {
        // We are starting with a previous saved state; restore from it.
        engine.state = *(struct saved_state*)state->savedState;
    }

    // loop waiting for stuff to do.
    while (1) {
        // Read all pending events.
        int ident;
        int events;
        struct android_poll_source* source;

        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while ((ident=ALooper_pollAll(engine.animating ? 0 : -1, NULL, &events,
                (void**)&source)) >= 0) {

            // Process this event.
            if (source != NULL) {
                source->process(state, source);
            }

            // Check if we are exiting.
            if (state->destroyRequested != 0) {
                engine_term_display(&engine);
                return;
            }
        }

        if (engine.animating) {
            // Done with events; draw next animation frame.
            engine.state.angle += 0.1f;
            if (engine.state.angle >= UPDATE_INTERVAL) {
                engine.state.angle = 0;
                ejoy2d_win_update();
                engine_draw_frame(&engine);
            }
            else {
                usleep(1000);
            }
        }
    }
}

