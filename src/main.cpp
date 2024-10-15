// main.cpp

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
// #include <unistd.h>
#include <vector>

#include <assimp/Importer.hpp>    // C++ importer interface 
#include <assimp/scene.h>         // Output data structure
#include <assimp/postprocess.h>   // Post processing flags

#include <glm/glm.hpp>                  // For math operations (OpenGL Mathematics)
#include <glm/gtc/matrix_transform.hpp> // For transformations
#include <glm/gtc/type_ptr.hpp>         // For converting matrices to OpenGL-compatible pointers

#include "shader.h"
#include "controls.hpp"
#include "texture_loader.h" // For loading the background image as texture

#include <filesystem>

#include "TextRenderer.h"

// Define the GameState enum before using it
enum GameState {
    STATE_MENU,
    STATE_GAME,
    STATE_PAUSE,
    // Add more later if required
};

// Global variables
GameState currentState = STATE_MENU;

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

std::vector<Vertex> vertices;
std::vector<unsigned int> indices;
unsigned int VAO, VBO, EBO;

// Declare the texture ID for the loading screen
unsigned int loadingScreenTexture;

// Function declarations (prototypes) for functions used in main
void processNode(aiNode* node, const aiScene* scene);
void processMenuInput(GLFWwindow* window);
void renderLoadingScreen(unsigned int backgroundTexture);
void drawMesh();
void renderQuad(float x, float y, float width, float height);
void processMesh(aiMesh* mesh, const aiScene* scene);
void setupMesh();

// Main Function
int main() {
    // Change working directory to the project root only on Mac needed
    #ifdef __APPLE__
        chdir("..");
    #endif

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
    Shader textShader("src/shaders/text_shader.vert", "src/shaders/text_shader.frag");

    // Initialize Text Renderer
    TextRenderer textRenderer("src/fonts/arial.ttf", textShader);

    std::cout << "Current Working Directory: " << std::filesystem::current_path() << std::endl;

    // Load the loading screen image as a texture
    loadingScreenTexture = loadTexture("src/loading-screen-image.png");

    // Load object with Assimp
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile("src/models/cow.obj", aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return -1;
    }

    processNode(scene->mRootNode, scene);
    std::cout << "Loaded " << vertices.size() << " vertices and " << indices.size() << " indices." << std::endl;

    // Set light properties
    glm::vec3 lightPos(1.2f, 100.0f, 2.0f);

    // Set the directional light's direction pointing downwards
    glm::vec3 lightDirection = glm::normalize(glm::vec3(0.0f, -1.0f, -0.3f));

    // Set the light color (like sunlight)
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 0.9f);  // Slightly yellow

    // Set the object color
    glm::vec3 objectColor = glm::vec3(0.6f, 0.6f, 0.6f);  // Gray

    // Set camera position (viewer's position)
    glm::vec3 viewPos(0.0f, 40.0f, 3.0f);

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
            renderLoadingScreen(loadingScreenTexture);
        } else if (currentState == STATE_GAME) {
            // Hide the cursor during the game
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            // Activate the shader program
            shaderProgram.use();

            // Compute controls
            computeMatricesFromInputs(window, deltaTime);

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

            // Rotate the model
            float timeValue = glfwGetTime();
            float angle = timeValue * glm::radians(50.0f);
            model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));

            shaderProgram.setMat4("model", model);
            shaderProgram.setMat4("view", view);
            shaderProgram.setMat4("projection", projection);

            // Draw the mesh
            drawMesh();
        }

        glfwSwapBuffers(window);
    }

    // Delete VAO and VBO
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // Terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


// Methods
void setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // Vertex normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);

    // Vertex texture coords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void drawMesh() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void processMesh(aiMesh* mesh, const aiScene* scene) {
    // Process vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        glm::vec3 vector;

        // Position
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;

        // Normal
        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.Normal = vector;

        // Texture coordinates
        if (mesh->mTextureCoords[0]) {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        } else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    // Process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Now create buffers
    setupMesh();
}

void processNode(aiNode* node, const aiScene* scene) {
    // Process each mesh at this node
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene);
    }

    // After processing all the meshes, recursively process children
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

// Function: renderQuad
// Renders a quad (rectangle) for rendering the background image
void renderQuad(float x, float y, float width, float height) {
    // Define the quad vertices with positions and texture coordinates
    float vertices[] = {
        // Positions        // Texture Coords
        x,          y + height,  0.0f, 1.0f,
        x,          y,           0.0f, 0.0f,
        x + width,  y,           1.0f, 0.0f,

        x,          y + height,  0.0f, 1.0f,
        x + width,  y,           1.0f, 0.0f,
        x + width,  y + height,  1.0f, 1.0f
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

// Function: renderLoadingScreen
// Function to render the loading screen with a background image
void renderLoadingScreen(unsigned int backgroundTexture) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Disable depth testing so 2D elements are rendered on top
    glDisable(GL_DEPTH_TEST);

    // Set up orthographic projection for 2D rendering
    glm::mat4 projection = glm::ortho(0.0f, 1024.0f, 0.0f, 768.0f);

    // Use the quad shader
    Shader quadShader("src/shaders/quad_shader.vert", "src/shaders/quad_shader.frag");
    quadShader.use();
    quadShader.setMat4("projection", projection);

    // Bind the background texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, backgroundTexture);
    quadShader.setInt("texture1", 0);

    // Render the quad for the background
    renderQuad(0.0f, 0.0f, 1024.0f, 768.0f); // Full-screen background quad

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);

    // Re-enable depth testing for 3D rendering
    glEnable(GL_DEPTH_TEST);
}

// Function: processMenuInput
// Handles inputs on the loading screen. Detects when the user clicks "Start" and changes to the game state
void processMenuInput(GLFWwindow* window) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        // Convert to OpenGL coordinate system if necessary
        ypos = 768.0 - ypos; // Invert Y coordinate

        // Check if the cursor is within the bounds of the "Start" button
        if (xpos >= 412.0 && xpos <= 612.0 && ypos >= 300.0 && ypos <= 380.0) {
            currentState = STATE_GAME;
        }
    }
}
