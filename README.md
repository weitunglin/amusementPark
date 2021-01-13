# project 5
project for computer graphis

# How To Build ?
```
mkdir build
cd build
cmake ../.
make -j4
```

# How To Execute ?
```
cd ../bin/cg/
./cg__amusementPark
```

# User Manual
## Basic Control
### camera position
w -> go forward \
s -> go backward \
a -> go left \
d -> go right

### camera direction
up -> pull up camera direction \
down -> push down camera direction \
right -> move camera toward right \
left -> move camera toward left

### Exit
ESC -> quit the application

## Paramertic Rendering
1 ~ 5 -> set sphere subdivision level

# Tech Document
I use the skeleton by LearnOpenGL and some well-implemented classes like Shader and Model, instead of FLTK.

I implement a sphere using subdivion technique. The level of subdivision can be controlled by the user.

I render a skybox as a background.

I put a statue in front of the camera at the beginning. It is for the demonstration of billboard technique. Wherever you look at, the statue will face toward you.
The status is rendering under pipeline with geometry shader. It shows the effect of explotion.

# Reference
1. [LearnOpenGL](https://learnopengl.com/)
2. [SongHo](http://www.songho.ca/opengl/index.html)
