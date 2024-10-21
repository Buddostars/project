#include "renderScene.hpp"

RenderScene(Shader &shaderProgram, Shader &objectShader, Shader &objectShader2, Shader &reflectionShader, Shader &smokeShader, Model &groundModel, glm::vec3 position, glm::vec3 direction, glm::vec3 velocity, float distanceTraveled, bool moving, bool isRotating, float rotationAngle, float targetRotationAngle, float rotationSpeed, float totalRotationAngle, float speed, float maxSpeed, float acceleration, float deceleration, float stopDuration, float timeStopped, float counter, std::mutex giraffeMutex, std::mt19937 rng, std::uniform_real_distribution<float> rotationDist){
    this->shaderProgram = shaderProgram;
    this->objectShader = objectShader;
    this->objectShader2 = objectShader2;
    this->reflectionShader = reflectionShader;
    this->smokeShader = smokeShader;
    this->model = model;
}

void justRender(glm::mat4 view, glm::mat4 projection){
    shaderProgram.use();
    objectShader.use();

    setLightingAndObjectProperties(shaderProgram);
    setLightingAndObjectProperties(objectShader);

    // Draw the car model   
    car.draw(objectShader)
    // Draw the ground model
    glm::mat4 groundModel = glm::mat4(1.0f);
    groundModel = glm::translate(groundModel, glm::vec3(0.0f, 0.0f, 0.0f)); // Position of ground
    objectShader.setMat4("model", groundModel);
    objectShader.setMat4("view", view);
    objectShader.setMat4("projection", projection);
    ground.draw(objectShader); // Draw ground

    // Pass the updated matrices to the shader
    objectShader.setMat4("model", cowModelMatrix);
    objectShader.setMat4("view", view);
    objectShader.setMat4("projection", projection);

    // Render the cow
    cow.draw(objectShader);

    objectShader2.use();
    // Render the giraffes after movement updates
    for (auto& giraffe : giraffes) {
        glm::mat4 giraffeModelMatrix = glm::mat4(1.0f);
        giraffeModelMatrix = glm::translate(giraffeModelMatrix, giraffe.getPosition());
        giraffeModelMatrix = glm::rotate(giraffeModelMatrix, glm::radians(giraffe.getTotalRotationAngle()), glm::vec3(0.0f, 1.0f, 0.0f));
        giraffeModelMatrix = glm::scale(giraffeModelMatrix, glm::vec3(0.2f, 0.2f, 0.2f))
        objectShader2.setMat4("model", giraffeModelMatrix);
        objectShader2.setMat4("view", view);
        objectShader2.setMat4("projection", projection);
        giraffe.draw(objectShader2);
    }

    reflectionShader.use();
            setLightingAndObjectProperties(reflectionShader);

    // Draw the rocks
    for (const auto& position : smallRockPositions) {
        Hitbox smallRockHitBox = small_rock.calculateHitbox();
        smallRockHitBox.minCorner += position;
        smallRockHitBox.maxCorner += position;
        // Check if this hitbox already exists in environmentHitboxes
        bool isUnique = std::none_of(environmentHitboxes.begin(), environmentHitboxes.end(),
                                    [&](const Hitbox& hitbox) {
                                        return hitbox == smallRockHitBox;
                                    })
        if (isUnique) {
            environmentHitboxes.push_back(smallRockHitBox);
        
        glm::mat4 smallRockkModel = glm::mat4(1.0f);
        smallRockkModel = glm::translate(smallRockkModel, position); // Use fixed position
        smallRockkModel = glm::scale(smallRockkModel, glm::vec3(3.5f, 3.5f, 3.5f)); // Scale trees if necessary
        
        reflectionShader.setMat4("model", smallRockkModel);
        reflectionShader.setMat4("view", view);
        reflectionShader.setMat4("projection", projection);
        reflectionShader.setVec3("cameraPos", camera.position);
        small_rock.draw(reflectionShader, cubemap.getTextureID()); // Draw small rocks
    
    for (const auto& position : bigRockPositions) {
        Hitbox bigRockHitBox = big_rock.calculateHitbox();
        bigRockHitBox.minCorner += position;
        bigRockHitBox.maxCorner += position;
        // Check if this hitbox already exists in environmentHitboxes
        bool isUnique = std::none_of(environmentHitboxes.begin(), environmentHitboxes.end(),
                                    [&](const Hitbox& hitbox) {
                                        return hitbox == bigRockHitBox;
                                    })
        if (isUnique) {
            environmentHitboxes.push_back(bigRockHitBox);
        
        glm::mat4 bigRockkModel = glm::mat4(1.0f);
        bigRockkModel = glm::translate(bigRockkModel, position); // Use fixed position
        bigRockkModel = glm::scale(bigRockkModel, glm::vec3(1.5f, 1.5f, 1.5f)); // Scale trees if necessary
        
        reflectionShader.setMat4("model", bigRockkModel);
        reflectionShader.setMat4("view", view);
        reflectionShader.setMat4("projection", projection);
        reflectionShader.setVec3("cameraPos", camera.position);
        big_rock.draw(reflectionShader, cubemap.getTextureID()); // Draw big rocks
    }

    // Render smoke particles
    exhaustSystem.render(smokeShader, view, projection);

}