#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <unistd.h> // For changing the working directory
#include <filesystem>

#include <glm/glm.hpp>                  // For math operations (OpenGL Mathematics)
#include <glm/gtc/matrix_transform.hpp> // For transformations
#include <glm/gtc/type_ptr.hpp>         // For converting matrices to OpenGL-compatible pointers
#include <cmath>
#include <random>
#include <thread>  // Include this for std::this_thread::sleep_for
#include <chrono>  // Include this for std::chrono::milliseconds
#include <unordered_set>
#include <iostream>
#include <stb_image.h>

#include "shader.h"
#include "controls.hpp"
#include "texture_loader.h" // For loading the background image as texture
#include "model.hpp"
#include "globals.hpp"
#include "car.hpp"
#include "Cow_Character.h"

// Define the GameState enum before using it
enum GameState {
    STATE_MENU,
    STATE_GAME,
    STATE_PAUSE,
    // Add more later if required
};


// Game state variable
GameState currentState = STATE_MENU;

struct Vec3Hash {
    std::size_t operator()(const glm::vec3& v) const {
        return std::hash<float>()(v.x) ^ std::hash<float>()(v.y) ^ std::hash<float>()(v.z);
    }
};

// Function to initialize GLFW
GLFWwindow* initializeWindow() {
    // Change working directory to the project root only on Mac needed
    #ifdef __APPLE__
        chdir("..");
    #endif
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1024, 768, "Cows n Cars", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwPollEvents();
    glfwSetCursorPos(window, 1024 / 2, 768 / 2);

    // Load OpenGL functions
    gladLoadGL();
    glViewport(0, 0, 1024, 768);
    glEnable(GL_DEPTH_TEST); // Enable depth testing
    glClearColor(0.68f, 0.85f, 0.9f, 1.0f);  // Light blue color
    return window;
}

// Function to set light and object properties
void setLightingAndObjectProperties(Shader& shader) {
    glm::vec3 lightPos(1.2f, 100.0f, 2.0f);
    glm::vec3 lightDirection = glm::normalize(glm::vec3(0.0f, -1.0f, -0.3f));
    glm::vec3 lightColor(1.0f, 1.0f, 0.9f);
    glm::vec3 objectColor(0.6f, 0.6f, 0.6f);
    glm::vec3 viewPos(0.0f, 40.0f, 3.0f);

    shader.setVec3("lightPos", lightPos);
    shader.setVec3("lightDirection", lightDirection);
    shader.setVec3("lightColor", lightColor);
    shader.setVec3("viewPos", viewPos);
    shader.setVec3("objectColor", objectColor);
}

// Function to check the distance between two positions
bool isPositionValid(const glm::vec3& newPosition, const std::vector<glm::vec3>& existingPositions, float minDistance) {
    for (const auto& pos : existingPositions) {
        float distance = glm::distance(newPosition, pos);
        if (distance < minDistance) {
            return false;
        }
    }
    return true;
}

// Function to generate pseudo-random object positions with at least 5 units of distance between them
std::vector<glm::vec3> generateSpacedObjectPositions(int count, float range, float minDistance) {
    std::vector<glm::vec3> positions;

    // Random number generator for X and Z positions
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-range, range);

    while (positions.size() < count) {
        // Generate a new potential position
        glm::vec3 newPosition(dist(gen), 0.0f, dist(gen));

        // Check if the new position is valid (i.e., far enough from other positions)
        if (isPositionValid(newPosition, positions, minDistance)) {
            positions.push_back(newPosition);
        }
    }

    return positions;
}

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
    GLFWwindow* window = initializeWindow();
    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);
    // Create shader programs
    Shader quadShader("src/shaders/quad_shader.vert", "src/shaders/quad_shader.frag");
    Shader shaderProgram("src/shaders/vertex_shader.vert", "src/shaders/fragment_shader.frag");
    Shader objectShader("src/shaders/obj_vertex_shader.vert", "src/shaders/obj_fragment_shader.frag");

    // Load models
    Model big_rock("src/models/big_rock.obj");
    Model small_rock("src/models/small_rock.obj");
    Model tree("src/models/tree.obj");
    Model ground("src/models/ground.obj");
    Model carModel("src/models/car.obj");
    Model cowModel("src/models/new_cow.obj");

    Car car(carModel);
    Cow_Character cow(cowModel);

    // Create camera object
    Camera camera;

    int treeCount = 50;
    std::vector<glm::vec3> treePositions = generateSpacedObjectPositions(treeCount, 90.0f, 15.0f);  // Range -90 to 90, at least 5 units apart

    int bigRockCount = 80;
    std::vector<glm::vec3> bigRockPositions = generateSpacedObjectPositions(bigRockCount, 90.0f, 15.0f);

    int smallRockCount = 50;
    std::vector<glm::vec3> smallRockPositions = generateSpacedObjectPositions(smallRockCount, 90.0f, 15.0f);

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

    

    // // Set light properties
    // glm::vec3 lightPos(1.2f, 100.0f, 2.0f);
    // glm::vec3 lightDirection = glm::normalize(glm::vec3(0.0f, -1.0f, -0.3f));
    // glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 0.9f);  // Slightly yellow
    // glm::vec3 objectColor = glm::vec3(0.6f, 0.6f, 0.6f);  // Gray
    // glm::vec3 viewPos(0.0f, 40.0f, 3.0f);

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

            car.update(deltaTime, window);

            camera.computeMatricesFromInputs(window, car.getPosition(), car.getForwardDirection());
            
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            shaderProgram.use();
            objectShader.use();
            //shaderProgram.setSampler("texture_diffuse", 0);

            glm::mat4 view = camera.getViewMatrix();
            glm::mat4 projection = camera.getProjectionMatrix();
            setLightingAndObjectProperties(shaderProgram);
            setLightingAndObjectProperties(objectShader);

            // Draw the car model   
            car.draw(objectShader);

            // Draw the ground model
            glm::mat4 groundModel = glm::mat4(1.0f);
            groundModel = glm::translate(groundModel, glm::vec3(0.0f, 0.0f, 0.0f)); // Position of ground
            objectShader.setMat4("model", groundModel);
            objectShader.setMat4("view", view);
            objectShader.setMat4("projection", projection);
            ground.draw(objectShader); // Draw ground
            
            //Draw the tree model using fixed positions
            // for (const auto& position : treePositions) {
            //     glm::mat4 treeModel = glm::mat4(1.0f);
            //     treeModel = glm::translate(treeModel, position); // Use fixed position
            //     treeModel = glm::scale(treeModel, glm::vec3(0.5f, 0.5f, 0.5f)); // Scale trees if necessary
                
            //     objectShader.setMat4("model", treeModel);
            //     objectShader.setMat4("view", view);
            //     objectShader.setMat4("projection", projection);
            //     tree.draw(objectShader); // Draw tree
            // }

            // Draw the rocks
            for (const auto& position : smallRockPositions) {
                glm::mat4 smallRockkModel = glm::mat4(1.0f);
                smallRockkModel = glm::translate(smallRockkModel, position); // Use fixed position
                smallRockkModel = glm::scale(smallRockkModel, glm::vec3(3.5f, 3.5f, 3.5f)); // Scale trees if necessary
                
                objectShader.setMat4("model", smallRockkModel);
                objectShader.setMat4("view", view);
                objectShader.setMat4("projection", projection);
                small_rock.draw(objectShader); // Draw small rocks
            }

            for (const auto& position : bigRockPositions) {
                glm::mat4 bigRockkModel = glm::mat4(1.0f);
                bigRockkModel = glm::translate(bigRockkModel, position); // Use fixed position
                bigRockkModel = glm::scale(bigRockkModel, glm::vec3(1.5f, 1.5f, 1.5f)); // Scale trees if necessary
                
                objectShader.setMat4("model", bigRockkModel);
                objectShader.setMat4("view", view);
                objectShader.setMat4("projection", projection);
                big_rock.draw(objectShader); // Draw big rocks
            }

           // Update the cow's position
            cow.moveRandomly(deltaTime);  // Update position and movement logic

            // Draw the cow model using the updated position and rotation
            glm::mat4 cowModelMatrix = glm::mat4(1.0f);

            // Apply translation for the cow's position
            cowModelMatrix = glm::translate(cowModelMatrix, cow.getPosition());

            // Apply rotation for the cow's direction (use totalRotationAngle for smooth rotation)
            cowModelMatrix = glm::rotate(cowModelMatrix, glm::radians(cow.getTotalRotationAngle()), glm::vec3(0.0f, 1.0f, 0.0f));

            // Scale the cow to 0.1x size
            cowModelMatrix = glm::scale(cowModelMatrix, glm::vec3(0.1f, 0.1f, 0.1f));

            // Pass the updated matrices to the shader
            objectShader.setMat4("model", cowModelMatrix);
            objectShader.setMat4("view", view);
            objectShader.setMat4("projection", projection);

            // Render the cow
            cow.draw(objectShader);
        }

        glfwSwapBuffers(window);
    }

    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
