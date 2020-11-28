# RealEngine

Basic engine created for UPC's Master in Advanced Programming for AAA Videogames.

## Features

- 3D Scene view with Unity-like controls
- Loading basic 3D models into the scene by dragging and dropping files
- Basic console log
- Basic configuration and properties panels

## Additional features

- Basic templated pool data structure to allow for faster allocation/deallocation of resources
- Rendering to framebuffers to show the scene inside an ImGui window.

## How to use

### Loading models and textures

- Drag model files inside any engine window to load it and replace the current geometry
- You can also load a different texture file for the model in the same manner

### Menu items

- File
  - Quit: Exit the engine
- View
  - Open/Close different panels from here
- Help
  - Documentation: Go to wiki page in github
  - Download latest: Go to releases page in github
  - Report a bug: Go to issues page in github
  - About: Open/Close about panel

### Scene view navigation

Left click to focus on the scene window first.

- Right-click to enter flythrough mode
  - WASD + QE to move around
  - Mouse to look around
- Alt + Left-click to orbit around the camera focus
- Alt + Right-click to zoom with the mouse (moves the camera focus)
- Mouse wheel to zoom (moves the camera focus)
- Arrow keys to move around
- F key to focus around the geometry
- Shift to move faster (5x)

Movement and zoom speed depend on the distance to the focus point (zooming out makes moving and zooming faster).

### Configuration panel

This panel contains engine information and configuration variables for multiple modules:

- Application
  - Application name and organization
  - FPS and MS graph
  - Change Framerate Limit
  - Activate/Deactivate Framerate Limit
  - Activate/Deactivate VSync
- Hardware
  - Information about library versions
  - Information about CPU, RAM and GPU
- Window
  - Change window mode
    - Windowed
    - Borderless
    - Fullscreen (Not working properly)
    - Desktop Fullscreen
  - Change brightness
  - Resize window
- Camera
  - Frustum vector information (read-only)
  - Change camera position
  - Change camera movement/rotation/zoom speed
  - Change the shift key speed multiplier
  - Change near/far planes
  - Change the vertical FOV
  - Change the background color
- Textures
  - Change min/mag filters (applies to all textures)
  - Change texture wrapping (applies to all textures)
  - Information about loaded textures (size and preview)
- Models
  - Information about loaded models
    - Meshes (vertices, triangles and material)
    - Materials (texture information)
    - Information about the calculated bounding sphere

### Properties panel

Contains information about the currently loaded model

- Transformation (postition, rotation and scale)
- Geometry (meshes and bounding sphere)
- Textures (size and preview)

### Console panel

Prints all logged information and allows you to input commands.

There are no commands currently available.

## Libraries used

- SDL 2 (https://www.libsdl.org/download-2.0.php)
- MatGeoLib (https://github.com/juj/MathGeoLib)
- Dear ImGui (https://github.com/ocornut/imgui)
- DeviL (http://openil.sourceforge.net/)
- assimp (https://github.com/assimp/assimp)

## Authors

_Jaime Bea Pérez-Zubizarreta_ (https://github.com/JaimeBea)

## Github

https://github.com/JaimeBea/UPCMasterEngine

## License

[MIT](./LICENSE)
