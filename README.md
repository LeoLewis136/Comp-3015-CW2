# Comp-3015-CW1
 
## What was used to make the project
The project was made and tested on Windows 10 using Visual Studio 2022
   

## The different parts of the program
### Scene creation
- SceneBasic_Uniform constructor
- initScene()
  
These functions are for scene setup they load assets such as models or textues, setup any static uniforms for the shaders, setup Fbo's required for multi-pass rendering and other miscellaneous setup such as assigning object positions

### Input
- manageInput()
- MouseCallback()

These functions manage mouse and keyboard input to allow for camera moevement. manageInput() takes keyboard inputs and modifies the position of the camera accordingly. MouseCallback() recieves the mouse callback event that is passed through the program and uses the information recieved to rotate the camera.

### Rendering
- render()
- pass1()
- pass2()
- pass3()

These functions are used to render the scene. The render function simply calls pass1, 2 and 3 in order. Pass 1 renders all the scene geometry to a custom FBO and texture, Pass 2 applies vertical gaussian blur to the scene then renders to another FBO and texture. Finally, Pass 3 applies horizontal gaussian blur then renders to the default FBO to be shown on the screen.

### Shader Assignments
- assignCubemap()
- assignTexture()
- assignMaterial()

These functions pass values to the shaders to allow objects to have different textures, objects to have different material values and to allow the cubemap tetures to be assigned for skybox rendering.

### Model Management
- positionModel()
- rotateModel()
- setScale()

These functions allow modifying the model matix to position different objects at different places, rotate models around specific axis, and set the scale of an object to scale it up or down 
