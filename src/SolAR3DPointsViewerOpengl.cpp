/**
 * @copyright Copyright (c) 2017 B-com http://www.b-com.com/
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "SolAR3DPointsViewerOpengl.h"

#include "xpcf/component/ComponentFactory.h"

#include <map>
#include <random>

namespace xpcf = org::bcom::xpcf;


XPCF_DEFINE_FACTORY_CREATE_INSTANCE(SolAR::MODULES::OPENGL::SolAR3DPointsViewerOpengl)

namespace SolAR {
using namespace datastructure;
namespace MODULES {
namespace OPENGL {

SolAR3DPointsViewerOpengl * SolAR3DPointsViewerOpengl::m_instance = NULL ;

SolAR3DPointsViewerOpengl::SolAR3DPointsViewerOpengl():ConfigurableBase(xpcf::toUUID<SolAR3DPointsViewerOpengl>())
{
    addInterface<api::display::I3DPointsViewer>(this);

    SRef<xpcf::IPropertyMap> params = getPropertyRootNode();

    params->wrapString("title",m_title);
    params->wrapUnsignedInteger("width", m_width);
    params->wrapUnsignedInteger("height", m_height);
    params->wrapUnsignedIntegerVector("backgroundColor", m_backgroundColor);
    params->wrapUnsignedIntegerVector("pointsColor", m_pointsColor);
    params->wrapUnsignedIntegerVector("cameraColor", m_cameraColor);
    params->wrapFloat("cameraScale", m_cameraScale);
    params->wrapFloatVector("position", m_position);
    params->wrapFloatVector("orientation", m_orientation);
    params->wrapInteger("exitKey", m_exitKey);
    params->wrapInteger("keyRight", m_keyRight);
    params->wrapInteger("keyLeft", m_keyLeft);
    params->wrapInteger("keyUp", m_keyUp);
    params->wrapInteger("keyDown", m_keyDown);

    m_instance = this ;

   LOG_DEBUG(" SolAR3DPointsViewerOpengl constructor");

}

SolAR3DPointsViewerOpengl::~SolAR3DPointsViewerOpengl()
{
    LOG_DEBUG(" SolAR3DPointsViewerOpengl destructor")
}

FrameworkReturnCode SolAR3DPointsViewerOpengl::display (const std::vector<SRef<CloudPoint>>& points, const Transform3Df & pose)
{
    m_points = points;
    m_cameraPose = pose;
    m_glcamera.resetview(math_vector_3f(m_position[0], m_position[1], m_position[2]), 1.0f);

    char *myargv [1];
    int myargc=1;
    myargv [0]=strdup (m_title.c_str());
    glutInit(&myargc, myargv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    m_resolutionX = m_width;
    m_resolutionY = m_height;

    glutInitWindowSize(m_width, m_height);

    glutCreateWindow(m_title.c_str());
    glutDisplayFunc(Render);
    glutKeyboardFunc(KeyBoard);
    glutMouseFunc(MouseState);
    glutMotionFunc(MouseMotion);
    glutReshapeFunc(ResizeWindow);
    glutIdleFunc(MainLoop);
    glutMainLoop();
    return FrameworkReturnCode::_SUCCESS;
}

void SolAR3DPointsViewerOpengl::OnMainLoop()
{
   // std::cout << "main loop "  << std::endl;
   //callBackIdle() ;
}

void SolAR3DPointsViewerOpengl::OnRender()
{
    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);

    m_glcamera.set_viewport(0, 0, m_resolutionX, m_resolutionY);
    m_glcamera.setup();
    m_glcamera.use_light(false);

    glClearColor(m_backgroundColor[0], m_backgroundColor[1], m_backgroundColor[2], 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);

//	DrawAxis();

    bool drawing = (!m_points.empty()) ;
    if(drawing)
    {
         glPushMatrix();
         glPointSize(2.25f);
         glBegin(GL_POINTS);
         for (unsigned int i = 0; i < m_points.size(); ++i) {
             glColor3f(m_pointsColor[0], m_pointsColor[1], m_pointsColor[2]);
             glVertex3f(m_points[i]->getX(), m_points[i]->getY(), m_points[i]->getZ());
         }
         glEnd();
         glPopMatrix();

         // draw  camera pose !
         glPushMatrix();
         std::vector<Vector4f> cameraPyramid;
         float offsetCorners = 0.075f * m_cameraScale;
         cameraPyramid.push_back(m_cameraPose * Vector4f(offsetCorners, offsetCorners, 2.0f*offsetCorners, 1.0f));
         cameraPyramid.push_back(m_cameraPose * Vector4f(-offsetCorners, offsetCorners, 2.0f*offsetCorners, 1.0f));
         cameraPyramid.push_back(m_cameraPose * Vector4f(-offsetCorners, -offsetCorners, 2.0f*offsetCorners, 1.0f));
         cameraPyramid.push_back(m_cameraPose * Vector4f(offsetCorners, -offsetCorners, 2.0f*offsetCorners, 1.0f));
         cameraPyramid.push_back(m_cameraPose * Vector4f(0, 0, 0, 1.0f));

         // draw a sphere at each corner of the frustum
         double cornerDiameter = 0.01f * m_cameraScale;
         glColor3f(m_cameraColor[0], m_cameraColor[1], m_cameraColor[2]);
         for (int i = 0; i < cameraPyramid.size(); ++i)
         {
             glPushMatrix();
             glTranslatef(cameraPyramid[i][0], cameraPyramid[i][1], cameraPyramid[i][2]);
             glutSolidSphere(cornerDiameter, 30, 30);
             glPopMatrix();
         }

         // draw frustum lines
         float line_width = 1.0f *m_cameraScale;
         glLineWidth(line_width);
         for (int i = 0; i < 4; ++i)
         {
             glBegin(GL_LINES);
             glVertex3f(cameraPyramid[4][0], cameraPyramid[4][1], cameraPyramid[4][2]);
             glVertex3f(cameraPyramid[i][0], cameraPyramid[i][1], cameraPyramid[i][2]);
             glEnd();
         }

         glBegin(GL_LINE_STRIP);
         glVertex3f(cameraPyramid[0][0], cameraPyramid[0][1], cameraPyramid[0][2]);
         glVertex3f(cameraPyramid[1][0], cameraPyramid[1][1], cameraPyramid[1][2]);
         glVertex3f(cameraPyramid[2][0], cameraPyramid[2][1], cameraPyramid[2][2]);
         glVertex3f(cameraPyramid[3][0], cameraPyramid[3][1], cameraPyramid[3][2]);
         glVertex3f(cameraPyramid[0][0], cameraPyramid[0][1], cameraPyramid[0][2]);
         glEnd();
         glPopMatrix();

    }
    glutSwapBuffers();
    glutPostRedisplay();
}


void SolAR3DPointsViewerOpengl::OnResizeWindow(int _w, int _h)
{
    m_resolutionX = _w;
    m_resolutionY = _h;
}

void SolAR3DPointsViewerOpengl::OnKeyBoard(unsigned char key, int x, int y)
{
   callbackKeyBoard(key) ;
}


void SolAR3DPointsViewerOpengl::OnMouseMotion(int x, int y)
{
    y = m_resolutionY - y;
    m_glcamera.mouse_move(x, y);

}
void SolAR3DPointsViewerOpengl::OnMouseState(int button, int state, int x, int y)
{
    y = m_resolutionY - y;
    int zoom = 10;
    Mouse::button b = Mouse::NONE;

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

        b = Mouse::ROTATE;
        m_glcamera.mouse(x, y, b);

    }
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {

        b = Mouse::MOVEXY;
        m_glcamera.mouse(x, y, b);
    }
    else if ((button & 3) == 3) {

        m_glcamera.mouse_wheel(zoom);
    }
    else if ((button & 4) == 4) {

        m_glcamera.mouse_wheel(-zoom);
    }
}

}
}
}
