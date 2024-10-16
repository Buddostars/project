#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "controls.hpp"
#include "model.hpp"
#include <filesystem>
#include <thread>  // Include this for std::this_thread::sleep_for
#include <chrono>  // Include this for std::chrono::milliseconds

std::vector<Vertex> vertices;
std::vector<unsigned int> indices;
unsigned int VAO, VBO, EBO;

// Function to initialize GLFW
GLFWwindow* initializeWindow() {
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1024, 768, "OpenGL", NULL, NULL);
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
    glViewport(0, 0, 1500, 1100);
    glEnable(GL_DEPTH_TEST); // Enable depth testing

    return window;
}

// Function to set up the mesh
void setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

// Function to draw the mesh
void drawMesh() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// Function to process mesh data
void processMesh(aiMesh* mesh, const aiScene* scene) {
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        glm::vec3 vector;

        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;

        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.Normal = vector;

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

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    setupMesh();
}

// Function to process node
void processNode(aiNode* node, const aiScene* scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

// Function to load model
bool loadModel(const std::string& path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return false;
    }

    processNode(scene->mRootNode, scene);
    return true;
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

// Function to initialize tree positions
std::vector<glm::vec3> generateObjectPositions(int count) {
    std::vector<glm::vec3> positions;
    for (int i = 0; i < count; ++i) {
        float xPos = static_cast<float>(rand() % 201 - 100); // Random x position between -100 and 100
        float zPos = static_cast<float>(rand() % 201 - 100); // Random z position between -100 and 100
        positions.emplace_back(xPos, 0.0f, zPos);
    }
    return positions;
}


int main() {
    GLFWwindow* window = initializeWindow();
    if (!window) return -1;

    Shader shaderProgram("src/shaders/vertex_shader.vert", "src/shaders/fragment_shader.frag");
    Shader groundShader("src/shaders/ground_vertex_shader.vert", "src/shaders/ground_fragment_shader.frag");
    Model big_rock("src/models/big_rock.obj");
    Model tree("src/models/tree.obj");
    Model ground("src/models/ground.obj");
    Camera camera;

    std::cout << "Current Working Directory: " << std::filesystem::current_path() << std::endl;
    if (!loadModel("src/models/cow.obj")) return -1;

    std::cout << "Loaded " << vertices.size() << " vertices and " << indices.size() << " indices." << std::endl;

    // Generate fixed tree positions
    int treeCount = 15; // Change this for more trees
    std::vector<glm::vec3> treePositions = generateObjectPositions(treeCount);

    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) {
        glClearColor(0.68f, 0.85f, 0.9f, 1.0f);  // Light blue color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderProgram.use();
        camera.computeMatricesFromInputs(window);

        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix();
        setLightingAndObjectProperties(shaderProgram);

        // Draw the cow model
        // glm::mat4 cowModel = glm::mat4(1.0f);
        // cowModel = glm::translate(cowModel, glm::vec3(0.0f, 0.0f, 0.0f)); // Position of cow
        // shaderProgram.setMat4("model", cowModel);
        // shaderProgram.setMat4("view", view);
        // shaderProgram.setMat4("projection", projection);
        // cow.draw(shaderProgram); // Draw cow

        glm::mat4 groundModel = glm::mat4(1.0f);
        groundModel = glm::translate(groundModel, glm::vec3(0.0f, 0.0f, 0.0f)); // Position of cow
        shaderProgram.setMat4("model", groundModel);
        shaderProgram.setMat4("view", view);
        shaderProgram.setMat4("projection", projection);
        ground.draw(shaderProgram); // Draw ground

        // Draw the tree model using fixed positions
        for (const auto& position : treePositions) {
            glm::mat4 treeModel = glm::mat4(1.0f);
            treeModel = glm::translate(treeModel, position); // Use fixed position
            treeModel = glm::scale(treeModel, glm::vec3(0.5f, 0.5f, 0.5f)); // Scale trees if necessary
            
            shaderProgram.setMat4("model", treeModel);
            shaderProgram.setMat4("view", view);
            shaderProgram.setMat4("projection", projection);
            tree.draw(shaderProgram); // Draw tree
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup (as before)
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

