
<<<<<<< Updated upstream
#include <iostream>
#include <vector>
#include <filesystem>
=======
// #include <glad/glad.h>
// #include <GLFW/glfw3.h>

// #include <iostream>
// #include <vector>
// #include <filesystem>

// #include <assimp/Importer.hpp>    // C++ importer interface 
// #include <assimp/scene.h>         // Output data structure
// #include <assimp/postprocess.h>   // Post processing flags
>>>>>>> Stashed changes

// #include <glm/glm.hpp>                  // For math operations (OpenGL Mathematics)
// #include <glm/gtc/matrix_transform.hpp> // For transformations
// #include <glm/gtc/type_ptr.hpp>         // For converting matrices to OpenGL-compatible pointers

<<<<<<< Updated upstream
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
=======
// #include "shaders/shader.h"
// #include "controls.hpp"
// #include "models/object.h"
// #include "display.h"

// // Function prototypes
// void framebuffer_size_callback(GLFWwindow* window, int width, int height);
// void processInput(GLFWwindow *window);
// void setLight(Shader shaderProgram, glm::mat4 view);
// void drawObject(const Object& object, Shader& shaderProgram, glm::mat4 view, glm::mat4 projection);

// int main() {

//     // Global camera position and speed
//     glm::vec3 cameraPos(0.0f, 0.0f, 0.0f); // Initial camera position
//     float cameraSpeed = 0.05f; // Camera speed

//     // Create display
//     GLFWwindow* window = DisplayManager::createDisplay();
//     if (!window) return -1;

//     // Set up OpenGL options
//     glEnable(GL_DEPTH_TEST); // Enable depth testing
//     glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // Set callback for resizing

//     // Set clear color for the background
//     glClearColor(0.68f, 0.85f, 0.9f, 1.0f); // Light blue color

//     // Create shader program
//     Shader shaderProgram("src/shaders/vertex_shader.vert", "src/shaders/fragment_shader.frag");

//     // Instantiate objects manually (without using addObject)
//     Object terrain("src/models/Terrain/Terrain.obj", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f));
//     Object tree("src/models/Tree/Tree.obj", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f));
//     Object big_rock("src/models/Rock/Big_Rocks.obj", glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f));
//     Object small_rock("src/models/Car/car.obj", glm::vec3(4.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f));
//     std::cout<<"grass done"<<std::endl;
//     // Main game loop
//     while (!glfwWindowShouldClose(window)) {
//         // Input handling
//         processInput(window);
//         //camera.move();
//         // Clear the screen with color and depth buffer
//         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//         // Activate the shader program
//         shaderProgram.use();

//         // Compute camera controls (get view/projection matrices)
//         computeMatricesFromInputs(window);
//         glm::mat4 view = getViewMatrix();
//         glm::mat4 projection = getProjectionMatrix();

//         // Set up the light
//         setLight(shaderProgram, view);
        
//         // Draw tree object using helper function
//         drawObject(tree, shaderProgram, view, projection);

//         // Draw cow object using helper function
//         drawObject(big_rock, shaderProgram, view, projection);

//         // Draw cow object using helper function
//         drawObject(small_rock, shaderProgram, view, projection);

//         drawObject(terrain, shaderProgram, view, projection);
//         //renderer.render(light, camera);
//         DisplayManager::updateDisplay(window);
//     }

//     // Close display
//     DisplayManager::closeDisplay(window);

//     return 0;
// }

// // Helper function to set lighting parameters
// void setLight(Shader shaderProgram, glm::mat4 view) {
//     glm::vec3 lightPos(0.0f, 5.0f, 0.0f);
//     glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
//     glm::vec3 ambientColor(0.2f, 0.2f, 0.2f); // Dark gray for ambient light
//     glm::vec3 viewPos = glm::vec3(view[3]); // Extract camera position from the view matrix

//     // Set lighting uniforms
//     shaderProgram.setVec3("lightPos", lightPos);
//     shaderProgram.setVec3("lightColor", lightColor);
//     shaderProgram.setVec3("ambientColor", ambientColor); // Add ambient light color
//     shaderProgram.setVec3("viewPos", viewPos); // Pass dynamic camera position
// }

// // Helper function to draw an object
// void drawObject(const Object& object, Shader& shaderProgram, glm::mat4 view, glm::mat4 projection) {
//     // Initialize the model matrix
//     glm::mat4 model = glm::mat4(1.0f);

//     // Apply object transformations (position and scale)
//     model = glm::translate(model, object.getPosition()); // Apply object position
//     model = glm::scale(model, object.getScale()); // Apply object scale

//     // Set the model, view, and projection matrices in the shader
//     shaderProgram.setMat4("model", model);
//     shaderProgram.setMat4("view", view);
//     shaderProgram.setMat4("projection", projection);

//     // Draw the object using its own draw method
//     object.Draw(shaderProgram);
// }

// // Handle window resize
// void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
//     glViewport(0, 0, width, height);
// }

// // Handle input
// void processInput(GLFWwindow *window) {
//     if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//         glfwSetWindowShouldClose(window, true);
// }

/*

        Copyright 2022 Etay Meiri

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Terrain Rendering - demo 1
*/
>>>>>>> Stashed changes
