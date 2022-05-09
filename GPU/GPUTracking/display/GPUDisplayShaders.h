//**************************************************************************\
//* This file is property of and copyright by the ALICE Project            *\
//* ALICE Experiment at CERN, All rights reserved.                         *\
//*                                                                        *\
//* Primary Authors: Matthias Richter <Matthias.Richter@ift.uib.no>        *\
//*                  for The ALICE HLT Project.                            *\
//*                                                                        *\
//* Permission to use, copy, modify and distribute this software and its   *\
//* documentation strictly for non-commercial purposes is hereby granted   *\
//* without fee, provided that the above copyright notice appears in all   *\
//* copies and that both the copyright notice and this permission notice   *\
//* appear in the supporting documentation. The authors make no claims     *\
//* about the suitability of this software for any purpose. It is          *\
//* provided "as is" without express or implied warranty.                  *\
//**************************************************************************

/// \file GPUDisplayShaders.h
/// \author David Rohr

#ifndef GPUDISPLAYSHADERS_H
#define GPUDISPLAYSHADERS_H

#include "GPUCommonDef.h"
namespace GPUCA_NAMESPACE
{
namespace gpu
{

struct GPUDisplayShaders {
  static constexpr const char* vertexShader = R"(
#version 450 core
layout (location = 0) in vec3 pos;
uniform mat4 ModelViewProj;

void main()
{
  gl_Position = ModelViewProj * vec4(pos.x, pos.y, pos.z, 1.0);
}
)";

  static constexpr const char* fragmentShader = R"(
#version 450 core
out vec4 fragColor;
uniform vec4 color;

void main()
{
  fragColor = vec4(color.x, color.y, color.z, 1.f);
}
)";

  static constexpr const char* vertexShaderTexture = R"(
#version 450 core
layout (location = 0) in vec4 vertex;
out vec2 TexCoords;

uniform mat4 projection;

void main()
{
  gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
  TexCoords = vertex.zw;
}
)";

  static constexpr const char* fragmentShaderTexture = R"(
#version 450 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D tex;
uniform float alpha;

void main()
{
    color = vec4(texture(tex, TexCoords).rgb, alpha);
}
)";

  static constexpr const char* fragmentShaderText = R"(
#version 450 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec4 textColor;

void main()
{
  vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
  color = textColor * sampled;
}
)";
};

} // namespace gpu
} // namespace GPUCA_NAMESPACE

#endif
