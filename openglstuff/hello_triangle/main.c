
#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <assert.h>

#define GL_LOG_FILE "gl.log"

int g_gl_width = 640;
int g_gl_height = 480;



void glfw_window_size_callback(GLFWwindow * window, int width, int height) {
    g_gl_width = width;
    g_gl_height = height;
    /* update perspective matrices here apparently */
}

/* combining gl_log and gl_log_err from the tutorial */
char gl_log(const char * message, ...) {
    va_list argptr;
    FILE * file = fopen(GL_LOG_FILE, "a");
    if (!file) {
        fprintf(stderr, "couldn't append to GL_LOG_FILE %s\n", GL_LOG_FILE);
        return 0; /* failed */
    }
    va_start(argptr, message);
    vfprintf(file, message, argptr);
    va_end(argptr);
    va_start(argptr, message);
    vfprintf(stderr, message, argptr);
    va_end(argptr);
    fclose(file);
    return 1; /* succeeded */
}


void log_gl_params() {
    GLenum params[] = {
        GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
        GL_MAX_CUBE_MAP_TEXTURE_SIZE,
        GL_MAX_DRAW_BUFFERS,
        GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
        GL_MAX_TEXTURE_IMAGE_UNITS,
        GL_MAX_TEXTURE_SIZE,
        GL_MAX_VARYING_FLOATS,
        GL_MAX_VERTEX_ATTRIBS,
        GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
        GL_MAX_VERTEX_UNIFORM_COMPONENTS,
        GL_MAX_VIEWPORT_DIMS,
        GL_STEREO,
    };
    const char* names[] = {
        "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS",
        "GL_MAX_CUBE_MAP_TEXTURE_SIZE",
        "GL_MAX_DRAW_BUFFERS",
        "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS",
        "GL_MAX_TEXTURE_IMAGE_UNITS",
        "GL_MAX_TEXTURE_SIZE",
        "GL_MAX_VARYING_FLOATS",
        "GL_MAX_VERTEX_ATTRIBS",
        "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS",
        "GL_MAX_VERTEX_UNIFORM_COMPONENTS",
        "GL_MAX_VIEWPORT_DIMS",
        "GL_STEREO",
    };
    gl_log("GL Context Params:\n");
    char msg[256];
    // integers - only works if the order is 0-10 integer return types
    for (int i = 0; i < 10; i++) {
        int v = 0;
        glGetIntegerv(params[i], &v);
        gl_log("%s %i\n", names[i], v);
    }
    // others
    int v[2];
    v[0] = v[1] = 0;
    glGetIntegerv(params[10], v);
    gl_log("%s %i %i\n", names[10], v[0], v[1]);
    unsigned char s = 0;
    glGetBooleanv(params[11], &s);
    gl_log("%s %u\n", names[11], (unsigned int)s);
    gl_log("-----------------------------\n");
}

char restart_gl_log() {
    FILE * fp = fopen(GL_LOG_FILE, "w");
    if (!fp) {
        fprintf(stderr, "ERROR can't write to log file\n");
        return 0; /* failed */
    }
    time_t now = time(NULL);
    char * date = ctime(&now);
    fprintf(fp, "GL_LOG_FILE log. local time %s\n", date);
    fclose(fp);
    return 1; /* success*/
}


/* read a file and make const char * result point to a null-terminated string
 * containing the file, or just NULL if something went wrong */
/* TODO needs to be better */
char * read_file(const char * const filename) {
    char * buffer = NULL;
    size_t length;
    FILE * fp;

    fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "ERROR can't open file %s\n", filename);
        return NULL;
    }
    
    fseek(fp, 0, SEEK_END);
    length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    buffer = malloc(length * sizeof(char));
    if (buffer == NULL) {
        fprintf(stderr, "ERROR failed to load file %s\n", filename);
        return NULL;
    }
    fread(buffer, 1, length, fp);
    fclose(fp);
    
    return buffer;
}


void glfw_error_callback(int error, const char * description) {
    gl_log("GLFW (ERROR): code %i msg: %s\n", error, description);
}


int main() {
    assert(restart_gl_log());
    /* make gl context and window with glfw */
    gl_log("GLFW: starting GLFW version %s\n", glfwGetVersionString());

    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit()) {
        gl_log("GLFW (ERROR): can't init glfw oops\n");
        return 1;
    }

    /* glfw version hints */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    /* antialiasing apparently */
    glfwWindowHint(GLFW_SAMPLES, 4);

    /* monitor info */
    GLFWmonitor * mon = glfwGetPrimaryMonitor();
    const GLFWvidmode * vmode = glfwGetVideoMode(mon);
    /* create window */
    // GLFWwindow * window = glfwCreateWindow(vmode->width, vmode->height,
    //         "trongle", mon, NULL);
    GLFWwindow * window = glfwCreateWindow(640, 480, "trongle", NULL, NULL);
    if (!window) {
        gl_log("GLFW (ERROR): can't create window\n");
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    log_gl_params(); /* needs a context */
    glfwSetWindowSizeCallback(window, glfw_window_size_callback);

    /* start glew extension handler */
    glewExperimental = GL_TRUE;
    glewInit();

    /* get version info */
    const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString(GL_VERSION); // version as a string
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);

    // tell GL to only draw onto a pixel if the shape is closer to the viewer
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    // depth-testing interprets a smaller value as "closer"
    glDepthFunc(GL_LESS);

    /* do a triangle */
    float points[] = {
        0.0f, 0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f
    };
    /* put the above triangle numbers up in the graphics card */
    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), points, GL_STATIC_DRAW);

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    /* using version 300 of glsl for my lil guy laptop, not sure how to choose
     * versions in general */
    const char * vertex_shader = read_file("triangle.vs.glsl");
    if (vertex_shader == NULL) return 1;
    const char * fragment_shader = read_file("triangle.fs.glsl");
    if (fragment_shader == NULL) return 1;

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);

    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, fs);
    glAttachShader(shader_program, vs);
    glLinkProgram(shader_program);

    while (!glfwWindowShouldClose(window)) {
        /* clear drawing surface */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, g_gl_width, g_gl_height);
        glUseProgram(shader_program);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
        glfwPollEvents();

        /* check if escape has been pressed and close the window if so */
        if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window, 1);
        }
    }

    // close GL context and any other GLFW resources
    glfwTerminate();
    
    free((char *) vertex_shader);
    free((char *) fragment_shader);

    return 0;
}
