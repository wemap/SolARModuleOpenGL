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

#ifndef SOLARMODULEOPENGL_TRAITS_H
#define SOLARMODULEOPENGL_TRAITS_H

#include "xpcf/core/traits.h"

namespace SolAR {
namespace MODULES {
namespace OPENGL {
class SolAR3DPointsViewerOpengl;
class SinkPoseTextureBuffer;
}
}
}

XPCF_DEFINE_COMPONENT_TRAITS(SolAR::MODULES::OPENGL::SolAR3DPointsViewerOpengl,
                             "afd38ea0-9a46-11e8-9eb6-529269fb1459",
                             "SolAR3DPointsViewerOpengl",
                             "Displays in a window a set of 3D points as well as the current camera and its previous path (based on an OpenGL implementation).");

XPCF_DEFINE_COMPONENT_TRAITS(SolAR::MODULES::OPENGL::SinkPoseTextureBuffer,
                             "3af7813c-4647-4d70-9cc6-e3cedd8dd77c",
                             "SinkPoseTextureBuffer",
                             "A Sink component for a synchronized pose and texture buffer based on OpenGL texture buffer");

#endif // SOLARMODULEOPENGL_TRAITS_H
