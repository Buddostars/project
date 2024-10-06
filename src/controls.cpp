// Include GLFW
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <iostream>

#include "controls.hpp"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;




// Initial position 
glm::vec3 position; 
glm::vec3 carPosition(0.0f, 0.0f, 0.0f);

// Initial horizontal angle : toward -Z
float distanceFromModel = 20.0f;
float horizontalAngle = 0.0f;
float verticalAngle = 0.0f;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.005f;
float initialFoV = 45.0f; // Initial Field of View

glm::vec3 modelPosition(0.0f, 0.0f, 0.0f); // Initial position of the model
glm::vec3 modelDirection(0.0f, 0.0f, -1.0f); // Forward direction (initially along -Z)

float modelSpeed = 10.0f; // Speed at which the model moves
float rotationSpeed = glm::radians(90.0f); // Speed at which the model rotates, in radians/sec
float modelAngle = 0.0f; // Current orientation of the model (rotation about Y axis)

glm::mat4 getViewMatrix(){
	return ViewMatrix;
}

glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}

void setThirdPersonView(const glm::vec3& cposition, const glm::vec3& direction){
	carPosition = cposition; 
	// Set the camera distance and angle=
	
    float distance = distanceFromModel;
    float verticalOffset = 15.0f; // height of the camera above the car
    
	// Calculate the new camera position
	glm::vec3 behindDirection = glm::normalize(glm::vec3(-direction.x, 0.0f, -direction.z));

    // Calculate new position using spherical coordinates
    position = carPosition - behindDirection * distance + glm::vec3(0.0f, verticalOffset, 0.0f);

	glm::vec3 lookAtPoint = carPosition + direction * 15.0f - glm::vec3(0.0f, 1.0f, 0.0f); // Add a small upward offset to the look at point

	ViewMatrix = glm::lookAt(
        position,          // Camera position (elevated and behind)
        lookAtPoint,       // Look at the cow's position
        glm::vec3(0.0f, 1.0f, 0.0f) // Up vector to keep the camera upright
    );
}

void computeMatricesFromInputs(GLFWwindow* window, float deltaTime){
	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse position for next frame
	glfwSetCursorPos(window, 1024/2, 768/2);

	// Compute new orientation
	horizontalAngle += mouseSpeed * float(1024/2 - xpos );
	verticalAngle   += mouseSpeed * float( 768/2 - ypos );

	// Limit the vertical angle to prevent flipping
    if (verticalAngle > glm::radians(89.0f))
        verticalAngle = glm::radians(89.0f);
    if (verticalAngle < glm::radians(-89.0f))
        verticalAngle = glm::radians(-89.0f);

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle), 
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);
	
	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f/2.0f), 
		0,
		cos(horizontalAngle - 3.14f/2.0f)
	);
	
	// Up vector
	glm::vec3 up = glm::cross( right, direction );

	// Move forward
	if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
		position += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
		position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS){
		position -= right * deltaTime * speed;
	}

	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
	// // Camera matrix
	// ViewMatrix       = glm::lookAt(
	// 							position,           // Camera is here
	// 							carPosition, 		// and looks here : at the same position
	// 							up      
	// 					   );

	
}
void updateModelPosition(GLFWwindow* window, float deltaTime) {
	
	// Handle left and right rotation
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        modelAngle += rotationSpeed * deltaTime; // Rotate left
		std::cout << "modelAngle: " << modelAngle << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        modelAngle -= rotationSpeed * deltaTime; // Rotate right
		std::cout << "modelAngle: " << modelAngle << std::endl;
    }

    // Update the model's direction vector based on the current angle
    modelDirection = glm::vec3(
        cos(modelAngle),
        0.0f,
        sin(modelAngle)
    );

    // Handle forward and backward movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        modelPosition += modelDirection * modelSpeed * deltaTime; // Move forward
		std::cout << "modelPosition: " << modelPosition.x << " " << modelPosition.y << " " << modelPosition.z << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        modelPosition -= modelDirection * modelSpeed * deltaTime; // Move backward
		std::cout << "modelPosition: " << modelPosition.x << " " << modelPosition.y << " " << modelPosition.z << std::endl;
    }
}