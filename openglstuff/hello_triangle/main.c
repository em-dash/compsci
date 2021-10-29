
#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

#define GL_LOG_FILE "gl.log"

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
/* NOTE this is bad, i should probably make a generalized function for doing
 * this or something.  no i don't want to use the gnu extension thing. */
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


int main() {
    /* make gl context and window with glfw */
    if (!glfwInit()) {
        fprintf(stderr, "ERROR can't do glfw oops\n");
        return 1;
    }

    GLFWwindow* window = glfwCreateWindow(640, 480, "henlo triangle", NULL,
        NULL);
    if (!window) {
        fprintf(stderr, "ERROR can't do window with glfw heck\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

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
        glUseProgram(shader_program);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    // close GL context and any other GLFW resources
    glfwTerminate();
    
    free((char *) vertex_shader);
    free((char *) fragment_shader);

    return 0;
}
