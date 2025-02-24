// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

#include "vtkGLTFTexture.h"

#include "vtkGLTFDocumentLoader.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkTexture.h"

VTK_ABI_NAMESPACE_BEGIN
//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkGLTFTexture);

//------------------------------------------------------------------------------
vtkSmartPointer<vtkTexture> vtkGLTFTexture::GetVTKTexture()
{
  vtkNew<vtkTexture> texture;
  texture->SetColorModeToDirectScalars();
  texture->SetBlendingMode(vtkTexture::VTK_TEXTURE_BLENDING_MODE_MODULATE);
  // Approximate filtering settings
  if (this->Sampler.MinFilter == vtkGLTFDocumentLoader::Sampler::FilterType::NEAREST ||
    this->Sampler.MinFilter == vtkGLTFDocumentLoader::Sampler::FilterType::LINEAR)
  {
    texture->MipmapOff();
  }
  else
  {
    texture->MipmapOn();
  }

  if (this->Sampler.WrapS == vtkGLTFDocumentLoader::Sampler::WrapType::CLAMP_TO_EDGE ||
    this->Sampler.WrapT == vtkGLTFDocumentLoader::Sampler::WrapType::CLAMP_TO_EDGE)
  {
    texture->RepeatOff();
    texture->EdgeClampOn();
  }
  else if (this->Sampler.WrapS == vtkGLTFDocumentLoader::Sampler::WrapType::REPEAT ||
    this->Sampler.WrapT == vtkGLTFDocumentLoader::Sampler::WrapType::REPEAT)
  {
    texture->RepeatOn();
    texture->EdgeClampOff();
  }
  else
  {
    vtkWarningWithObjectMacro(nullptr, "Mirrored texture wrapping is not supported!");
  }

  if (this->Sampler.MinFilter == vtkGLTFDocumentLoader::Sampler::FilterType::LINEAR ||
    this->Sampler.MinFilter == vtkGLTFDocumentLoader::Sampler::FilterType::LINEAR_MIPMAP_NEAREST ||
    this->Sampler.MinFilter == vtkGLTFDocumentLoader::Sampler::FilterType::NEAREST_MIPMAP_LINEAR ||
    this->Sampler.MinFilter == vtkGLTFDocumentLoader::Sampler::FilterType::LINEAR_MIPMAP_LINEAR ||
    this->Sampler.MagFilter == vtkGLTFDocumentLoader::Sampler::FilterType::LINEAR ||
    this->Sampler.MagFilter == vtkGLTFDocumentLoader::Sampler::FilterType::LINEAR_MIPMAP_NEAREST ||
    this->Sampler.MagFilter == vtkGLTFDocumentLoader::Sampler::FilterType::NEAREST_MIPMAP_LINEAR ||
    this->Sampler.MagFilter == vtkGLTFDocumentLoader::Sampler::FilterType::LINEAR_MIPMAP_LINEAR)
  {
    texture->InterpolateOn();
  }
  texture->SetInputData(this->Image);
  return texture;
}

//------------------------------------------------------------------------------
void vtkGLTFTexture::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "MagFilter: " << this->Sampler.MagFilter << "\n"
     << indent << "MinFilter: " << this->Sampler.MinFilter << "\n"
     << indent << "WrapS: " << this->Sampler.WrapS << "\n"
     << indent << "WrapT: " << this->Sampler.WrapT << "\n"
     << indent << "Image: " << this->Image << "\n";
}

VTK_ABI_NAMESPACE_END
