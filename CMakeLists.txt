cmake_minimum_required(VERSION 3.7.2)

##################################################
set (VERSION_NUMBER "0.4.0")
project("SolARModuleOpenGL")
set (SOURCES 
	src/SolARModuleOpengl.cpp 
    src/SolAR3DOverlayPointsOpengl.cpp 
)

set (HEADERS
    interfaces/SolAROpenglAPI.h
    interfaces/SolAR3DOverlayPointsOpengl.h
    interfaces/SolARModuleOpengl_traits.h
)

install (FILES "${CMAKE_CURRENT_SOURCE_DIR}/xpcf_SolARModuleOpenGL_registry.xml" DESTINATION $ENV{BCOMDEVROOT}/.xpcf/SolAR/	)

##################################################

# various macros
include("$ENV{BCOMDEVROOT}/bcomBuild/SolARFramework/solarmacros.cmake")
# config setup
setup()
# process packagedependencies.txt
processPackagedependencies()
# define targets (type library)
defineTargets("library" "")


