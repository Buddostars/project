#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

// Function to create a perspective projection matrix
void setPerspective(float fov, float aspectRatio, float near, float far) {
    float f = 1.0f / tan(fov * 0.5f * 3.14f / 180.0f);

    float matrix[16] = {
        f / aspectRatio, 0, 0, 0,
        0, f, 0, 0,
        0, 0, (far + near) / (near - far), -1,
        0, 0, (2 * far * near) / (near - far), 0
    };

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(matrix);
}

void setCamera() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Camera position and orientation
    float eyeX = 1.5f, eyeY = 1.5f, eyeZ = 1.5f;
    float centerX = 0.0f, centerY = 0.0f, centerZ = 0.0f;
    float upX = 0.0f, upY = 1.0f, upZ = 0.0f;

    // Calculate forward, right, and up vectors
    float forwardX = centerX - eyeX;
    float forwardY = centerY - eyeY;
    float forwardZ = centerZ - eyeZ;

    // Normalize the forward vector
    float forwardLen = std::sqrt(forwardX * forwardX + forwardY * forwardY + forwardZ * forwardZ);
    forwardX /= forwardLen;
    forwardY /= forwardLen;
    forwardZ /= forwardLen;

    // Calculate right vector (cross product of forward and up vectors)
    float rightX = forwardY * upZ - forwardZ * upY;
    float rightY = forwardZ * upX - forwardX * upZ;
    float rightZ = forwardX * upY - forwardY * upX;

    // Normalize the right vector
    float rightLen = std::sqrt(rightX * rightX + rightY * rightY + rightZ * rightZ);
    rightX /= rightLen;
    rightY /= rightLen;
    rightZ /= rightLen;

    // Recalculate the orthogonal up vector (cross product of right and forward)
    upX = rightY * forwardZ - rightZ * forwardY;
    upY = rightZ * forwardX - rightX * forwardZ;
    upZ = rightX * forwardY - rightY * forwardX;

    // Create the look-at matrix
    float matrix[16] = {
        rightX,    upX,    -forwardX,    0.0f,
        rightY,    upY,    -forwardY,    0.0f,
        rightZ,    upZ,    -forwardZ,    0.0f,
        -(rightX * eyeX + rightY * eyeY + rightZ * eyeZ),
        -(upX * eyeX + upY * eyeY + upZ * eyeZ),
        forwardX * eyeX + forwardY * eyeY + forwardZ * eyeZ,
        1.0f
    };

    // Load the matrix
    glMultMatrixf(matrix);
}

void drawCube() {
    glBegin(GL_QUADS);

    // Front face
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);

    // Back face
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);

    // Left face
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);

    // Right face
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);

    // Top face
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);

    // Bottom face
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);

    glEnd();
}



GLuint shaderProgram;

char* loadShaderFile(const char* path) {
    FILE* file = nullptr;
    errno_t err = fopen_s(&file, path, "r");  // Using fopen_s instead of fopen
    if (err != 0 || file == nullptr) {
        printf("Failed to open file: %s\n", path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Ensure the file isn't empty or has an unexpected size
    if (length <= 0) {
        printf("Shader file is empty or invalid: %s\n", path);
        fclose(file);
        return NULL;
    }

    // Allocate buffer with room for null terminator
    char* buffer = (char*)malloc(length + 1);
    if (buffer == NULL) {
        printf("Failed to allocate memory for shader file\n");
        fclose(file);
        return NULL;
    }

    // Ensure fread reads the expected amount of data
    size_t readSize = fread(buffer, 1, length, file);
    if (readSize != length) {
        printf("Failed to read the complete file: %s\n", path);
        free(buffer);
        fclose(file);
        return NULL;
    }

    buffer[length] = '\0';  // Null-terminate the buffer

    fclose(file);
    return buffer;
}


// Function to load shaders from files
GLuint loadShader(const char* vertexPath, const char* fragmentPath) {
    // Step 1: Load the shader source code from files
    char* vShaderCode = loadShaderFile(vertexPath);
    char* fShaderCode = loadShaderFile(fragmentPath);

    if (vShaderCode == nullptr || fShaderCode == nullptr) {
        printf("Failed to load shader files.\n");
        return 0;  // Early exit if shaders can't be loaded
    }

    // Step 2: Create vertex and fragment shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);

    // Step 3: Check for shader compilation errors
    GLint success;
    char infoLog[512];

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
    }

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
    }

    // Step 4: Create shader program and link the shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Step 5: Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
    }

    // Step 6: Delete the shaders as they're linked into the program now
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Free the loaded shader source code
    free(vShaderCode);
    free(fShaderCode);



    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "Vertex Shader compilation error: " << infoLog << std::endl;
    }

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "Fragment Shader compilation error: " << infoLog << std::endl;
    }
    std::cout << infoLog << std::endl;
    return shaderProgram;  // Return the shader program ID
}

// Cube drawing and other functions remain unchanged...

int main(void) {

    GLFWwindow* window;

    if (!glfwInit()) {
        return -1;
    }

    window = glfwCreateWindow(640, 640, "PBR Cube Example", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewInit(); // Initialize GLEW

    // Load shaders
    //shaderProgram = loadShader("pbr.vert", "pbr.frag");



    // Set the viewport
    glViewport(0, 0, 640, 640);

    // Set up the custom perspective projection
    setPerspective(45.0f, 1.0f, 0.1f, 100.0f);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use the shader program
        glUseProgram(shaderProgram);

        // Set the camera and light positions
        setCamera();  // Set the camera with manual matrix

        // Update the shader uniforms
        //glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), 1.0f, 1.0f, 1.0f);
        //glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), 1.5f, 1.5f, 1.5f);

        drawCube();  // Draw the cube

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
