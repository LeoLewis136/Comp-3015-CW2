#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include <vector>

// Premade for this module includes
#include "helper/torus.h"
#include "helper/scene.h"
#include "helper/objmesh.h" // Model loader
#include "helper/random.h"
#include "helper/grid.h"
#include "helper/particleutils.h"

#include <glm/glm.hpp>

#include <glad/glad.h>
#include "helper/glslprogram.h"
#include "helper/cube.h"
#include "helper/skybox.h"
#include "helper/plane.h"
#include <GLFW/glfw3.h>

#include "Player.h"
#include "Projectile.h"
#include "Bullet.h"


using namespace glm;

class SceneBasic_Uniform : public Scene
{
private:
    // Camera variables
    vec3 cameraPosition = vec3(0.0f, 0.0f, 0.0f);
    vec3 cameraForward = vec3(0.0f, 0.0f, 1.0f);
    vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);
    float yaw = 0.0f;
    float pitch = 0.0f;
    bool mouseEntered = false;

    bool EKeyPressed = false;

    vec2 lastMousePos = vec2(0.0f);

    vec3 movementVec = vec3(0.0f); // The current movement vector
    float lastFrame = 0.0f; // To manage deltaTime;

    GLSLProgram standardShaders;
    GLSLProgram skyboxShaders;
    GLSLProgram particleShaders;
    GLSLProgram flatShaders;

    SkyBox sky;
    Plane water;

    // -- Particles --
    Random rand;
    GLuint initVel, startTime, particles, nParticles;
    Grid grid;
    vec3 emitterPos, emitDir;
    float particleLifetime, totalTime;

    // -- Textures -- 
    GLuint skyboxTexture; // Skybox
    GLuint plainTexture; // Player Texture
    GLuint playerTexture; // Player Object Texture

    GLuint particleTexture; // The texture to be used on particles

    // -- Objects --
    Player playerObject;
    std::vector<Projectile> projectileArray;
    std::vector<Bullet> bulletArray;

    float tempAngle = 0.0f;
    float debug_time = 0.0f;

    int currentAsteroidNum = 10;
    int asteroidsSpawned = 0;
    int score = 0; // The player's current score
    bool game_exit = false;


    GLuint lightTex, renderTex, verticalTex, horizontalTex, fsQuad, lightFBO, verticalFBO, horizontalFBO, renderFBO; // Multi-Pass rendering GLuints

    void setMatricies(GLSLProgram& currentShaders);

    void compile();
    void assignCubemap(GLuint albedo);
    void assignTexture(GLuint albedo);
    void assignMaterial(float shininess);

    void positionModel(glm::vec3 newPosition);
    void rotateModel(float rotation, vec3 axis);
    void rotateModel(mat4 orientation);
    void setScale(float scale);

    // Particle Rendering
    void initParticleBuffer();
    float randFloat();

    // Multi-Pass rendering
    void setupFBO();
    void pass1();
    void pass2();
    void pass3();
    void pass4();
    void pass5();

    float gauss(float x, float y);


public:
    SceneBasic_Uniform();

    void manageInput(GLFWwindow* myWindow);
    void MouseCallback(GLFWwindow* window, double xpos, double ypos);

    void initScene();
    void update(float t);
    void render();
    void resize(int, int);

    void manageAsteroids();
};

#endif // SCENEBASIC_UNIFORM_H
