#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>

#include "../../util/file.h"


/* globals */
/* to be written only by glfw_window_size_callback() */
int g_gl_width = 640;
int g_gl_height = 480;


void glfw_error_callback(int error, const char * description) {
    printf("GLFW (ERROR): code %i msg: %s\n", error, description);
}

void glfw_window_size_callback(GLFWwindow * window, int width, int height) {
    g_gl_width = width;
    g_gl_height = height;
    /* update perspective matrices here apparently */
}

/* TODO free memory properly */
int main() {
    fprintf(stderr, "GLFW: starting GLFW version %s\n", glfwGetVersionString());
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not initialize GLFW, exiting\n");
        return 1;
    }
    
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow * window = glfwCreateWindow(640, 480, "shader town", NULL, NULL);
    if (!window) {
        fprintf(stderr, "ERROR: GLFW cannot create window, exiting\n");
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSetWindowAspectRatio(window, 4, 3);
    glfwSetWindowSizeCallback(window, glfw_window_size_callback);

    glewExperimental = GL_TRUE;
    glewInit();

    /* tell GL to only draw onto a pixel if the shape is closer to the viewer */
    glEnable(GL_DEPTH_TEST);
    /* depth-testing interprets a smaller value as "closer" */
    glDepthFunc(GL_LESS);

    unsigned char const * vertex_shader = get_file("test.vs.glsl");
    if (vertex_shader == NULL) return 1;
    unsigned char const * fragment_shader = get_file("test.fs.glsl");
    if (fragment_shader == NULL) return 1;

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(vs);
    int status = -1;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        fprintf(stderr, "ERROR: GL shader inder %i did not compile\n", vs);
        return 1;
    }
    glCompileShader(fs);
    status = -1;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        fprintf(stderr, "ERROR: GL shader inder %i did not compile\n", vs);
        return 1;
    }

    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, fs);
    glAttachShader(shader_program, vs);
    glLinkProgram(shader_program);
    status = -1;

    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        fprintf(stderr, "ERROR: could not link shader program GL index %u\n",
                program);
        return 1;
    }

    GLint color_location = glGetUniformLocation(shader_program, "input_color");

    glUseProgram(shader_program);
    glUniform4f(color_location, 0.23f, 0.95f, 0.34f, 1.0f);

    return 0;
}
