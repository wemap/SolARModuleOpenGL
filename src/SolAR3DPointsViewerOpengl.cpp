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
#include <math.h>
#include <random>

namespace xpcf = org::bcom::xpcf;


XPCF_DEFINE_FACTORY_CREATE_INSTANCE(SolAR::MODULES::OPENGL::SolAR3DPointsViewerOpengl)

namespace SolAR {
using namespace datastructure;
namespace MODULES {
namespace OPENGL {

static Transform3Df SolAR2GL = [] {
  Matrix<float, 4, 4> matrix;
  matrix << 1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0;
  return Transform3Df(matrix);
}();

SolAR3DPointsViewerOpengl * SolAR3DPointsViewerOpengl::m_instance = NULL ;

SolAR3DPointsViewerOpengl::SolAR3DPointsViewerOpengl():ConfigurableBase(xpcf::toUUID<SolAR3DPointsViewerOpengl>())
{
    addInterface<api::display::I3DPointsViewer>(this);

    SRef<xpcf::IPropertyMap> params = getPropertyRootNode();

    params->wrapString("title",m_title);
    params->wrapUnsignedInteger("width", m_width);
    params->wrapUnsignedInteger("height", m_height);
    params->wrapUnsignedIntegerVector("backgroundColor", m_backgroundColor);
    params->wrapUnsignedInteger("fixedPointsColor", m_fixedPointsColor);
    params->wrapUnsignedIntegerVector("pointsColor", m_pointsColor);
    params->wrapUnsignedIntegerVector("cameraColor", m_cameraColor);
    params->wrapUnsignedInteger("keyframeAsCamera", m_keyframeAsCamera);
    params->wrapUnsignedIntegerVector("framesColor", m_framesColor);
    params->wrapUnsignedIntegerVector("keyframesColor", m_keyframesColor);
    params->wrapUnsignedInteger("drawCameraAxis", m_drawCameraAxis);
    params->wrapUnsignedInteger("drawSceneAxis", m_drawSceneAxis);
    params->wrapUnsignedInteger("drawWorldAxis", m_drawWorldAxis);
    params->wrapFloat("axisScale", m_axisScale);
    params->wrapFloat("pointSize", m_pointSize);
    params->wrapFloat("cameraScale", m_cameraScale);
    params->wrapFloat("zoomSensitivity", m_zoomSensitivity);
    params->wrapInteger("exitKey", m_exitKey);
    m_instance = this ;

   LOG_DEBUG(" SolAR3DPointsViewerOpengl constructor");

}

SolAR3DPointsViewerOpengl::~SolAR3DPointsViewerOpengl()
{
    LOG_DEBUG(" SolAR3DPointsViewerOpengl destructor")
}

xpcf::XPCFErrorCode SolAR3DPointsViewerOpengl::onConfigured()
{
    LOG_DEBUG(" SolAR3DPointsViewerOpengl onConfigured");
    char *myargv [1];
    int myargc=1;
    myargv [0]=strdup (m_title.c_str());
    glutInit(&myargc, myargv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    m_resolutionX = m_width;
    m_resolutionY = m_height;

    glutInitWindowSize(m_width, m_height);

    return xpcf::_SUCCESS;
}



void SolAR3DPointsViewerOpengl::drawFrustumCamera(Transform3Df&cPose,
                                           std::vector<float>& cColor,
                                           float cFactor,
                                           float oFactor,
                                           float lFactor){

 //   Transform3Df cameraInv = camPose.inverse();
    // draw  camera pose !
    std::vector<Vector4f> cameraPyramid;
    float offsetCorners =  cFactor * m_cameraScale * m_sceneSize; // cornerScale = 0.033
    Transform3Df cameraPoseGL = SolAR2GL * cPose;
    cameraPyramid.push_back(cameraPoseGL * Vector4f(offsetCorners, offsetCorners, 2.0f*offsetCorners, 1.0f));
    cameraPyramid.push_back(cameraPoseGL * Vector4f(-offsetCorners, offsetCorners, 2.0f*offsetCorners, 1.0f));
    cameraPyramid.push_back(cameraPoseGL * Vector4f(-offsetCorners, -offsetCorners, 2.0f*offsetCorners, 1.0f));
    cameraPyramid.push_back(cameraPoseGL * Vector4f(offsetCorners, -offsetCorners, 2.0f*offsetCorners, 1.0f));
    cameraPyramid.push_back(cameraPoseGL * Vector4f(0, 0, 0, 1.0f));
    cameraPyramid.push_back(cameraPoseGL * Vector4f(3.0f * offsetCorners, 0, 0, 1.0f));
    cameraPyramid.push_back(cameraPoseGL * Vector4f(0, 3.0f * offsetCorners, 0, 1.0f));
    cameraPyramid.push_back(cameraPoseGL * Vector4f(0, 0, 3.0f * offsetCorners, 1.0f));

    // draw a sphere at each corner of the frustum
    double cornerDiameter = oFactor * m_cameraScale * m_sceneSize; //offsetCornerScale = 0.004
    glColor3f(cColor[0], cColor[1], cColor[2]);
    for (int i = 0; i < 5; ++i)
    {
     glPushMatrix();
     glTranslatef(cameraPyramid[i][0], cameraPyramid[i][1], cameraPyramid[i][2]);
     glutSolidSphere(cornerDiameter, 30, 30);
     glPopMatrix();
    }

    // draw frustum lines
    float line_width = lFactor * m_cameraScale * m_sceneSize; // lFactor = 0.001;
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
}


void SolAR3DPointsViewerOpengl::drawSphereCamera(Transform3Df&cPose,
                                           std::vector<float>& cColor,
                                           float cFactor,
                                           float oFactor,
                                           float lFactor){

    Transform3Df glPose = SolAR2GL * cPose;
    GLUquadric * point = gluNewQuadric();
    glPopMatrix();
    glPushMatrix();

     double cornerDiameter = cFactor * m_cameraScale * m_sceneSize * 1.2; //offsetCornerScale = 0.004
    glColor3f(cColor[0], cColor[1], cColor[2]);
    glTranslatef(glPose(0,3), glPose(1,3), glPose(2,3));
    gluSphere(point, GLdouble(cornerDiameter), GLint(oFactor), GLint(lFactor)); // 0.015, 20, 20
    glPopMatrix();


    gluDeleteQuadric(point);

}


FrameworkReturnCode SolAR3DPointsViewerOpengl::display (const std::vector<SRef<CloudPoint>>& points,
                                                        const Transform3Df & pose,
                                                        const std::vector<Transform3Df> keyframePoses,
                                                        const std::vector<Transform3Df> framePoses)
{
    m_points = points;
    m_cameraPose = pose.inverse();
    m_framePoses = framePoses;

    m_keyframePoses.clear();
    for (int i = 0; i<keyframePoses.size(); i++)
        m_keyframePoses.push_back(keyframePoses[i].inverse());

    m_framePoses.clear();
    for (int i = 0; i<framePoses.size(); i++)
        m_framePoses.push_back(framePoses[i].inverse());

    if (m_glWindowID == -1)
    {
        // Compute the center point of the point cloud
        Point3Df minPoint, maxPoint;
        maxPoint(0)=std::numeric_limits<float>::lowest(); maxPoint(1)=std::numeric_limits<float>::lowest(); maxPoint(2)=std::numeric_limits<float>::lowest();
        minPoint(0)=std::numeric_limits<float>::max(); minPoint(1)=std::numeric_limits<float>::max(); minPoint(2)=std::numeric_limits<float>::max();
        for (int i = 0; i < m_points.size(); i++)
        {
            if (points[i]->getX() > maxPoint(0)) maxPoint(0)=points[i]->getX();
            if (points[i]->getY() > maxPoint(1)) maxPoint(1)=points[i]->getY();
            if (points[i]->getZ() > maxPoint(2)) maxPoint(2)=points[i]->getZ();
            if (points[i]->getX() < minPoint(0)) minPoint(0)=points[i]->getX();
            if (points[i]->getY() < minPoint(1)) minPoint(1)=points[i]->getY();
            if (points[i]->getZ() < minPoint(2)) minPoint(2)=points[i]->getZ();
        }
        Vector3f sceneDiagonal;

        // Center the scene on the center of the point cloud
        m_sceneCenter = Point3Df((minPoint(0)+maxPoint(0))/2.0f, -(minPoint(1)+maxPoint(1))/2.0f, -(minPoint(2)+maxPoint(2))/2.0f);

        // Add the camera to the box of the scene
        if (m_cameraPose(0,3) > maxPoint(0)) maxPoint(0)=m_cameraPose(0,3);
        if (m_cameraPose(1,3) > maxPoint(1)) maxPoint(1)=m_cameraPose(1,3);
        if (m_cameraPose(2,3) > maxPoint(2)) maxPoint(2)=m_cameraPose(2,3);
        if (m_cameraPose(0,3) < minPoint(0)) minPoint(0)=m_cameraPose(0,3);
        if (m_cameraPose(1,3) < minPoint(1)) minPoint(1)=m_cameraPose(1,3);
        if (m_cameraPose(2,3) < minPoint(2)) minPoint(2)=m_cameraPose(2,3);

        // Copmute the diagonal of the box to define the scene Size
        sceneDiagonal(0) = maxPoint(0) - minPoint(0);
        sceneDiagonal(1) = maxPoint(1) - minPoint(1);
        sceneDiagonal(2) = maxPoint(2) - minPoint(2);
        m_sceneSize = sceneDiagonal.norm();

        // Set the camera according to the center and the size of the scene.
        m_glcamera.resetview(math_vector_3f(m_sceneCenter.getX(), m_sceneCenter.getY(), m_sceneCenter.getY()), m_sceneSize);

        m_glWindowID = glutCreateWindow(m_title.c_str());
        glutDisplayFunc(Render);
        glutKeyboardFunc(KeyBoard);
        glutMouseFunc(MouseState);
        glutMotionFunc(MouseMotion);
        glutReshapeFunc(ResizeWindow);
        glutIdleFunc(MainLoop);
    }
    if (m_exitKeyPressed)
    {
        m_glcamera.clear(0.0, 0.0, 0.0, 1.0);
        glutDestroyWindow(m_glWindowID);
        return FrameworkReturnCode::_STOP;
    }

    glutMainLoopEvent();
    return FrameworkReturnCode::_SUCCESS;
}



FrameworkReturnCode SolAR3DPointsViewerOpengl::displayClouds(const std::vector<SRef<CloudPoint>>&points_0,
                                                           const std::vector<SRef<CloudPoint>>&points_1,
                                                           std::vector<unsigned int>& color_0,
                                                           std::vector<unsigned int>& color_1){
    m_points = points_0;
    m_points1 = points_1;

    m_pointsColor[0]= color_0[0];m_pointsColor[1] =color_0[1];m_pointsColor[2] =color_0[2];
    m_pointsColor1[0]= color_1[0];m_pointsColor1[1] =color_1[1];m_pointsColor1[2] =color_1[2];

    if (m_glWindowID == -1)
    {
        // Compute the center point of the point cloud
        Point3Df minPoint, maxPoint;
        maxPoint(0)=std::numeric_limits<float>::lowest(); maxPoint(1)=std::numeric_limits<float>::lowest(); maxPoint(2)=std::numeric_limits<float>::lowest();
        minPoint(0)=std::numeric_limits<float>::max(); minPoint(1)=std::numeric_limits<float>::max(); minPoint(2)=std::numeric_limits<float>::max();
        for (int i = 0; i < m_points.size(); i++)
        {
            if (points_1[i]->getX() > maxPoint(0)) maxPoint(0)=points_1[i]->getX();
            if (points_1[i]->getY() > maxPoint(1)) maxPoint(1)=points_1[i]->getY();
            if (points_1[i]->getZ() > maxPoint(2)) maxPoint(2)=points_1[i]->getZ();
            if (points_1[i]->getX() < minPoint(0)) minPoint(0)=points_1[i]->getX();
            if (points_1[i]->getY() < minPoint(1)) minPoint(1)=points_1[i]->getY();
            if (points_1[i]->getZ() < minPoint(2)) minPoint(2)=points_1[i]->getZ();
        }
        Vector3f sceneDiagonal;

        // Center the scene on the center of the point cloud
        m_sceneCenter = Point3Df((minPoint(0)+maxPoint(0))/2.0f, -(minPoint(1)+maxPoint(1))/2.0f, -(minPoint(2)+maxPoint(2))/2.0f);

        // Add the camera to the box of the scene
        if (m_cameraPose(0,3) > maxPoint(0)) maxPoint(0)=m_cameraPose(0,3);
        if (m_cameraPose(1,3) > maxPoint(1)) maxPoint(1)=m_cameraPose(1,3);
        if (m_cameraPose(2,3) > maxPoint(2)) maxPoint(2)=m_cameraPose(2,3);
        if (m_cameraPose(0,3) < minPoint(0)) minPoint(0)=m_cameraPose(0,3);
        if (m_cameraPose(1,3) < minPoint(1)) minPoint(1)=m_cameraPose(1,3);
        if (m_cameraPose(2,3) < minPoint(2)) minPoint(2)=m_cameraPose(2,3);

        // Copmute the diagonal of the box to define the scene Size
        sceneDiagonal(0) = maxPoint(0) - minPoint(0);
        sceneDiagonal(1) = maxPoint(1) - minPoint(1);
        sceneDiagonal(2) = maxPoint(2) - minPoint(2);
        m_sceneSize = sceneDiagonal.norm();

        // Set the camera according to the center and the size of the scene.
        m_glcamera.resetview(math_vector_3f(m_sceneCenter.getX(), m_sceneCenter.getY(), m_sceneCenter.getY()), m_sceneSize);

        m_glWindowID = glutCreateWindow(m_title.c_str());
        glutDisplayFunc(RenderClouds);
        glutKeyboardFunc(KeyBoard);
        glutMouseFunc(MouseState);
        glutMotionFunc(MouseMotion);
        glutReshapeFunc(ResizeWindow);
        glutIdleFunc(MainLoop);
    }
    if (m_exitKeyPressed)
    {
        m_glcamera.clear(0.0, 0.0, 0.0, 1.0);
        glutDestroyWindow(m_glWindowID);
        return FrameworkReturnCode::_STOP;
    }

    glutMainLoopEvent();
    return FrameworkReturnCode::_SUCCESS;
}


void SolAR3DPointsViewerOpengl::OnMainLoop()
{

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

    if (m_drawWorldAxis)
    {
        glLineWidth(m_axisScale);
        glColor3f(255, 0, 0);
        glBegin(GL_LINES);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(m_sceneSize * 0.1 * m_axisScale, 0.0f, 0.0f);
        glEnd();
        glColor3f(0, 255, 0);
        glBegin(GL_LINES);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, -m_sceneSize * 0.1 * m_axisScale, 0.0f);
        glEnd();
        glColor3f(0, 0, 255);
        glBegin(GL_LINES);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, -m_sceneSize * 0.1 * m_axisScale);
        glEnd();
        glLineWidth(1.0f);
    }

    if (m_drawSceneAxis)
    {
        std::vector<Vector4f> sceneAxis;
        sceneAxis.push_back(Vector4f(m_sceneCenter[0], m_sceneCenter[1], m_sceneCenter[2], 1.0));
        sceneAxis.push_back(Vector4f(m_sceneCenter[0] + m_sceneSize * 0.1 * m_axisScale, m_sceneCenter[1], m_sceneCenter[2], 1.0));
        sceneAxis.push_back(Vector4f(m_sceneCenter[0], m_sceneCenter[1] - m_sceneSize * 0.1 * m_axisScale, m_sceneCenter[2], 1.0));
        sceneAxis.push_back(Vector4f(m_sceneCenter[0], m_sceneCenter[1], m_sceneCenter[2] - m_sceneSize * 0.1 * m_axisScale, 1.0));
        glLineWidth(m_axisScale);
        glColor3f(255, 0, 0);
        glBegin(GL_LINES);
        glVertex3f(sceneAxis[0][0], sceneAxis[0][1], sceneAxis[0][2]);
        glVertex3f(sceneAxis[1][0], sceneAxis[1][1], sceneAxis[1][2]);
        glEnd();
        glColor3f(0, 255, 0);
        glBegin(GL_LINES);
        glVertex3f(sceneAxis[0][0], sceneAxis[0][1], sceneAxis[0][2]);
        glVertex3f(sceneAxis[2][0], sceneAxis[2][1], sceneAxis[2][2]);
        glEnd();
        glColor3f(0, 0, 255);
        glBegin(GL_LINES);
        glVertex3f(sceneAxis[0][0], sceneAxis[0][1], sceneAxis[0][2]);
        glVertex3f(sceneAxis[3][0], sceneAxis[3][1], sceneAxis[3][2]);
        glEnd();
        glLineWidth(1.0f);
    }

    if(!m_points.empty())
    {
         glPushMatrix();
         glEnable (GL_POINT_SMOOTH);
         glPointSize(m_pointSize);
         glBegin(GL_POINTS);
         for (unsigned int i = 0; i < m_points.size(); ++i) {
             if (m_fixedPointsColor)
                glColor3f(m_pointsColor[0], m_pointsColor[1], m_pointsColor[2]);
             else
                 glColor3f(m_points[i]->getR(), m_points[i]->getG(), m_points[i]->getB());

             glVertex3f(m_points[i]->getX(), -m_points[i]->getY(), -m_points[i]->getZ());
         }
         glEnd();
         glPopMatrix();
    }
    std::vector<float> colorCurrent = {0.0, 0.0, 1.0};
        std::vector<float> colorFrame = {0.0, 0.0, 1.0};
            std::vector<float> colorKeyframe = {1.0, 0.0, 0.0};

    drawFrustumCamera(m_cameraPose, colorCurrent, 0.013, 0.0015, 0.9);
    if(m_keyframePoses.size()>0){
        for(auto &cc: m_keyframePoses){
            drawFrustumCamera(cc,colorKeyframe,0.013,0.0015,0.001);
        }
    }
    if(m_framePoses.size()>0){
        for(auto &cc: m_framePoses){
            drawSphereCamera(cc,colorFrame,0.002,20.0,20.0);
        }
    }

    glutSwapBuffers();
    glutPostRedisplay();
}


void SolAR3DPointsViewerOpengl::OnRenderClouds(){
    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);

    m_glcamera.set_viewport(0, 0, m_resolutionX, m_resolutionY);
    m_glcamera.setup();
    m_glcamera.use_light(false);

    glClearColor(m_backgroundColor[0], m_backgroundColor[1], m_backgroundColor[2], 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);


    if(!m_points.empty())
    {
         glPushMatrix();
         glEnable (GL_POINT_SMOOTH);
         glPointSize(m_pointSize);
         glBegin(GL_POINTS);
         for (unsigned int i = 0; i < m_points.size(); ++i) {
             if (m_fixedPointsColor)
                glColor3f(m_pointsColor[0], m_pointsColor[1], m_pointsColor[2]);
             else
                 glColor3f(m_points[i]->getR(), m_points[i]->getG(), m_points[i]->getB());

             glVertex3f(m_points[i]->getX(), -m_points[i]->getY(), -m_points[i]->getZ());
         }
         glEnd();
         glPopMatrix();
    }
    if(!m_points1.empty()){
         glPushMatrix();
         glEnable (GL_POINT_SMOOTH);
         glPointSize(m_pointSize);
         glBegin(GL_POINTS);
         for (unsigned int i = 0; i < m_points1.size(); ++i) {
             if (m_fixedPointsColor)
                glColor3f(m_pointsColor1[0], m_pointsColor1[1], m_pointsColor1[2]);
             else
                 glColor3f(m_points1[i]->getR(), m_points1[i]->getG(), m_points1[i]->getB());

             glVertex3f(m_points1[i]->getX(), -m_points1[i]->getY(), -m_points1[i]->getZ());
         }
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
   if (key == m_exitKey)
       m_exitKeyPressed = true;
}


void SolAR3DPointsViewerOpengl::OnMouseMotion(int x, int y)
{
    y = m_resolutionY - y;
    m_glcamera.mouse_move(x, y);
}

void SolAR3DPointsViewerOpengl::OnMouseState(int button, int state, int x, int y)
{
    y = m_resolutionY - y;
    int zoom = 3;
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

        m_glcamera.mouse_wheel(m_zoomSensitivity);
    }
    else if ((button & 4) == 4) {

        m_glcamera.mouse_wheel(-m_zoomSensitivity);
    }
}

}
}
}
