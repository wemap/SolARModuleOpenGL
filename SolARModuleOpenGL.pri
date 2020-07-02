HEADERS += interfaces/SolAROpenglAPI.h \
    interfaces/SolARModuleOpengl_traits.h \
    interfaces/SolAR3DPointsViewerOpengl.h \
    src/glcamera/common.hpp \
    src/glcamera/gl_camera.hpp \
    src/glcamera/math.hpp \
    src/glcamera/matrix.hpp \
    src/glcamera/matrix_fixed.hpp \
    src/glcamera/rigid_motion.hpp \
    src/glcamera/trackball.hpp \
    src/glcamera/vector.hpp \
    src/glcamera/vector_fixed.hpp \
    interfaces/SolARSinkPoseTextureBufferOpengl.h

SOURCES += src/SolARModuleOpengl.cpp \
    src/SolAR3DPointsViewerOpengl.cpp \
    src/glcamera/gl_camera.cpp \
    src/SolARSinkPoseTextureBufferOpengl.cpp
