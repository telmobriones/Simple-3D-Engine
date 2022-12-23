# Simple-3D-Engine
This project is a basic 3D engine that uses scene graph data structures and the OpenGL pipeline. It includes a number of features such as frustum culling, bump mapping, specular mapping, cube mapping and multitexturing. The engine also supports different types of lights (local, directional, and spotlight) that are calculated using the Blinn–Phong reflection model, as well as lighting provided by the Cook-Torrance BRDF microfacet model. All of these features are implemented as shaders in the shader folder.

# How to run it

1) You need the following packages for installing this:
- freeglut3-dev
- libglew-dev

The following command would do in on an Ubuntu system:

% sudo apt-get install freeglut3-dev libglew-dev

2) Build the project using CMAKE

Run this *only once*

% cmake CMakeLists.txt

Then, build tha project with:

% make

3) Run the project:

% ./browser

4) Sometimes you have to uncomment this line in Browser/browser.c (around line
   446):

  // Uncomment following line if you have problems initiating GLEW
  //
-  // glewExperimental = GL_TRUE;
+  glewExperimental = GL_TRUE;


# Controls

    ESC: exit the program
    Arrow keys: move the camera
    Page Up/Page Down: advance/go back
    Home: return to the default location
    a: rotate object left (around Y axis)
    d: rotate object right (around Y axis)
    d: rotate object down (around X axis)
    x: move object up (along X axis)
    q/Q: spin object left/right
    i/I: move object along local X axis (negative X direction)
    o/O: move object along local Y axis (negative Y direction)
    p/P: move object along local Z axis (negative Z direction)
    f: increase fovy (zoom in)
    F: decrease fovy (zoom out)
    l: set lights
    L: disable lights
    s: enable shading
    S: disable shading
    z: enable z-buffer
    Z: disable z-buffer
    .: toggle between walk and fly modes
    0: animate
    1 through 7: switch light on/off
    Alt + c: print camera information
    Alt + i: print registered images
    Alt + l: print registered lights
    Alt + m: print registered materials
    Alt + t: print registered textures
    Alt + p: print projection transformation
    Alt + s: print render state
    Alt + S: print shaders
    Alt + v: print modelview transformation
    Alt + b: draw bounding boxes
    Alt + a: toggle line aliasing on/off
    Alt + C: go through objects
    Alt + f: toggle between culling and unculling the camera
    Alt + 0: take a snapshot
    Alt + 1: go to parent node
    Alt + 2: go to first child node
    Alt + 3: go to next sibling node

