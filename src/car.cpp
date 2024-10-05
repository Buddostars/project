#include "car.hpp"
#include <iostream>




#include <glm/gtc/matrix_transform.hpp> // For transformations
// Assuming you have a struct to hold vertex data
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color; // Add color to the vertex structure
};

Car::Car() : position(glm::vec3(25.0f, 0.0f, 0.0f)), speed(10.0f) {
    // Load the car model
    loadModel("src/models/cow.obj");
    
}

void Car::update(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        position.z -= speed * 0.01f;  // Move forward along the Z-axis
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        position.z += speed * 0.01f;  // Move backward along the Z-axis
    }
}

void Car::loadModel(const std::string& path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
     if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }
    processNode(scene->mRootNode, scene);
    std::cout << "Loaded " << vertices.size() << " vertices and " << indices.size() << " indices." << std::endl;

}

void Car::processNode(aiNode* node, const aiScene* scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene);
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

void Car::processMesh(aiMesh* mesh, const aiScene* scene) {
    vertices.clear();
    indices.clear();

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        glm::vec3 vertex;
        vertex.x = mesh->mVertices[i].x;
        vertex.y = mesh->mVertices[i].y;
        vertex.z = mesh->mVertices[i].z;
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

void Car::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);


    glBindVertexArray(0);
}

void Car::render() {
    // Render the car model here
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    model = glm::scale(model, glm::vec3(0.1f));

    

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

}

glm::vec3 Car::getPosition() const {
    return position;
}
