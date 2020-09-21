# SolARModuleOpenGL

The **SolARModuleOpenGL** module is based on [OpenGL](https://www.opengl.org/) (Open Graphics Library) which is a cross-language, cross-platform application programming interface (API) for rendering 2D and 3D vector graphics. The API is typically used to interact with a graphics processing unit (GPU), to achieve hardware-accelerated rendering.

The OpenGL specification describes an abstract API for drawing 2D and 3D graphics. Although it is possible for the API to be implemented entirely in software, it is designed to be implemented mostly or entirely in hardware. 

OpenGL is is a Free Software [License B](https://directory.fsf.org/wiki/License:SGI-B-2.0).

SolARModuleOpenGL also uses the [freeglut](http://freeglut.sourceforge.net/) library, which is a free-software/open-source alternative to the OpenGL Utility Toolkit (**GLUT**) library. freeglut is released under the [MIT license](https://opensource.org/licenses/MIT).

GLUT (and hence freeglut) takes care of all the system-specific chores required for creating windows, initializing OpenGL contexts, and handling input events, to allow for trully portable OpenGL programs.