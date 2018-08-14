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

#include <iostream>
#include <string>
#include <vector>

#include <boost/log/core.hpp>

// ADD COMPONENTS HEADERS HERE

#include "SolARModuleOpengl_traits.h"

#include "xpcf/xpcf.h"

#include "api/display/I3DPointsViewer.h"

using namespace SolAR;
using namespace SolAR::datastructure;
using namespace SolAR::api;
using namespace SolAR::MODULES::OPENGL;

namespace xpcf = org::bcom::xpcf;

int main(int argc, char **argv){

#if NDEBUG
    boost::log::core::get()->set_logging_enabled(false);
#endif

    LOG_ADD_LOG_TO_CONSOLE();

    /* instantiate component manager*/
    /* this is needed in dynamic mode */
    SRef<xpcf::IComponentManager> xpcfComponentManager = xpcf::getComponentManagerInstance();

    if(xpcfComponentManager->load("conf_PointsCloud.xml")!=org::bcom::xpcf::_SUCCESS)
    {
        LOG_ERROR("Failed to load the configuration file conf_PointsCloud.xml")
        return -1;
    }

    // declare and create components
    LOG_INFO("Start creating components");

 // component creation
    auto viewer3DPoints =xpcfComponentManager->create<SolAR3DPointsViewerOpengl>()->bindTo<display::I3DPointsViewer>();

    // declarations
    std::vector<int> visibility;
    std::vector<SRef<CloudPoint>> testCloud;
    Transform3Df cameraPose = Transform3Df::Identity();


    // Add 4 points representing a simple coordinate system with 4 points
    testCloud.push_back(xpcf::utils::make_shared<CloudPoint>(0,0,0,255, 255,255,0,visibility));
    testCloud.push_back(xpcf::utils::make_shared<CloudPoint>(1,0,0,255, 0, 0, 0, visibility));
    testCloud.push_back(xpcf::utils::make_shared<CloudPoint>(0,1.0,0,0, 255, 0, 0, visibility));
    testCloud.push_back(xpcf::utils::make_shared<CloudPoint>(0,0,1.0,0, 0,255, 0, visibility));

    // Move camera along the X, Y and Z-axis
    cameraPose.translate(Vector3f(2.0f, 2.0f, 2.0f));

    // Rotate the camera to look at the scene center
    cameraPose.rotate(Maths::AngleAxisf(0.0f * SOLAR_DEG2RAD, Vector3f::UnitX()) *
                      Maths::AngleAxisf(-90.0f * SOLAR_DEG2RAD, Vector3f::UnitY()) *
                      Maths::AngleAxisf(0.0f * SOLAR_DEG2RAD, Vector3f::UnitZ()));
    cameraPose.rotate(Maths::AngleAxisf(45.0f * SOLAR_DEG2RAD, Vector3f::UnitX()) *
                      Maths::AngleAxisf(0.0f * SOLAR_DEG2RAD, Vector3f::UnitY()) *
                      Maths::AngleAxisf(0.0f * SOLAR_DEG2RAD, Vector3f::UnitZ()));
    cameraPose.rotate(Maths::AngleAxisf(0.0f * SOLAR_DEG2RAD, Vector3f::UnitX()) *
                      Maths::AngleAxisf(-45.0f * SOLAR_DEG2RAD, Vector3f::UnitY()) *
                      Maths::AngleAxisf(0.0f * SOLAR_DEG2RAD, Vector3f::UnitZ()));

    while (true){
        if (viewer3DPoints->display(testCloud, cameraPose) == FrameworkReturnCode::_STOP)
        {
           LOG_INFO("End of Triangulation sample");
           break;
        }
    }
    return 0;
}



