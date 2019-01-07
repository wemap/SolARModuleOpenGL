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

#include "SolARSinkPoseTextureBufferOpengl.h"

namespace xpcf = org::bcom::xpcf;


XPCF_DEFINE_FACTORY_CREATE_INSTANCE(SolAR::MODULES::OPENGL::SinkPoseTextureBuffer)

namespace SolAR {
using namespace datastructure;
namespace MODULES {
namespace OPENGL {

static Transform3Df SolAR2GL = [] {
  Matrix<float, 4, 4> matrix;
  matrix << 1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0;
  return Transform3Df(matrix);
}();

static std::map<Image::ImageLayout, GLenum> SolAR2OpenGLLayout = {{Image::LAYOUT_RGB, GL_RGB},
                                                                  {Image::LAYOUT_RGBA, GL_RGBA},
                                                                  {Image::LAYOUT_RGBX, GL_RGBA},
                                                                  {Image::LAYOUT_GREY, GL_DEPTH_COMPONENT}};
static std::map<Image::DataType, GLenum> SolAR2OpenGLDataType = {{Image::TYPE_8U, GL_UNSIGNED_BYTE},
                                                                 {Image::TYPE_16U, GL_UNSIGNED_SHORT},
                                                                 {Image::TYPE_32U, GL_FLOAT},
                                                                 {Image::TYPE_64U, GL_DOUBLE}};

SinkPoseTextureBuffer::SinkPoseTextureBuffer():ConfigurableBase(xpcf::toUUID<SinkPoseTextureBuffer>())
{
   addInterface<api::sink::ISinkPoseTextureBuffer>(this);
   m_image = nullptr;
   m_pose = Transform3Df::Identity();
   m_textureBufferSize = 0;
   m_newData = false;
}

void SinkPoseTextureBuffer::set(const Transform3Df& pose, const SRef<Image>& image )
{
    m_mutex.lock();
    m_pose = Transform3Df(pose);
    m_image = image->copy();
    m_newData = true;
    m_mutex.unlock();

}

FrameworkReturnCode SinkPoseTextureBuffer::setTextureBuffer(const void* textureBufferHandle)
{
    m_mutex.lock();
    m_textureHandle = (GLuint)(size_t)textureBufferHandle;
    m_mutex.unlock();
   return FrameworkReturnCode::_SUCCESS;
}

FrameworkReturnCode SinkPoseTextureBuffer::udpate( Transform3Df& pose)
{
    m_mutex.lock();
    m_newData = false;

    // Update the Texture Buffer
    glBindTexture( GL_TEXTURE_2D, m_textureHandle );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    // Set texture clamping method
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

    //use fast 4-byte alignment (default anyway) if possible
    glPixelStorei( GL_UNPACK_ALIGNMENT, ( m_image->getStep() & 3 ) ? 1 : 4 );

    //set length of one complete row in data (doesn't need to equal image.cols)
    glPixelStorei( GL_UNPACK_ROW_LENGTH, (int)(m_image->getWidth()));

    GLenum layout, dataType;
    try{
        layout = SolAR2OpenGLLayout.at(m_image->getImageLayout());
    }
    catch  (const std::out_of_range&) {
         LOG_WARNING("The layout of the image {} is not supported", m_image->getImageLayout());
        return FrameworkReturnCode::_ERROR_;
    }

    try{
        dataType = SolAR2OpenGLDataType.at(m_image->getDataType());
    }
    catch  (const std::out_of_range&) {
        LOG_WARNING("The data type of the image {} is not supported", m_image->getDataType());
        return FrameworkReturnCode::_ERROR_;
    }

    glTexSubImage2D( GL_TEXTURE_2D,
                     0,
                     0,
                     0,
                     m_image->getWidth(),
                     m_image->getHeight(),
                     layout,
                     dataType,
                     m_image->data() );

    pose = Transform3Df(m_pose);
    m_mutex.unlock();
    return FrameworkReturnCode::_SUCCESS;
}


FrameworkReturnCode SinkPoseTextureBuffer::tryUpdate( Transform3Df& pose)
{
    if (!m_newData)
        return FrameworkReturnCode::_ERROR_;
    return udpate(pose);
}

}
}
}
