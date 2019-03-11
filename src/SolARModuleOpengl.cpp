/**
 * @copyright Copyright (c) 2015 All Right Reserved, B-com http://www.b-com.com/
 *
 * This file is subject to the B<>Com License.
 * All other rights reserved.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 */

#include <iostream>

#include "xpcf/module/ModuleFactory.h"
#include "SolARModuleOpengl_traits.h"

#include "SolAR3DPointsViewerOpengl.h"
#include "SolARSinkPoseTextureBufferOpengl.h"

namespace xpcf=org::bcom::xpcf;

XPCF_DECLARE_MODULE("6e960df6-9a36-11e8-9eb6-529269fb1459", "SolARModuleOpenGL", "SolARModuleOpenGL module");

extern "C" XPCF_MODULEHOOKS_API xpcf::XPCFErrorCode XPCF_getComponent(const boost::uuids::uuid& componentUUID,SRef<xpcf::IComponentIntrospect>& interfaceRef)
{
    xpcf::XPCFErrorCode errCode = xpcf::XPCFErrorCode::_FAIL;
    errCode = xpcf::tryCreateComponent<SolAR::MODULES::OPENGL::SolAR3DPointsViewerOpengl>(componentUUID,interfaceRef);
    if (errCode != xpcf::XPCFErrorCode::_SUCCESS)
    {
        errCode = xpcf::tryCreateComponent<SolAR::MODULES::OPENGL::SinkPoseTextureBuffer>(componentUUID,interfaceRef);
    }
    return errCode;
}

XPCF_BEGIN_COMPONENTS_DECLARATION
XPCF_ADD_COMPONENT(SolAR::MODULES::OPENGL::SolAR3DPointsViewerOpengl)
XPCF_ADD_COMPONENT(SolAR::MODULES::OPENGL::SinkPoseTextureBuffer)
XPCF_END_COMPONENTS_DECLARATION
