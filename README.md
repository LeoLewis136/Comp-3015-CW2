# Comp-3015-CW2

## Using the project
To run the project get the .ex and place the media folder in the same folder.
Double click the exe to run the project.
### Controls
- W & S to rotate up or down (Pitch)
- A & D to turn Left and Right (Yaw)
- Up arrow & Down arrow to move forwards or backwards
- Left arrow & Right arrow to rotate the ship (Roll)
- E to shoot
- Esc to exit the game

## General program flow
The project follows standard game loop design with the game looping forever until the user presses esc to exit.
Within this loop there is an update() and render() funtion. Update() causes all game objects such as the asteroids, the player projectiles or the player to run their update code and change their state. Render() then manages visualising all the obejcts on the screen also managing the flow of multi pass rendering to make bloom possible. While my gameloop doesn't have an explicit event() function all the player input events are managed at the start of each frame in a separate function within the scene object. These inputs are then passed to the player for the player update function.

## Shader program
My program has bloom and particles implemented. The bloom effect is managed within the basic_uniform fragment and vertex shader. This shader maanges the 5 passes that are required for my specifc bloom implementation. The 5 passes within my bloom implementation are: Render the scene as normal, Find the bright spots in the scene that need bloom and draw these to a texture, Blur the bloom texture horizontally, Blur the bloom texture vetically, Mix the bloom texture and whole scene texture for the final image. 

The particle effects are managed within the particle fragment and vertex shader. This shader manages the first pass of rendering a particle system within my game as the first pass is always to render the scene to a temporary texture so any alternate shaders from the basic_uniform such as particle or skybox can work as normal. The particle implementation I have gone with is to have a shader that calculates particle positions based on time, camera position and particle infomation. These specific particles can then be rendered.

## What was used to make the project
The project was made and tested on Windows 10 using Visual Studio 2022

## Links
[Video link](https://youtu.be/W6pTQ9zHx7s)

[Github Link](https://github.com/LeoLewis136/Comp-3015-CW2)
