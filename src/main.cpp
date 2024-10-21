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
#include <future>
#include <sstream>
#include <iomanip>

#include "shader.h"
#include "controls.hpp"
#include "texture_loader.h" // For loading the background image as texture
#include "model.hpp"
#include "globals.hpp"
#include "car.hpp"
#include "Cow_Character.h"
#include "Giraffe_Character.h"
#include "ExhaustSystem.h"
#include "TextRenderer.h"   // To show the game score

// Define the GameState enum before using it
enum GameState {
    STATE_MENU,
    STATE_GAME,
    STATE_END_GAME, // Added this line
    STATE_PAUSE,
    // Add more later if required
};

// Game state variable
GameState currentState = STATE_MENU;

// Game variables
float gameStartTime = 0.0f;
float gameTimeElapsed = 0.0f;
int gameScore = 0;
bool gameStarted = false;

// Game logic variables
bool doOnce = true;
bool cowInflated = false;

struct Vec3Hash {
    std::size_t operator()(const glm::vec3& v) const {
        return std::hash<float>()(v.x) ^ std::hash<float>()(v.y) ^ std::hash<float>()(v.z);
    }
};

// Define a struct for the "PLAY AGAIN" button
struct Button {
    float x;
    float y;
    float width;
    float height;
};

Button playAgainButton; // Global variable for the "PLAY AGAIN" button

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
        float xRNG = dist(gen);
        float xSpan = xRNG + ((xRNG / abs(xRNG)) * 5.0); // stop spawning 5 units left and right of origin (car position)
        float zRNG = dist(gen);
        float zSpan = zRNG + ((zRNG / abs(zRNG)) * 3.0); // stop spawning 3 units left and right of origin (car position)
        glm::vec3 newPosition(xSpan, 0.0f, zSpan);

        // Check if the new position is valid (i.e., far enough from other positions)
        if (isPositionValid(newPosition, positions, minDistance)) {
            positions.push_back(newPosition);
        }
    }

    return positions;
}

// Function to generate bowling pin positions with at least 5 units of distance between them
std::vector<glm::vec3> generateBowlingPinPositions(glm::vec3 center) {
    std::vector<glm::vec3> positions;

    // line 1
    positions.push_back(center + glm::vec3(0.0f, 0.0f, 2.0f));

    // line 2
    positions.push_back(center + glm::vec3(-0.5f, 0.0f, 1.0f));
    positions.push_back(center + glm::vec3(0.5f, 0.0f, 1.0f));

    // line 3
    positions.push_back(center + glm::vec3(-1.0f, 0.0f, 0.0f));
    positions.push_back(center);
    positions.push_back(center + glm::vec3(1.0f, 0.0f, 0.0f));

    // line 4
    positions.push_back(center + glm::vec3(-1.5f, 0.0f, -1.0f));
    positions.push_back(center + glm::vec3(-0.5f, 0.0f, -1.0f));
    positions.push_back(center + glm::vec3(0.5f, 0.0f, -1.0f));
    positions.push_back(center + glm::vec3(1.5f, 0.0f, -1.0f));

    return positions;
}

// Declare the texture ID for the loading screen
unsigned int loadingScreenTexture;

// Function declarations
void processMenuInput(GLFWwindow* window);
void processEndGameInput(GLFWwindow* window, Car& car, Cow_Character& cow, std::vector<Giraffe_Character>& giraffes, Model& carModel, Model& cowModel, Model& giraffeModel);
void renderLoadingScreen(unsigned int backgroundTexture, Shader& quadShader);
void renderEndGameScreen(Shader& quadShader, TextRenderer& textRenderer, int gameScore);
void renderQuad(float x, float y, float width, float height);
void resetGame(Car& car, Cow_Character& cow, std::vector<Giraffe_Character>& giraffes, Model& carModel, Model& cowModel, Model& giraffeModel);

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

// Function to process input in the end game state
void processEndGameInput(GLFWwindow* window, Car& car, Cow_Character& cow, std::vector<Giraffe_Character>& giraffes, Model& carModel, Model& cowModel, Model& giraffeModel) {
    // Detect when the user clicks "PLAY AGAIN" and reset the game
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        ypos = 818.0 - ypos; // Invert Y coordinate

        // Check if the click is within the button area
        if (xpos >= playAgainButton.x && xpos <= playAgainButton.x + playAgainButton.width &&
            ypos >= playAgainButton.y && ypos <= playAgainButton.y + playAgainButton.height) {
            resetGame(car, cow, giraffes, carModel, cowModel, giraffeModel);
            currentState = STATE_MENU;
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

// Function to render the end game screen
void renderEndGameScreen(Shader& quadShader, TextRenderer& textRenderer, int gameScore) {
    // Set clear color to black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Disable depth testing so 2D elements are rendered on top
    glDisable(GL_DEPTH_TEST);

    // Set up orthographic projection for 2D rendering
    glm::mat4 projection = glm::ortho(0.0f, 1024.0f, 0.0f, 768.0f);

    // Render "GAME OVER" text
    textRenderer.SetProjection(projection);
    float gameOverScale = 2.0f;
    std::string gameOverText = "GAME OVER";
    float gameOverWidth = textRenderer.CalculateTextWidth(gameOverText, gameOverScale);
    textRenderer.RenderText(gameOverText, 512.0f - gameOverWidth / 2.0f, 600.0f, gameOverScale, glm::vec3(1.0f));

    // Render "SCORE: X" text
    float scoreScale = 1.5f;
    std::string scoreText = "SCORE: " + std::to_string(gameScore);
    float scoreWidth = textRenderer.CalculateTextWidth(scoreText, scoreScale);
    textRenderer.RenderText(scoreText, 512.0f - scoreWidth / 2.0f, 500.0f, scoreScale, glm::vec3(1.0f));

    // Render "PLAY AGAIN" button as text
    float buttonScale = 1.5f;
    std::string buttonText = "PLAY_AGAIN_IN_NIGHT_MODE";
    float buttonWidth = textRenderer.CalculateTextWidth(buttonText, buttonScale);
    float buttonX = 512.0f - buttonWidth / 2.0f;
    float buttonY = 400.0f; // y-position of the button
    float buttonHeight = 50.0f; // approximate height of the text, adjust as needed

    // Store the button area in the global variable
    playAgainButton.x = buttonX;
    playAgainButton.y = buttonY;
    playAgainButton.width = buttonWidth;
    playAgainButton.height = buttonHeight;

    textRenderer.RenderText(buttonText, buttonX, buttonY, buttonScale, glm::vec3(1.0f));

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

// Hitbox for the walls around map
std::vector<Hitbox> wallHitboxes;

void initializeWallsFromGround(const Hitbox& groundHitbox) {
    glm::vec3 groundMin = groundHitbox.minCorner;
    glm::vec3 groundMax = groundHitbox.maxCorner;

    // Create Left Wall
    glm::vec3 leftWallMin = glm::vec3(groundMin.x, 0.0f, groundMin.z);
    glm::vec3 leftWallMax = glm::vec3(groundMin.x + 1.0f, 5.0f, groundMax.z);
    wallHitboxes.push_back(Hitbox(leftWallMin, leftWallMax));
    std::cout << "Left Wall Min: " << leftWallMin.x << ", " << leftWallMin.y << ", " << leftWallMin.z << std::endl;
    std::cout << "Left Wall Max: " << leftWallMax.x << ", " << leftWallMax.y << ", " << leftWallMax.z << std::endl;

    // Create Right Wall
    glm::vec3 rightWallMin = glm::vec3(groundMax.x - 1.0f, 0.0f, groundMin.z);
    glm::vec3 rightWallMax = glm::vec3(groundMax.x, 5.0f, groundMax.z);
    wallHitboxes.push_back(Hitbox(rightWallMin, rightWallMax));
    std::cout << "Right Wall Min: " << rightWallMin.x << ", " << rightWallMin.y << ", " << rightWallMin.z << std::endl;
    std::cout << "Right Wall Max: " << rightWallMax.x << ", " << rightWallMax.y << ", " << rightWallMax.z << std::endl;

    // Create Front Wall(north)
    glm::vec3 frontWallMin = glm::vec3(groundMin.x, 0.0f, groundMax.z - 1.0f);
    glm::vec3 frontWallMax = glm::vec3(groundMax.x, 5.0f, groundMax.z);
    wallHitboxes.push_back(Hitbox(frontWallMin, frontWallMax));
    std::cout << "Front Wall Min: " << frontWallMin.x << ", " << frontWallMin.y << ", " << frontWallMin.z << std::endl;
    std::cout << "Front Wall Max: " << frontWallMax.x << ", " << frontWallMax.y << ", " << frontWallMax.z << std::endl;

    // Create Back Wall(south)
    glm::vec3 backWallMin = glm::vec3(groundMin.x, 0.0f, groundMin.z);
    glm::vec3 backWallMax = glm::vec3(groundMax.x, 5.0f, groundMin.z + 1.0f);
    wallHitboxes.push_back(Hitbox(backWallMin, backWallMax));
    std::cout << "Back Wall Min: " << backWallMin.x << ", " << backWallMin.y << ", " << backWallMin.z << std::endl;
    std::cout << "Back Wall Max: " << backWallMax.x << ", " << backWallMax.y << ", " << backWallMax.z << std::endl;
}

// Function to reset the game
void resetGame(Car& car, Cow_Character& cow, std::vector<Giraffe_Character>& giraffes, Model& carModel, Model& cowModel, Model& giraffeModel) {
    // Reset game variables
    gameScore = 0;
    gameStartTime = glfwGetTime();
    gameTimeElapsed = 0.0f;
    gameStarted = false;
    doOnce = true;
    cowInflated = false;

    // Reset cow and car position and state
    car.reset();
    cow.reset();

    // Reset giraffes
    glm::vec3 center(0.0f, 0.0f, -20.0f);
    std::vector<glm::vec3> positions = generateBowlingPinPositions(center);
    for (size_t i = 0; i < giraffes.size(); ++i) {
        giraffes[i].reset(positions[i]);
    }
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
    Shader objectShader2("src/shaders/obj_vertex_shader.vert", "src/shaders/obj_fragment_shader.frag");
    Shader smokeShader("src/shaders/particle_vertex_shader.vert", "src/shaders/particle_fragment_shader.frag");
    Shader textShader("src/shaders/text_shader.vert", "src/shaders/text_shader.frag");

    // Load models
    Model big_rock("src/models/big_rock.obj");
    Model small_rock("src/models/small_rock.obj");
    Model tree("src/models/tree.obj");
    Model ground("src/models/ground.obj");
    Model carModel("src/models/car.obj");
    Model cowModel("src/models/new_cow.obj");
    Model giraffeModel("src/models/new_giraffe.obj");
    
    // Create game objects
    Hitbox groundHitbox = ground.calculateHitbox();
    glm::vec3 groundMin = groundHitbox.minCorner;
    glm::vec3 groundMax = groundHitbox.maxCorner;

    Car car(carModel);
    Cow_Character cow(cowModel);

    std::vector<Giraffe_Character> giraffes;
    glm::vec3 center(0.0f, 0.0f, -20.0f);
    for (const auto& position : generateBowlingPinPositions(center)) {
        giraffes.emplace_back(giraffeModel, position);  // This will use the move constructor
        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Wait for 100 milliseconds
    }

    // Particle system for smoke (position the exhaust pipe relatively to the car)
    glm::vec3 exhaustOffset = glm::vec3(-1.0f, 0.5f, -2.0f);  // Example exhaust position on the left side of the car
    ExhaustSystem exhaustSystem(100, exhaustOffset);  // Max 100 particles

    // Create camera object
    Camera camera;

    int treeCount = 10;
    std::vector<glm::vec3> treePositions = generateSpacedObjectPositions(treeCount, 90.0f, 15.0f);  // Range -90 to 90, at least 5 units apart

    int bigRockCount = 15;
    std::vector<glm::vec3> bigRockPositions = generateSpacedObjectPositions(bigRockCount, 90.0f, 15.0f);

    int smallRockCount = 30;
    std::vector<glm::vec3> smallRockPositions = generateSpacedObjectPositions(smallRockCount, 90.0f, 15.0f);

    // Hitboxes for collision detection
    std::vector<Hitbox> environmentHitboxes;

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

    // Initialize time variables
    float lastTime = glfwGetTime();

    // Initialize the walls around the map
    initializeWallsFromGround(groundHitbox);

    // Print the working directory
    std::cout << "Current Working Directory: " << std::filesystem::current_path() << std::endl;

    // Initialise Text Renderer
    TextRenderer textRenderer("src/fonts/arial.ttf", textShader, 30.0f);

    // Set up projection matrix for text rendering
    glm::mat4 textProjection = glm::ortho(0.0f, static_cast<float>(1024), 0.0f, static_cast<float>(768));
    textRenderer.SetProjection(textProjection);

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
            if (!gameStarted) {
                gameStartTime = glfwGetTime();
                gameScore = 0;
                gameStarted = true;
            }

            // Compute game time elapsed
            gameTimeElapsed = currentTime - gameStartTime;

            // Check if 30 seconds have passed
            if (gameTimeElapsed >= 30.0f) {
                currentState = STATE_END_GAME;
                gameStarted = false;
                std::cout << "Game over! Final Score: " << gameScore << std::endl;
            }

            car.update(deltaTime, window, exhaustSystem, environmentHitboxes, wallHitboxes);

            camera.computeMatricesFromInputs(window, car.getPosition(), car.getForwardDirection());
            
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            shaderProgram.use();
            objectShader.use();
            objectShader2.use();
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
           
            // Draw the rocks
            for (const auto& position : smallRockPositions) {
                Hitbox smallRockHitBox = small_rock.calculateHitbox();
                smallRockHitBox.minCorner += position;
                smallRockHitBox.maxCorner += position;
                // Check if this hitbox already exists in environmentHitboxes
                bool isUnique = std::none_of(environmentHitboxes.begin(), environmentHitboxes.end(),
                                            [&](const Hitbox& hitbox) {
                                                return hitbox == smallRockHitBox;
                                            });

                if (isUnique) {
                    environmentHitboxes.push_back(smallRockHitBox);
                }

                glm::mat4 smallRockkModel = glm::mat4(1.0f);
                smallRockkModel = glm::translate(smallRockkModel, position); // Use fixed position
                smallRockkModel = glm::scale(smallRockkModel, glm::vec3(3.5f, 3.5f, 3.5f)); // Scale trees if necessary
                
                objectShader.setMat4("model", smallRockkModel);
                objectShader.setMat4("view", view);
                objectShader.setMat4("projection", projection);
                small_rock.draw(objectShader); // Draw small rocks
            }

            for (const auto& position : bigRockPositions) {
                Hitbox bigRockHitBox = big_rock.calculateHitbox();
                bigRockHitBox.minCorner += position;
                bigRockHitBox.maxCorner += position;
                // Check if this hitbox already exists in environmentHitboxes
                bool isUnique = std::none_of(environmentHitboxes.begin(), environmentHitboxes.end(),
                                            [&](const Hitbox& hitbox) {
                                                return hitbox == bigRockHitBox;
                                            });

                if (isUnique) {
                    environmentHitboxes.push_back(bigRockHitBox);
                }


                glm::mat4 bigRockkModel = glm::mat4(1.0f);
                bigRockkModel = glm::translate(bigRockkModel, position); // Use fixed position
                bigRockkModel = glm::scale(bigRockkModel, glm::vec3(1.5f, 1.5f, 1.5f)); // Scale trees if necessary
                
                objectShader.setMat4("model", bigRockkModel);
                objectShader.setMat4("view", view);
                objectShader.setMat4("projection", projection);
                big_rock.draw(objectShader); // Draw big rocks
            }

           // Update the cow's position
            cow.moveRandomly(deltaTime, environmentHitboxes, wallHitboxes);  // Update position and movement logic

            glm::mat4 cowModelMatrix = glm::mat4(1.0f);
            cowModelMatrix = glm::translate(cowModelMatrix, cow.getPosition());
            cowModelMatrix = glm::rotate(cowModelMatrix, glm::radians(cow.getTotalRotationAngle()), glm::vec3(0.0f, 1.0f, 0.0f));
            cowModelMatrix = glm::scale(cowModelMatrix, glm::vec3(0.1f, 0.1f, 0.1f));

            // Pass the updated matrices to the shader
            objectShader.setMat4("model", cowModelMatrix);
            objectShader.setMat4("view", view);
            objectShader.setMat4("projection", projection);

            // Render the cow
            cow.draw(objectShader);

            // Use a thread pool for giraffe updates
            std::vector<std::future<void>> futures;
            
            for (auto& giraffe : giraffes) {
                futures.push_back(std::async(std::launch::async, [&giraffe, deltaTime]() {
                    giraffe.moveRandomly(deltaTime);
                }));
            }

            // Wait for all giraffes to finish updating
            for (auto& future : futures) {
                future.get();  // Ensure all updates are complete
            }

            // Render the giraffes after movement updates
            for (auto& giraffe : giraffes) {
                glm::mat4 giraffeModelMatrix = glm::mat4(1.0f);
                giraffeModelMatrix = glm::translate(giraffeModelMatrix, giraffe.getPosition());
                giraffeModelMatrix = glm::rotate(giraffeModelMatrix, glm::radians(giraffe.getTotalRotationAngle()), glm::vec3(0.0f, 1.0f, 0.0f));
                giraffeModelMatrix = glm::scale(giraffeModelMatrix, glm::vec3(0.2f, 0.2f, 0.2f));

                objectShader2.setMat4("model", giraffeModelMatrix);
                objectShader2.setMat4("view", view);
                objectShader2.setMat4("projection", projection);
                giraffe.draw(objectShader2);
            }

            // Render smoke particles
            exhaustSystem.render(smokeShader, view, projection);

            // Check for collisions between the car and the cow
            if (car.getHitbox().isColliding(cow.getHitbox())) {
                
                // prevent multiple knockback force if there is still collision on next frames
                if (doOnce) { 
                    glm::vec3 hitDirection = cow.getPosition() - car.getPosition();
                    cow.gameHit(hitDirection, car.getSpeed());  // Pass car speed and direction to apply knockback
                    car.gameHit();
                    // doOnce = false; //commented out to allow multiple chances to hit giraffes with cow
                    cowInflated = true;
                }
            }

            if (cowInflated) {
                for (auto& giraffe : giraffes) {
                    if (cow.getHitbox().isColliding(giraffe.getHitbox())) {
                        glm::vec3 hitDirection = giraffe.getPosition() - cow.getPosition();
                        giraffe.gameHit(hitDirection, cow.getSpeed(), deltaTime);

                        // Score one point for each giraffe hit by a cow
                        gameScore++;
                        std::cout << "Giraffe hit! Current Score: " << gameScore << std::endl;
                    }
                    giraffe.update(deltaTime);
                }
            }

            // Check for collisions between the car and the environment
            for (const auto& hitbox : environmentHitboxes) {
                if (car.getHitbox().isColliding(hitbox)) {
                    std::cout << "Car and environment collided!" << std::endl;
                }
            }

            // Render the score at the top left
            std::string scoreText = "SCORE: " + std::to_string(gameScore);
            textRenderer.RenderText(scoreText, 25.0f, 725.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f)); // White color

            // Render the remaining time at the top right
            std::stringstream stream;
            stream << std::fixed << std::setprecision(2) << (30.0f - gameTimeElapsed);
            std::string timeText = "TIME: " + stream.str();
            textRenderer.RenderText(timeText, 875.0f, 725.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f)); // White color

        } else if (currentState == STATE_END_GAME) {
            // Show the cursor in the end game menu
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

            // Process end game input
            processEndGameInput(window, car, cow, giraffes, carModel, cowModel, giraffeModel);

            // Render the end game screen
            renderEndGameScreen(quadShader, textRenderer, gameScore);
        }

        glfwSwapBuffers(window);
    }

    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
