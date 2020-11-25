# RealEngine

Basic engine created for UPC's Master in Advanced Programming for AAA Videogames.

## Features

- 3D Scene view with Unity-like controls
- Loading basic 3D models into the scene by dragging and dropping files
- Basic console log
- Basic configuration and properties panels

## Scene view navigation

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

## Libraries used

- SDL 2 (https://www.libsdl.org/download-2.0.php)
- MatGeoLib (https://github.com/juj/MathGeoLib)
- Dear ImGui (https://github.com/ocornut/imgui)
- DeviL (http://openil.sourceforge.net/)
- assimp (https://github.com/assimp/assimp)

## Authors

_Jaime Bea Pérez-Zubizarreta_

## Github

https://github.com/JaimeBea/UPCMasterEngine

## License

[MIT](./LICENSE)
