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
#ifndef SOLAR3DPOINTSVIEWEROPENGL_H
#define SOLAR3DPOINTSVIEWEROPENGL_H

#include <vector>

#include "api/display/I3DPointsViewer.h"

#include "SolAROpenglAPI.h"

#include "xpcf/component/ConfigurableBase.h"

#include "glcamera/gl_camera.hpp"

namespace SolAR {
using namespace datastructure;
namespace MODULES {
namespace OPENGL {

class SOLAROPENGL_EXPORT_API SolAR3DPointsViewerOpengl : public org::bcom::xpcf::ConfigurableBase,
    public api::display::I3DPointsViewer
{
public:
    SolAR3DPointsViewerOpengl();
    ~SolAR3DPointsViewerOpengl();

    org::bcom::xpcf::XPCFErrorCode onConfigured() override final;
    void unloadComponent () override final;

    FrameworkReturnCode display(const std::vector<SRef<CloudPoint>>& points, const Transform3Df & pose) override;

protected:
    static SolAR3DPointsViewerOpengl * m_instance;

private:

    /// @brief the title of the window on which the image will be displayed
    std::string m_title = "";

    /// @brief the width of the window on which the image will be displayed (if <=0, the width of the input image)
    unsigned int m_width = 0;

    /// @brief the height of the window on which the image will be displayed (if <=0, the height of the input image)
    unsigned int m_height = 0;

    /// @brief background color
    std::vector<unsigned int> m_backgroundColor = {255,255,255};

    /// @brief points color
    std::vector<unsigned int> m_pointsColor = {0,255,0};

    /// @brief camera color
    std::vector<unsigned int> m_cameraColor = {0,0,255};

    /// @brief size of points
    float m_pointSize = 2.0f;

    /// @brief camera scale
    float m_cameraScale = 1.0f;

    /// @brief zoom sensitivity
    float m_zoomSensitivity = 10.0f;

    /// @brief The key code to press to close the window. If negative, no key is defined to close the window
    int m_exitKey = 27;



    int m_glWindowID = -1;
    std::vector<SRef<CloudPoint>> m_points;
    Transform3Df m_cameraPose;
    gl_camera m_glcamera;
    unsigned int m_resolutionX;
    unsigned int m_resolutionY;
    bool m_exitKeyPressed = false;

    void OnMainLoop() ;
    void OnRender() ;
    void OnResizeWindow(int _w , int _h) ;
    void OnKeyBoard(unsigned char key, int x, int y) ;
    void OnMouseMotion(int x, int y);
    void OnMouseState(int button, int state, int x, int y);

    static void MainLoop()
    {
        m_instance->OnMainLoop();
    }
    static void Render()
    {
        m_instance->OnRender();
    }
    static void ResizeWindow(int _w , int _h)
    {
        m_instance->OnResizeWindow(_w, _h);
    }
    static void KeyBoard(unsigned char key, int x, int y)
    {
        m_instance->OnKeyBoard(key, x , y);
    }

    static void MouseMotion(int x, int y)
    {
        m_instance->OnMouseMotion(x,  y);
    }
    static void MouseState(int button, int state, int x, int y)
    {
        m_instance->OnMouseState(button, state, x , y);
    }
};

}
}
}

#endif // SOLAR3DPOINTSVIEWEROPENGL_H
