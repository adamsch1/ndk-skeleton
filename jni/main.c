#include <stdbool.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>


#include <android/input.h>
#include <android_native_app_glue.h>

#include <android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, __FILE__, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, __FILE__, __VA_ARGS__))

static EGLDisplay display;
static EGLSurface surface;
static EGLContext context;

int foo(int x);

static void gles_init(struct android_app *android_app)
{
    foo(42);

    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(display, 0, 0);

    const int config_attribs[] = {
        EGL_RED_SIZE, 0,
        EGL_GREEN_SIZE, 0,
        EGL_BLUE_SIZE, 0,
        EGL_ALPHA_SIZE, 0,
        EGL_DEPTH_SIZE, 0,
        EGL_STENCIL_SIZE, 0,
        EGL_SAMPLES, 0,
        EGL_SAMPLE_BUFFERS, 0,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };

    LOGI("EGL_VENDOR: %s", eglQueryString(display, EGL_VENDOR));
    LOGI("EGL_VERSION: %s", eglQueryString(display, EGL_VERSION));
    LOGI("EGL_CLIENT_APIS: %s", eglQueryString(display, EGL_CLIENT_APIS));
    LOGI("EGL_EXTENSIONS: %s", eglQueryString(display, EGL_EXTENSIONS));

    EGLConfig config;
    int num_configs;
    eglChooseConfig(display, config_attribs, &config, 1, &num_configs);

    int format;
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
    ANativeWindow_setBuffersGeometry(android_app->window, 0, 0, format);

    surface = eglCreateWindowSurface(display, config, android_app->window, NULL);

    const int context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    eglBindAPI(EGL_OPENGL_ES_API);
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attribs);

    eglMakeCurrent(display, surface, surface, context);

    LOGI("GL_VERSION: %s", glGetString(GL_VERSION));
    LOGI("GL_VENDOR: %s", glGetString(GL_VENDOR));
    LOGI("GL_RENDERER: %s", glGetString(GL_RENDERER));
    LOGI("GL_EXTENSIONS: %s", glGetString(GL_EXTENSIONS));

}

static void gles_quit()
{
    eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(display, context);
    eglDestroySurface(display, surface);
    eglTerminate(display);
}

static void gles_paint()
{
    glClearColor(0.2, 0.4, 0.7, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    eglSwapBuffers(display, surface);
}

static void app_cmd_callback(struct android_app *android_app, int32_t cmd)
{
    (void)android_app;

    switch(cmd)
    {
        case APP_CMD_INIT_WINDOW:
            LOGI("APP_CMD_INIT_WINDOW");
            gles_init(android_app);
            gles_paint();
            break;
        case APP_CMD_TERM_WINDOW:
            LOGI("APP_CMD_TERM_WINDOW");
            gles_quit();
            break;
        case APP_CMD_WINDOW_REDRAW_NEEDED:
            LOGI("APP_CMD_WINDOW_REDRAW_NEEDED");
            gles_paint();
            break;

        case APP_CMD_INPUT_CHANGED:
            LOGI("APP_CMD_INPUT_CHANGED");
            break;
        case APP_CMD_WINDOW_RESIZED:
            LOGI("APP_CMD_WINDOW_RESIZED");
            break;
        case APP_CMD_CONTENT_RECT_CHANGED:
            LOGI("APP_CMD_CONTENT_RECT_CHANGED");
            break;
        case APP_CMD_GAINED_FOCUS:
            LOGI("APP_CMD_GAINED_FOCUS");
            break;
        case APP_CMD_LOST_FOCUS:
            LOGI("APP_CMD_LOST_FOCUS");
            break;
        case APP_CMD_CONFIG_CHANGED:
            LOGI("APP_CMD_CONFIG_CHANGED");
            break;
        case APP_CMD_LOW_MEMORY:
            LOGI("APP_CMD_LOW_MEMORY");
            break;
        case APP_CMD_START:
            LOGI("APP_CMD_START");
            break;
        case APP_CMD_RESUME:
            LOGI("APP_CMD_RESUME");
            break;
        case APP_CMD_SAVE_STATE:
            LOGI("APP_CMD_SAVE_STATE");
            android_app->savedState = NULL;
            android_app->savedStateSize = 0;
            break;
        case APP_CMD_PAUSE:
            LOGI("APP_CMD_PAUSE");
            break;
        case APP_CMD_STOP:
            LOGI("APP_CMD_STOP");
            break;
        case APP_CMD_DESTROY:
            LOGI("APP_CMD_DESTROY");
            break;
        default:
            break;
    }
}

static int32_t input_event_callback(struct android_app* android_app, AInputEvent* event)
{
    (void)android_app;
    (void)event;

    switch(AInputEvent_getType(event))
    {
        case AINPUT_EVENT_TYPE_KEY:
        case AINPUT_EVENT_TYPE_MOTION:
            //return 1;
        default:
            break;
    }

    return 0;
}

void android_main(struct android_app *android_app)
{
    (void)android_app;

    // Make sure glue isn't stripped.
    app_dummy();

    android_app->userData = NULL;
    android_app->onAppCmd = app_cmd_callback;
    android_app->onInputEvent = input_event_callback;

    while(!android_app->destroyRequested)
    {
        int events;
        struct android_poll_source *source;
        while(!android_app->destroyRequested &&
            ALooper_pollAll(-1, NULL, &events, (void**)&source) >= 0)
        {
            if(source)
                source->process(android_app, source);
        }
    }
}
