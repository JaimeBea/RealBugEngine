# Tesseract Game Engine

Basic engine created for UPC's Master in Advanced Programming for AAA Videogames.

![Photo of the Engine](https://i.imgur.com/ouVEiux.jpg)

## Features

- 3D Scene view with Unity-like controls
- Loading basic 3D models into the scene by dragging and dropping files
- Basic console log
- Loading and saving scenes
- Custom file format to save the information loaded
- Implementation of an editable Quadtree
- Frustum Culling
- PBR Phong implementation
- Three types of lights (directional, point, spot)
- Ray tracing (to select objects in the scene)

## Additional features

- Basic templated pool data structure to allow for faster allocation/deallocation of resources

## How to use

### Loading models and textures

- Drag model files inside any engine window to load it
- You can also load a different texture file for the model in the same manner. Select it clicking in the object, inside the Inspector Panel

### Menu items

- File
  - New: Create new empty scene
  - Load: Load a scene 
  - Save: Save a scene
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
- W/E/R to translate/rotate/scale selected object

Movement and zoom speed depend on the distance to the focus point (zooming out makes moving and zooming faster).

### Configuration panel

This panel contains engine information and configuration variables for multiple modules:

- Application
  - Application name and organization
- Time
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
- Scene
  - Draw Bounding Boxes
  - Draw Quadtree
  - Configure Quadtree
  - Draw Skybox
  - Change the background color
  - Change the ambient color
- Engine Camera
  - Frustum vector information (read-only)
  - Change near/far planes

### Scene panel
- Play/Stop/Pause controller
- Gizmos parameters
- Preview of the scene

### Inspector panel
Contains information about the currently loaded model

- Transformation (postition, rotation and scale)
- Geometry (meshes and bounding box)
- Textures (shader configuration, size and preview)
- Lights (lights parameters)
- Camera (camera parameters)

### Console panel

Prints all logged information and allows you to input commands.

There are no commands currently available.

## Libraries used

- SDL 2 (https://www.libsdl.org/download-2.0.php)
- MatGeoLib (https://github.com/juj/MathGeoLib)
- Dear ImGui (https://github.com/ocornut/imgui)
- DeviL (http://openil.sourceforge.net/)
- assimp (https://github.com/assimp/assimp)
- ImGuizmo (https://github.com/CedricGuillemet/ImGuizmo)
- RapidJason (https://github.com/Tencent/rapidjson)

## Authors

_Jaime Bea Pérez-Zubizarreta_ (https://github.com/JaimeBea)

_Lowy Solé Franquesa_ (http://github.com/lowysole)

_Jordi Gil Gonzalez_ (https://jordi-gil.github.io/)

_Cristian Ferrer Galan_ (https://github.com/Yonimevicio)

## Github

https://github.com/TBD-org/TBD-Engine

## License

[MIT](./LICENSE)

