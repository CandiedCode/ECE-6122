
# Homework 3: 3D World Scene Rendering with OpenGL and Assimp

## Overview

This project loads a variety of models using Assimp library. Phong lighting has been
applied via [object.frag](./assets/shaders/object.frag). You can move the camera around
the scene using W|A|S|D keys or arrow keys. Increase camera movement speed using the
trackpad (two fingers) or scroll wheel, or +/- on your keyboard. Press Q or Escape to
quit.

## Models Used

The 5 objects used in the scene include:

- [farmhouse](./assets/models/farmhouse/) - includes .obj/.mtl files and texture images
- [farmer](./assets/models/farmer/) - uses diffuse colors from .mtl file (no texture images)
- [barrel](./assets/models/barrel/) - includes texture images loaded via .obj/.mtl files
- [horse](./assets/models/Horse_Lores.obj) - uses white fallback texture (no colors in model)
- [robot](./assets/models/robot/) - updated Robot.mtl to reference Chrome.jpg instead of
  Chrome.png

For the ground plane, alternating green stripes simulate grass.

## Build

Running `make build/release` creates an executable in `../output/bin/Hmk3`. The
application must be run from the executable directory since asset paths are relative to it.

This has been tested on both my mac and pace ice.

## Video Demo

I build and ran my application from scratch on both my mac and pace ice
https://youtu.be/7wKtyRPxxnY

If you just want to see the application running you can jump to
https://youtu.be/7wKtyRPxxnY?t=76
