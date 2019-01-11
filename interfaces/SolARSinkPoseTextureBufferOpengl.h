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
#ifndef SOLARSINKPOSETEXTUREBUFFEROPENGL_H
#define SOLARSINKPOSETEXTUREBUFFEROPENGL_H

#include "api/sink/ISinkPoseTextureBuffer.h"
#include "SolAROpenglAPI.h"
#include "xpcf/component/ConfigurableBase.h"

#include "datastructure/Image.h"

#include "freeglut.h"
#include <mutex>

namespace SolAR {
using namespace datastructure;
using namespace api::sink;
namespace MODULES {
namespace OPENGL {

/**
 * @class SinkPoseTextureBuffer
 * @brief A Sink for a synchronized pose and texture buffer based on OpenGL texture buffer useful for AR video see-through pipelines.
 *
 * This interface allows to make available a pose to a third party application and to update a OpenGL texture buffer with a new image.
 */

class SOLAROPENGL_EXPORT_API SinkPoseTextureBuffer : public org::bcom::xpcf::ConfigurableBase,
    public api::sink::ISinkPoseTextureBuffer
{
public:
    SinkPoseTextureBuffer();
    ~SinkPoseTextureBuffer() = default;

    /// @brief Set a new image and pose coming from the pipeline.
    /// @param[in] pose The new pose to be made available to a third party application.
    /// @param[in,out] image The new image to update a buffer texture when required.
    void set( const Transform3Df& pose, const SRef<Image>& image ) override;

    /// @brief Set a new image without pose.
    /// @param[in,out] image The new image to update a buffer texture when required.
    void set( const SRef<Image>& image ) override;

    /// @brief Set a pointer to the texture buffer to update it with the new image when required.
    /// @return FrameworkReturnCode::_SUCCESS_ if the texture buffer pointer is well set.
    FrameworkReturnCode setTextureBuffer(const void* textureBufferPointer) override;

    /// @brief Provide an access to the new pose and update the texture buffer with the new image.
    /// The implementation of this interface must be thread safe
    /// @param[in] pose the new pose made available by the pipeline.
    /// @param[in,out] image The new image made available by the pipeline.
    SinkReturnCode udpate( Transform3Df& pose) override;

    /// @brief Provide an access to the new pose and update the texture buffer with the new image only if the image and the pose have been updated by the pipeline.
    /// The implementation of this interface must be thread safe
    /// @param[in] pose the new pose made available by the pipeline.
    /// @return return FrameworkReturnCode::_SUCCESS if a new pose and image are available, otherwise frameworkReturnCode::_ERROR.
    SinkReturnCode tryUpdate( Transform3Df& pose) override;

    void unloadComponent () override final;

private:
    SRef<Image> m_image;
    Transform3Df m_pose;
    GLuint m_textureHandle;
    size_t m_textureBufferSize;

    bool m_newPose;
    bool m_newImage;

    std::mutex m_mutex;

};

}
}
}

#endif // SOLARSINKPOSETEXTUREBUFFEROPENGL_H
