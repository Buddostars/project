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
#include "texture_loader.h" // For loading the background image as texture

// Define the GameState enum before using it
enum GameState {
    STATE_MENU,
    STATE_GAME,
    STATE_PAUSE,
    // Add more later if required
};

// Game state variable
GameState currentState = STATE_MENU;

// Declare the texture ID for the loading screen
unsigned int loadingScreenTexture;

// Function declarations
void processMenuInput(GLFWwindow* window);
void renderLoadingScreen(unsigned int backgroundTexture, Shader& quadShader);
void renderQuad(float x, float y, float width, float height);

// Function: processMenuInput
void processMenuInput(GLFWwindow* window) {
    // Detect when the user clicks "Start" and change to the game state
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        ypos = 768.0 - ypos; // Invert Y coordinate
        if (xpos >= 337.0 && xpos <= 687.0 && ypos >= 430.0 && ypos <= 500.0) {
            currentState = STATE_GAME;
        }
    }
}

// Function: renderLoadingScreen
void renderLoadingScreen(unsigned int backgroundTexture, Shader& quadShader) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Disable depth testing so 2D elements are rendered on top
    glDisable(GL_DEPTH_TEST);

    // Set up orthographic projection for 2D rendering
    glm::mat4 projection = glm::ortho(0.0f, 1024.0f, 0.0f, 768.0f);
    quadShader.use();
    quadShader.setMat4("projection", projection);

    // Bind the background texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, backgroundTexture);
    quadShader.setInt("texture1", 0);

    // Render the quad for the background
    renderQuad(0.0f, 0.0f, 1024.0f, 768.0f); // Full-screen background quad

    // Re-enable depth testing
    glEnable(GL_DEPTH_TEST);
}


// Function: renderQuad
void renderQuad(float x, float y, float width, float height) {
    float vertices[] = {
        // Positions          // Texture Coords
        x,          y + height,  0.0f, 1.0f, // Top-left
        x,          y,           0.0f, 0.0f, // Bottom-left
        x + width,  y,           1.0f, 0.0f, // Bottom-right

        x,          y + height,  0.0f, 1.0f, // Top-left
        x + width,  y,           1.0f, 0.0f, // Bottom-right
        x + width,  y + height,  1.0f, 1.0f  // Top-right
    };

    // Setup VAO and VBO if not already done
    static unsigned int quadVAO = 0, quadVBO;
    if (quadVAO == 0) {
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Texture Coords attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);
    } else {
        // Update vertex data
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    }

    // Render the textured quad
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Reset OpenGL states after rendering
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Main function
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

    // Load OpenGL functions using GLAD
    if (!gladLoadGL()) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Set viewport size
    glViewport(0, 0, 1024, 768);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);  // Dark grey background
    glEnable(GL_DEPTH_TEST); // Enable depth testing

    // Create shader programs
    Shader shaderProgram("src/shaders/vertex_shader.vert", "src/shaders/fragment_shader.frag");
    Shader quadShader("src/shaders/quad_shader.vert", "src/shaders/quad_shader.frag");

    std::cout << "Current Working Directory: " << std::filesystem::current_path() << std::endl;

    // Load the loading screen image as a texture
    loadingScreenTexture = loadTexture("src/loading-screen-image.png");

    // Check if the texture was loaded successfully
    if (loadingScreenTexture == 0) {
        std::cout << "Failed to load loading screen texture." << std::endl;
        return -1;
    } else {
        std::cout << "Loading screen texture loaded successfully. ID: " << loadingScreenTexture << std::endl;
    }

    // Load the cow model
    loadCowModel("src/models/cow.obj");

    // Set light properties
    glm::vec3 lightPos(1.2f, 100.0f, 2.0f);
    glm::vec3 lightDirection = glm::normalize(glm::vec3(0.0f, -1.0f, -0.3f));
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 0.9f);  // Slightly yellow
    glm::vec3 objectColor = glm::vec3(0.6f, 0.6f, 0.6f);  // Gray
    glm::vec3 viewPos(0.0f, 40.0f, 3.0f);

    // Initialize model position
    glm::vec3 modelPosition(0.0f, 0.0f, 0.0f);

    // Initialize time variables
    float lastTime = glfwGetTime();

    // Main loop
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window)) {
        glfwPollEvents(); // Process events

        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (currentState == STATE_MENU) {
            // Show the cursor in the menu
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

            // Process menu input
            processMenuInput(window);

            // Render the loading screen with background image
            renderLoadingScreen(loadingScreenTexture, quadShader);
        } else if (currentState == STATE_GAME) {
            // Hide the cursor during the game
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            // Activate the shader program
            shaderProgram.use();

            // Update camera and controls
            updateModelPosition(window, deltaTime);
            computeMatricesFromInputs(window, deltaTime);
            setThirdPersonView(modelPosition, glm::vec3(0.0f, 0.0f, -1.0f));

            // Set lighting uniforms
            shaderProgram.setVec3("lightPos", lightPos);
            shaderProgram.setVec3("lightDirection", lightDirection);
            shaderProgram.setVec3("lightColor", lightColor);
            shaderProgram.setVec3("viewPos", viewPos);
            shaderProgram.setVec3("objectColor", objectColor);

            // Set the model, view, and projection matrices
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 view = getViewMatrix();
            glm::mat4 projection = getProjectionMatrix();
            shaderProgram.setMat4("model", model);
            shaderProgram.setMat4("view", view);
            shaderProgram.setMat4("projection", projection);

            // Draw the cow model
            drawCow();
        }

        glfwSwapBuffers(window);
    }

    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
