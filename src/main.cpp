#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include <assimp/Importer.hpp>    // C++ importer interface 
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags


#include <glm/glm.hpp>                  // For math operations (OpenGL Mathematics)
#include <glm/gtc/matrix_transform.hpp> // For transformations
#include <glm/gtc/type_ptr.hpp>         // For converting matrices to OpenGL-compatible pointers

#include "shader.h"
#include "controls.hpp"
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
    GLFWwindow* window = glfwCreateWindow(1024, 768, "OpenGL", NULL, NULL);
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
    glfwSetCursorPos(window, 1024/2, 768/2);

    // add background color
    gladLoadGL();
    glViewport(0, 0, 1500, 1100);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);  // Dark grey background
    glEnable(GL_DEPTH_TEST); // Enable depth testing

    // Create the shader program
    Shader shaderProgram("src/shaders/vertex_shader.vert", "src/shaders/fragment_shader.frag");
    

    std::cout << "Current Working Directory: " << std::filesystem::current_path() << std::endl;
    // Load object with assimp
    loadCowModel("src/models/cow.obj");



	// Set light properties
    glm::vec3 lightPos(1.2f, 100.0f, 2.0f);
    
    // Set the directional light's direction pointing downwards
    glm::vec3 lightDirection = glm::normalize(glm::vec3(0.0f, -1.0f, -0.3f));
    
    // Set the light color (like sunlight)
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 0.9f);  // Slightly yellow
    
    // Set the object color
    glm::vec3 objectColor = glm::vec3(0.1f, 0.1f, 0.1f);  // Gray

    
    // Set camera position (viewer's position)
    glm::vec3 viewPos(0.0f, 40.0f, 3.0f);

    // run if window is not closed and escape key is not pressed
    while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0){

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the screen

        // Activate the shader program
        shaderProgram.use();

        // compute controlls
        computeMatricesFromInputs(window);

        // Set lighting uniforms
        shaderProgram.setVec3("lightPos", lightPos);
        shaderProgram.setVec3("lightDirection", lightDirection);
        shaderProgram.setVec3("lightColor", lightColor);
        shaderProgram.setVec3("viewPos", viewPos);
        shaderProgram.setVec3("objectColor", objectColor);

        // Set any uniform variables (like model, view, projection matrices)
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = getViewMatrix();
        glm::mat4 projection = getProjectionMatrix();

        // Rotate the model
        float timeValue = glfwGetTime();    // Get the current time (in seconds)
        float angle = timeValue * glm::radians(50.0f);  // Rotate 50 degrees per second

        //model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));  // Rotate around the Y-axis
        
        // Set the model, view, and projection matrices
         shaderProgram.setMat4("model", model);
         shaderProgram.setMat4("view", view);
         shaderProgram.setMat4("projection", projection);

        // Draw the mesh
        drawCow();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    // Terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}