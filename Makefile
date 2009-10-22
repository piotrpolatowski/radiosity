override SOURCEROOT	= ../..
PROGRAMS 		= radiosity
OWN_LIBS 		= X3DSceneGraph Geometry Graphics Components CGLA
LIBS			= ${GLUTLIBS} ${GLLIBS} ${WIN_SYS_LIBS} -lm -lexpat
CXXFLAGS = -DSOLUTION
include ${SOURCEROOT}/makefiles/application.mk
