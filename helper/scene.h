#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Scene
{
protected:
	glm::mat4 model, view, projection;

public:
    int width;
    int height;

	Scene() : m_animate(true), width(800), height(600) { }
	virtual ~Scene() {}

	void setDimensions( int w, int h ) {
	    width = w;
	    height = h;
	}
	
    /**
      Load textures, initialize shaders, etc.
      */
    virtual void initScene() = 0;

    /**
      This is called prior to every frame.  Use this
      to update your animation.
      */
    virtual void update( float t ) = 0;

    /**
      Draw your scene.
      */
    virtual void render() = 0;

    /**
      Called when screen is resized
      */
    virtual void resize(int, int) = 0;

    virtual void manageInput(GLFWwindow* myWindow) = 0;
    virtual void MouseCallback(GLFWwindow* window, double xpos, double ypos) = 0;
    
    void animate(GLFWwindow* myWindow, bool value ) { 
        if (value) {
            m_animate = value;
            glfwSetInputMode(myWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }

        else {
            m_animate = value;
            glfwSetInputMode(myWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
    bool animating() { return m_animate; }
    
protected:
	bool m_animate;
};
