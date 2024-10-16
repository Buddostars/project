#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <unordered_set>
#include <filesystem>
#include <vector>
#include <cmath>
#include <random>
#include <thread>  // Include this for std::this_thread::sleep_for
#include <chrono>  // Include this for std::chrono::milliseconds

#include "shader.h"
#include "controls.hpp"
#include "ground.hpp"
#include "model.hpp"



std::vector<Vertex> vertices;
std::vector<unsigned int> indices;
unsigned int VAO, VBO, EBO;

struct Vec3Hash {
    std::size_t operator()(const glm::vec3& v) const {
        return std::hash<float>()(v.x) ^ std::hash<float>()(v.y) ^ std::hash<float>()(v.z);
    }
};

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




int main() {
    GLFWwindow* window = initializeWindow();
    if (!window) return -1;

    Shader shaderProgram("src/shaders/vertex_shader.vert", "src/shaders/fragment_shader.frag");
    Shader groundShader("src/shaders/ground_vertex_shader.vert", "src/shaders/ground_fragment_shader.frag");
    Model big_rock("src/models/big_rock.obj");
    Model small_rock("src/models/small_rock.obj");
    Model tree("src/models/tree.obj");
    Model ground("src/models/ground.obj");
    Camera camera;

    std::cout << "Current Working Directory: " << std::filesystem::current_path() << std::endl;
    if (!loadModel("src/models/cow.obj")) return -1;

    std::cout << "Loaded " << vertices.size() << " vertices and " << indices.size() << " indices." << std::endl;

    int treeCount = 100;
    std::vector<glm::vec3> treePositions = generateSpacedObjectPositions(treeCount, 90.0f, 10.0f);  // Range -90 to 90, at least 5 units apart

    int bigRockCount = 50;
    std::vector<glm::vec3> bigRockPositions = generateSpacedObjectPositions(bigRockCount, 90.0f, 10.0f);

    int smallRockCount = 35;
    std::vector<glm::vec3> smallRockPositions = generateSpacedObjectPositions(smallRockCount, 90.0f, 10.0f);



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

        // Draw the rocks
        for (const auto& position : smallRockPositions) {
            glm::mat4 smallRockkModel = glm::mat4(1.0f);
            smallRockkModel = glm::translate(smallRockkModel, position); // Use fixed position
            smallRockkModel = glm::scale(smallRockkModel, glm::vec3(0.5f, 0.5f, 0.5f)); // Scale trees if necessary
            
            shaderProgram.setMat4("model", smallRockkModel);
            shaderProgram.setMat4("view", view);
            shaderProgram.setMat4("projection", projection);
            big_rock.draw(shaderProgram); // Draw tree
        }

        for (const auto& position : bigRockPositions) {
            glm::mat4 bigRockkModel = glm::mat4(1.0f);
            bigRockkModel = glm::translate(bigRockkModel, position); // Use fixed position
            bigRockkModel = glm::scale(bigRockkModel, glm::vec3(1.5f, 1.5f, 1.5f)); // Scale trees if necessary
            
            shaderProgram.setMat4("model", bigRockkModel);
            shaderProgram.setMat4("view", view);
            shaderProgram.setMat4("projection", projection);
            big_rock.draw(shaderProgram); // Draw tree
        }

        // for (const auto& position : smallRockPositions) {
        //     glm::mat4  smallRockkModel = glm::mat4(1.0f);
        //     smallRockkModel = glm::translate(smallRockkModel, position); // Use fixed position
        //     smallRockkModel = glm::scale(smallRockkModel, glm::vec3(10.5f, 10.5f, 10.5f)); // Scale trees if necessary
            
        //     shaderProgram.setMat4("model", smallRockkModel);
        //     shaderProgram.setMat4("view", view);
        //     shaderProgram.setMat4("projection", projection);
        //     small_rock.draw(shaderProgram); // Draw tree
        // }

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

