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

#include "xpcf/xpcf.h"

#include "api/display/I3DPointsViewer.h"
#include "core/Log.h"

using namespace SolAR;
using namespace SolAR::datastructure;
using namespace SolAR::api;

namespace xpcf = org::bcom::xpcf;

int main(int argc, char **argv){

#if NDEBUG
    boost::log::core::get()->set_logging_enabled(false);
#endif

    LOG_ADD_LOG_TO_CONSOLE();

    try {

        /* instantiate component manager*/
        /* this is needed in dynamic mode */
        SRef<xpcf::IComponentManager> xpcfComponentManager = xpcf::getComponentManagerInstance();

        if(xpcfComponentManager->load("SolAROpenGLPointCloudDisplay_conf.xml")!=org::bcom::xpcf::_SUCCESS)
        {
            LOG_ERROR("Failed to load the configuration file SolAROpenGLPointCloudDisplay_conf.xml")
            return -1;
        }

        // declare and create components
        LOG_INFO("Start creating components");

        // component creation
        auto viewer3DPoints =xpcfComponentManager->resolve<display::I3DPointsViewer>();

        // declarations
        std::map<unsigned int, unsigned int> visibility;
        std::vector<SRef<CloudPoint>> testCloud;
        std::vector<SRef<CloudPoint>> testCloud2;
        std::vector<Transform3Df> keyframes;
        std::vector<Transform3Df> frames;
        std::vector<Transform3Df> keyframes2;
        Transform3Df cameraPose = Transform3Df::Identity();

        // Add 4 points representing a simple coordinate system with 4 points
        testCloud.push_back(xpcf::utils::make_shared<CloudPoint>(0,0,0,255, 255,255,0,visibility));
        testCloud.push_back(xpcf::utils::make_shared<CloudPoint>(1,0,0,255, 0, 0, 0, visibility));
        testCloud.push_back(xpcf::utils::make_shared<CloudPoint>(0,1.0,0,0, 255, 0, 0, visibility));
        testCloud.push_back(xpcf::utils::make_shared<CloudPoint>(0,0,1.0,0, 0,255, 0, visibility));

        // Add 4 points representing a simple coordinate system with 4 points int the second cloud
        testCloud2.push_back(xpcf::utils::make_shared<CloudPoint>(2,0,0,255, 0, 0, 0, visibility));
        testCloud2.push_back(xpcf::utils::make_shared<CloudPoint>(0,2.0,0,0, 255, 0, 0, visibility));
        testCloud2.push_back(xpcf::utils::make_shared<CloudPoint>(0,0,2.0,0, 0, 255, 0, visibility));

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

        Transform3Df trans = Transform3Df::Identity();
        keyframes.push_back(trans.translate(Vector3f(1.0f, 2.0f, 2.0f)));
        trans = Transform3Df::Identity();
        keyframes.push_back(trans.translate(Vector3f(0.0f, 2.0f, 2.0f)));
        trans = Transform3Df::Identity();
        keyframes.push_back(trans.translate(Vector3f(-1.0f, 2.0f, 2.0f)));

        trans = Transform3Df::Identity();
        frames.push_back(trans.translate(Vector3f(1.6f, 2.0f, 2.0f)));
        trans = Transform3Df::Identity();
        frames.push_back(trans.translate(Vector3f(1.3f, 2.0f, 2.0f)));
        trans = Transform3Df::Identity();
        frames.push_back(trans.translate(Vector3f(0.6f, 2.0f, 2.0f)));
        trans = Transform3Df::Identity();
        frames.push_back(trans.translate(Vector3f(0.3f, 2.0f, 2.0f)));
        trans = Transform3Df::Identity();
        frames.push_back(trans.translate(Vector3f(-0.3f, 2.0f, 2.0f)));
        trans = Transform3Df::Identity();
        frames.push_back(trans.translate(Vector3f(-0.6f, 2.0f, 2.0f)));

        trans = Transform3Df::Identity();
        keyframes2.push_back(trans.translate(Vector3f(1.0f, 2.2f, 2.0f)));
        trans = Transform3Df::Identity();
        keyframes2.push_back(trans.translate(Vector3f(0.0f, 2.2f, 2.0f)));
        trans = Transform3Df::Identity();
        keyframes2.push_back(trans.translate(Vector3f(-1.0f, 2.2f, 2.0f)));


        while (true){
            if (viewer3DPoints->display(testCloud, cameraPose, keyframes, frames, testCloud2, keyframes2) == FrameworkReturnCode::_STOP)
            {
               LOG_INFO("End of Triangulation sample");
               break;
            }
        }
    }
    catch (xpcf::Exception e)
    {
        LOG_ERROR ("The following exception has been catched: {}", e.what());
        return -1;
    }

    return 0;
}



