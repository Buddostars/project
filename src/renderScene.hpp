#ifndef RENDER_SCENE_HPP
#define RENDER_SCENE_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <mutex>
#include <random>
#include "shader.hpp"
#include "model.hpp"

class RenderScene {
    public:
        RenderScene(Shader &shader, Model &model, glm::vec3 position, glm::vec3 direction, glm::vec3 velocity, float distanceTraveled, bool moving, bool isRotating, float rotationAngle, float targetRotationAngle, float rotationSpeed, float totalRotationAngle, float speed, float maxSpeed, float acceleration, float deceleration, float stopDuration, float timeStopped, float counter, std::mutex giraffeMutex, std::mt19937 rng, std::uniform_real_distribution<float> rotationDist);
        void runScene(glm::mat4 view, glm::mat4 projection);
    private:
        Shader shaderProgram;
        Shader objectShader;
        Shader objectShader2;
        Shader reflectionShader;
        Shader smokeShader;
        Model model;
        glm::vec3 position;
        glm::vec3 direction;
        glm::vec3 velocity;
        float distanceTraveled;
        bool moving;
        bool isRotating;
        float rotationAngle;
        float targetRotationAngle;
        float rotationSpeed;
        float totalRotationAngle;
        float speed;
        float maxSpeed;
        float acceleration;
        float deceleration;
        float stopDuration;
        float timeStopped;
        float counter;
        std::mutex giraffeMutex;
        std::mt19937 rng;
        std::uniform_real_distribution<float> rotationDist;
};

#endif // RENDER_SCENE_HPP