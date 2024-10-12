#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <filesystem>

#include <glm/glm.hpp>                  // For math operations (OpenGL Mathematics)
#include <glm/gtc/matrix_transform.hpp> // For transformations
#include <glm/gtc/type_ptr.hpp>         // For converting matrices to OpenGL-compatible pointers

#include "shader.h"
#include "controls.hpp"
#include "cow.hpp"
#include "TextRenderer.h" // For text rendering

// Define the GameState enum before using it
enum GameState {
    STATE_MENU,
    STATE_GAME,
    STATE_PAUSE,
    // Add more later if required
};

// Global variables
GameState currentState = STATE_MENU;

// Function to process menu input
void processMenuInput(GLFWwindow* window) {
    // Detect when the user clicks "Start" and change to the game state
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        currentState = STATE_GAME;
    }
}

// Function to render the loading screen
void renderLoadingScreen(TextRenderer& textRenderer) {
    // Render a simple loading screen with text
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 projection = glm::ortho(0.0f, 1024.0f, 0.0f, 768.0f);
    textRenderer.SetProjection(projection);

    // Display title and "Start" button using the text renderer
    textRenderer.RenderText("Cows n Cars", 362.0f, 500.0f, 1.5f, glm::vec3(1.0f));
    textRenderer.RenderText("Click to Start", 400.0f, 300.0f, 1.0f, glm::vec3(1.0f));
}

void checkCompileErrors(unsigned int shader, std::string type) {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n";
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n";
        }
    }
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a window
    GLFWwindow* window = glfwCreateWindow(1024, 768, "Cows n Cars", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Hide the mouse and enable unlimited movement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024 / 2, 768 / 2);

    // Load OpenGL functions using GLAD
    gladLoadGL();
    glViewport(0, 0, 1024, 768);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);  // Dark grey background
    glEnable(GL_DEPTH_TEST); // Enable depth testing

    // Create shader programs
    Shader shaderProgram("src/shaders/vertex_shader.vert", "src/shaders/fragment_shader.frag");
    Shader textShader("src/shaders/text_shader.vert", "src/shaders/text_shader.frag");

    // Initialize Text Renderer
    TextRenderer textRenderer("src/fonts/arial.ttf", textShader);

    std::cout << "Current Working Directory: " << std::filesystem::current_path() << std::endl;

    // Load object with assimp
    loadCowModel("src/models/cow.obj");

    glm::vec3 modelPosition = glm::vec3(0.0f, 0.0f, 0.0f);

    // Set light properties
    glm::vec3 lightPos(1.2f, 100.0f, 2.0f);

    // Set the directional light's direction pointing downwards
    glm::vec3 lightDirection = glm::normalize(glm::vec3(0.0f, -1.0f, -0.3f));

    // Set the light color (like sunlight)
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 0.9f);  // Slightly yellow

    // Set the object color
    glm::vec3 objectColor = glm::vec3(0.6f, 0.6f, 0.6f);  // Gray

    // Set camera position (viewer's position)
    glm::vec3 viewPos(0.0f, 0.0f, 0.0f);

    // Initialize time variables
    float lastTime = glfwGetTime();

    // Main loop
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window)) {
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (currentState == STATE_MENU) {
            // Show the cursor in the menu
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            processMenuInput(window);
            renderLoadingScreen(textRenderer);
        } else if (currentState == STATE_GAME) {
            // Hide the cursor during the game
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            // Update the model position
            updateModelPosition(window, deltaTime);

            // Compute controls
            computeMatricesFromInputs(window, deltaTime);

            setThirdPersonView(modelPosition, glm::vec3(0.0f, 0.0f, -1.0f));

            // Activate the shader program
            shaderProgram.use();

            // Set lighting uniforms
            shaderProgram.setVec3("lightPos", lightPos);
            shaderProgram.setVec3("lightDirection", lightDirection);
            shaderProgram.setVec3("lightColor", lightColor);
            shaderProgram.setVec3("viewPos", viewPos);
            shaderProgram.setVec3("objectColor", objectColor);

            // Set the model, view, and projection matrices
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, modelPosition); // No rotation applied
            glm::mat4 view = getViewMatrix();
            glm::mat4 projection = getProjectionMatrix();

            shaderProgram.setMat4("model", model);
            shaderProgram.setMat4("view", view);
            shaderProgram.setMat4("projection", projection);

            // Draw the mesh (cow model)
            drawCow();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
