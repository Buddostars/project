#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#define GLM_ENABLE_EXPERIMENTAL

#include <assimp/Importer.hpp>    // C++ importer interface 
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags


#include <glm/glm.hpp>                  // For math operations (OpenGL Mathematics)
#include <glm/gtc/matrix_transform.hpp> // For transformations
#include <glm/gtc/type_ptr.hpp>         // For converting matrices to OpenGL-compatible pointers
#include <glm/gtx/string_cast.hpp>      // For converting glm types to strings


#include "shader.h"
#include "controls.hpp"

#include "car.hpp"
#include "cow.hpp"

#include <filesystem>



int main(){
    // Initialize GLFW
    if (!glfwInit()) {
    std::cout << "Failed to initialize GLFW" << std::endl;
    return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a window
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Cows n Cars", NULL, NULL);
    if(window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    

    // Hide the mouse and enable unlimited movement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1280/2, 720/2);

    // add background color
    gladLoadGL();
    glViewport(0, 0, 1280, 720);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);  // Dark grey background
    glEnable(GL_DEPTH_TEST); // Enable depth testing

    // Create the shader program
    Shader shaderProgram("src/shaders/vertex_shader.vert", "src/shaders/fragment_shader.frag");
    
    // Load
   
    Car playerCar;
    Cow cow;

    //std::cout << "Current Working Directory: " << std::filesystem::current_path() << std::endl;
    // Load object with assimp
    // Assimp::Importer importer;
    // const aiScene* scene = importer.ReadFile("src/models/cow.obj", aiProcess_Triangulate | aiProcess_FlipUVs);
    // if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
    //     std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
    //     return -1;
    // }

    // processNode(scene->mRootNode, scene);
    // std::cout << "Loaded " << vertices.size() << " vertices and " << indices.size() << " indices." << std::endl;


	// Set light properties
    glm::vec3 lightPos(1.2f, 100.0f, 2.0f);
    
    // Set the directional light's direction pointing downwards
    glm::vec3 lightDirection = glm::normalize(glm::vec3(0.0f, -1.0f, -0.3f));
    
    // Set the light color (like sunlight)
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 0.9f);  // Slightly yellow
    
    // Set the object color
    glm::vec3 objectColor = glm::vec3(1.0f, 1.0f, 1.0f);  // Gray

    
    // Set camera position (viewer's position)
    glm::vec3 cameraOffset = glm::vec3(0.0f, 0.1f, -0.3f);

    // run if window is not closed and escape key is not pressed
    while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0){

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the screen

        

        // compute controlls
        // computeMatricesFromInputs(window);
        playerCar.update(window);

        //Camera following the car
        glm::vec3 carPosition = playerCar.getPosition();
        glm::vec3 cameraPosition = playerCar.getPosition() + cameraOffset;
        
        //Set the view amd projection matrices
        glm::mat4 view = glm::lookAt(cameraPosition, carPosition, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);
        std::cout << "Camera Position: " << glm::to_string(cameraPosition) << std::endl;
        
        // Activate the shader program and set uniforms
        shaderProgram.use();
        shaderProgram.setVec3("lightPos", lightPos);
        shaderProgram.setVec3("lightDirection", lightDirection);
        shaderProgram.setVec3("lightColor", lightColor);
        

        // Set model martix for the car
        glm::mat4 model = glm::translate(glm::mat4(1.0f), carPosition);
        shaderProgram.setMat4("model", model);
        shaderProgram.setMat4("view", view);
        shaderProgram.setMat4("projection", projection);
        std::cout << "Model Position: " << glm::to_string(glm::vec3(model[3])) << std::endl; // Outputs the translation part of the model matrix
        // std::cout << "Model Matrix: " << glm::to_string(model) << std::endl;
        // std::cout << "View Matrix: " << glm::to_string(view) << std::endl;
        // std::cout << "Projection Matrix: " << glm::to_string(projection) << std::endl;
        // Update and render the car
        
        playerCar.render();

        // Update the cow movement
        cow.update();
        cow.render();

        

        // Rotate the model
        // float timeValue = glfwGetTime();    // Get the current time (in seconds)
        // float angle = timeValue * glm::radians(50.0f);  // Rotate 50 degrees per second

        //model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));  // Rotate around the Y-axis
        

        // Draw the mesh
        //drawMesh();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    

    // Terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}