#include <android_native_app_glue.h>

#include <errno.h>
#include <jni.h>
#include <sys/time.h>
#include <time.h>
#include <android/log.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <dlfcn.h>
#include <fcntl.h>

#include <pthread.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>


#define  LOG_TAG    "libFvJ2ME"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#define SKIN_BIN_PATH "/data/data/com.fonsview.FvJ2ME/lib/libskin.so"
#define SKIN_BIN_PATH_BK "/system/lib/libskin.so"

#define JVM_DEFAULT_PATH "/data/jvm"
#define JVM_PATH_BK "/mnt"

char *fileInstallArgv1[] = {"/data/jvm/fileInstaller","/data/jvm/suite.jar"};
char *fileInstallArgv2[] = {"/mnt/fileInstaller","/mnt/suite.jar"};
char *runNamsArgv1[] = {"/data/jvm/runNams","-runMidlet","1"};
char *runNamsArgv2[] = {"/mnt/runNams","-runMidlet","1"};
char **argv1;
char **argv2;
char * image_buffer = NULL;

pthread_mutex_t *lock = NULL;
struct android_app * my_state;
/**
input format: RGBA
input size: 640x528
*/
#define IN_WIDTH    640 
#define IN_HEIGHT   528
#define IN_BPP      4

struct mengine {
	struct android_app *app;

	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
	int32_t width;
	int32_t height;

	GLenum tex;
	GLenum tex_u;

	GLuint program;
	GLuint loc_pos;
	GLuint loc_tex_coord;
	GLuint loc_tex; 

	unsigned char *in_buf;
	int in_capacity;

	int init;
};

static void engine_init_draw_frame(struct mengine *engine);

static const char g_vert_src[] = " \
	attribute vec4 vPosition; \
	attribute vec2 a_texCoord; \
	varying vec2 v_texCoord; \
	void main() \
	{ \
		v_texCoord = a_texCoord; \
		gl_Position = vPosition; \
	} \
";

#if 0 
static const char g_frag_src[] = " \
	precision mediump float; \
	uniform sampler2D u_texture; \
	varying vec2 v_texCoord; \
	\
	void main() \
	{ \
		vec4 abgr = texture2D(u_texture, v_texCoord); \
		gl_FragColor = abgr; \
		gl_FragColor.a = 1.0; \
	} \
";
#else
static const char g_frag_src[] = " \
	precision mediump float; \
	uniform sampler2D u_texture; \
	varying vec2 v_texCoord; \
	\
	void main() \
	{ \
		vec4 abgr = texture2D(u_texture, v_texCoord); \
		gl_FragColor.r = abgr.b; \
		gl_FragColor.b = abgr.r; \
		gl_FragColor.g = abgr.g; \
		gl_FragColor.a = 1.0; \
	} \
";
#endif


static GLuint
load_shader(GLenum type, const char *src)
{
	GLuint shader = glCreateShader(type);
	if (shader) {
		glShaderSource(shader, 1, &src, NULL);
		glCompileShader(shader);
		GLint compiled = 0; glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled); if (!compiled) { GLint len = 0; glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
			if (len) {
				char *buf = (char *) malloc(len);
				if (buf) {
					glGetShaderInfoLog(shader, len,
							   NULL, buf);
					LOGE("Could not compile shader %d:\n",
					     type);
					free(buf);
				}
				glDeleteShader(shader);
				shader = 0;
			}
		}
	}
	return shader;
}

static GLuint
create_program(const char *vert_src, const char *frag_src)
{
	GLuint vs = load_shader(GL_VERTEX_SHADER, vert_src);
	if (!vs) {
		LOGE("load vertex shader failed\n");
		return 0;
	}

	GLuint ps = load_shader(GL_FRAGMENT_SHADER, frag_src);
	if (!ps) {
		LOGE("load frag shader failed\n");
		return 0;
	}

	GLuint program = glCreateProgram();
	if (!program) {
		return 0;
	}
	glAttachShader(program, vs);
	glAttachShader(program, ps);
	glLinkProgram(program);

	GLint status = GL_FALSE;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		GLint len = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
		if (len) {
			char *buf = (char *) malloc(len);
			if (buf) {
				glGetProgramInfoLog(program, len, NULL, buf);
				LOGE("Could not link program:\n%s\n", buf);
				free(buf);
			}
		}
		glDeleteProgram(program);
		program = 0;
	}
	return program;
}


/**
 * Initialize an EGL context for the current display.
 */
static int
init_egl(struct mengine *engine)
{
	// initialize OpenGL ES and EGL

	/*
	 * Here specify the attributes of the desired configuration.
	 * Below, we select an EGLConfig with at least 8 bits per color
	 * component compatible with on-screen windows
	 */
	const EGLint attribs[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_RED_SIZE, 8,
		EGL_NONE
	};
	const EGLint context_attribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE		
	};
	EGLint w, h, dummy, format;
	EGLint num_configs;
	EGLConfig config;
	EGLSurface surface;
	EGLContext context;
	EGLint major;
	EGLint minor;

	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	if (eglInitialize(display, &major, &minor) != EGL_TRUE) {
		LOGE("eglInitialize failed\n");
	}

	/* Here, the application chooses the configuration it desires. In this
	 * sample, we have a very simplified selection process, where we pick
	 * the first EGLConfig that matches our criteria */
	eglChooseConfig(display, attribs, &config, 1, &num_configs);

	if (!num_configs) {
		LOGE("egl configs num is 0\n");
		return -1;
	}
	/* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
	 * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
	 * As soon as we picked a EGLConfig, we can safely reconfigure the
	 * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
	eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

	ANativeWindow_setBuffersGeometry(engine->app->window, 0, 0, format);

	surface = eglCreateWindowSurface(display, config,
	                                 engine->app->window,
	                                 NULL);
	context = eglCreateContext(display, config, EGL_NO_CONTEXT,
	                           context_attribs);

	if (eglMakeCurrent(display, surface, surface, context) ==
	    EGL_FALSE) {
		LOGW("Unable to eglMakeCurrent");
		return -1;
	}

	eglQuerySurface(display, surface, EGL_WIDTH, &w);
	eglQuerySurface(display, surface, EGL_HEIGHT, &h);

	engine->display = display;
	engine->context = context;
	engine->surface = surface;
	engine->width   = w;
	engine->height  = h;

	return 0;
}



static int
setup_graphics(struct mengine *engine)
{
	if (!engine->app->window) {
		return 0;
	}
#if 0
	engine->in_capacity = IN_WIDTH*IN_HEIGHT*IN_BPP;
	engine->in_buf = malloc(engine->in_capacity);
	if (!engine->in_buf) {
		return -1;
	}
	FILE *fp = fopen("/data/iptv/xxx.rgba", "rb");
	if (!fp) {
		return -1;
	}
	fread(engine->in_buf, 1, IN_WIDTH*IN_HEIGHT*4, fp);
	fclose(fp);
#endif
	if (init_egl(engine) < 0) {
		return -1;
	}

	engine->program = create_program(g_vert_src, g_frag_src);
	if (!engine->program) {
		LOGE("Could not create program\n");
		return -1;
	}
	engine->loc_pos = glGetAttribLocation(engine->program, "vPosition");
	if (engine->loc_pos < 0) {
		LOGI("vPosition get: %d\n", glGetError());
		return -1;
	}
	
	engine->loc_tex_coord = glGetAttribLocation(engine->program, 
	                                            "a_texCoord");
	if (engine->loc_tex_coord < 0) {
		LOGI("a_texCoord get: %d\n", glGetError());
		return -1;
	}

	engine->loc_tex = glGetUniformLocation(engine->program, "u_texture");
	if (engine->loc_tex < 0) {
		LOGI("u_texture get: %d\n", glGetError());
		return -1;
	}
	glViewport(0, 0, engine->width, engine->height);

	glGenTextures(1, &engine->tex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, engine->tex);
	/**GL_NEAREST or GL_LINEAR*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, IN_WIDTH, 
	             IN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	return 0;
}


static const GLfloat g_vertices[] = { 
	-1.0f,  -1.0f,
	 1.0f,  -1.0f,
	-1.0f,   1.0f,
	 1.0f,   1.0f
};

static const GLfloat g_tex_coord[] = {
	0.0f,  1.0f, 
	1.0f,  1.0f,
	0.0f,  0.0f,
	1.0f,  0.0f
};	

static void
render_frame(struct mengine *engine)
{
	if (engine->display == NULL) {
		return;
	}
	if (engine->app->window == NULL) {
		// EGL_BAD_MATCH window.
		return;
	}
		

	glUseProgram(engine->program);

	glVertexAttribPointer(engine->loc_pos, 2, GL_FLOAT,
	                      GL_FALSE, 0, g_vertices);
	glEnableVertexAttribArray(engine->loc_pos);

	glVertexAttribPointer(engine->loc_tex_coord, 2, GL_FLOAT,
	                      GL_FALSE, 0, g_tex_coord);
	glEnableVertexAttribArray(engine->loc_tex_coord);

	glBindTexture(GL_TEXTURE_2D, engine->tex);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, IN_WIDTH, IN_HEIGHT, 
	                GL_RGBA, GL_UNSIGNED_BYTE, engine->in_buf);
	glUniform1i(engine->loc_tex, 0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	eglSwapBuffers(engine->display, engine->surface);
}



static void engine_init_draw_frame(struct mengine *engine)
{
	if (!engine->init) {
		if (setup_graphics(engine) < 0) {
			exit(1);
		}
		engine->init = 1;
	}
	return;
}

static void engine_handle_cmd(struct android_app *app, int32_t cmd)
{
	struct mengine *engine = (struct mengine *) app->userData;
	switch (cmd) {
	case APP_CMD_INIT_WINDOW:
		if (engine->app->window != NULL) {
			engine_init_draw_frame(engine);
		}
		break;
	case APP_CMD_TERM_WINDOW:
		break;
	case APP_CMD_LOST_FOCUS:
//		engine->animating = 0;
		break;
	}
}
int check_dir_path()
{
	char path[32] = {0};
	char param[32] = {0};
	if(!access(JVM_DEFAULT_PATH,F_OK)){
		argv1 = runNamsArgv1;
		argv2 = fileInstallArgv1;
	}else{
		if(!access(JVM_PATH_BK,F_OK)){ 
			sprintf(path,"%s/appdb",JVM_PATH_BK);
			if(access(path,F_OK)){
				if(mkdir(path,0777) < 0)
					return -1;
			}
			argv1 = runNamsArgv2;
			argv2 = fileInstallArgv2;
		}
	}
	return 0;
}

static void runNams_entry(){
    set_android_app_param(my_state);
    runNams(3, argv1);   
}

static void *j2me_vm_thread(void *arg){
    fileInstaller(2,argv2);
    runNams_entry();
    return 0;
}


void copySkinFile()
{
	char cmd[128] = {0};
	if(!access(SKIN_BIN_PATH_BK,F_OK)){
		sprintf(cmd,"cp -rf %s /mnt/",SKIN_BIN_PATH_BK);
		system(cmd);
	}else{
		if(!access(SKIN_BIN_PATH,F_OK)){
			sprintf(cmd,"cp -rf %s /mnt/",SKIN_BIN_PATH);
			system(cmd);
		}
	}
}

void android_main(struct android_app* state) {
    pthread_attr_t attr;
    pthread_t ptid1;
    struct mengine engine;
    memset(&engine, 0, sizeof(engine));
    engine.in_buf = (char *)malloc(IN_WIDTH*IN_HEIGHT*IN_BPP);
    if(check_dir_path() < 0){
	LOGI("check dir path error!");
	return;
    }
    copySkinFile();
    app_dummy();
    my_state = state;
    state->onAppCmd = engine_handle_cmd;
    state->userData = &engine;
    engine.app = state;
    lock = get_pthread_mutex();
    pthread_attr_init (&attr);
    pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);    
    pthread_create(&ptid1,&attr,j2me_vm_thread,NULL);
    pthread_attr_destroy (&attr);
    while (1) {
        // Read all pending events.
        int ident;
        int events;
        struct android_poll_source* source;

        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while ((ident=ALooper_pollAll(0, NULL, &events,
                (void**)&source)) >= 0) {
            // Process this event.
            if (source != NULL) {
                source->process(state, source);
            }

            // Check if we are exiting.
            if (state->destroyRequested != 0) {
		//exit(1);
            }
        }
	if(image_buffer == NULL){
    		image_buffer = get_display_buffer_addr();
		if(image_buffer != NULL)
			engine.in_buf = image_buffer;
	}
	pthread_mutex_lock(lock);
	if(image_buffer != NULL){
		memcpy(engine.in_buf,image_buffer,IN_HEIGHT*IN_WIDTH*IN_BPP);
	}
	pthread_mutex_unlock(lock);
	render_frame(&engine);
    }
    //pthread_join(ptid1,NULL);  
}
