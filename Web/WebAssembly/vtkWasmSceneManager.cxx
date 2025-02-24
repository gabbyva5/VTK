// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
#include "vtkWasmSceneManager.h"

#include "vtkCallbackCommand.h"
#include "vtkCommand.h"
#include "vtkObjectFactory.h"

#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------
vtkStandardNewMacro(vtkWasmSceneManager);

//-------------------------------------------------------------------------------
vtkWasmSceneManager::vtkWasmSceneManager() = default;

//-------------------------------------------------------------------------------
vtkWasmSceneManager::~vtkWasmSceneManager() = default;

//-------------------------------------------------------------------------------
void vtkWasmSceneManager::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObjectManager::PrintSelf(os, indent);
}

//-------------------------------------------------------------------------------
bool vtkWasmSceneManager::SetSize(vtkTypeUInt32 identifier, int width, int height)
{
  auto object = this->GetObjectAtId(identifier);
  if (auto renderWindow = vtkRenderWindow::SafeDownCast(object))
  {
    if (auto iren = renderWindow->GetInteractor())
    {
      iren->UpdateSize(width, height);
      return true;
    }
  }
  return false;
}

//-------------------------------------------------------------------------------
bool vtkWasmSceneManager::Render(vtkTypeUInt32 identifier)
{
  auto object = this->GetObjectAtId(identifier);
  if (auto renderWindow = vtkRenderWindow::SafeDownCast(object))
  {
    renderWindow->Render();
    return true;
  }
  return false;
}

//-------------------------------------------------------------------------------
bool vtkWasmSceneManager::ResetCamera(vtkTypeUInt32 identifier)
{
  auto object = this->GetObjectAtId(identifier);
  if (auto renderer = vtkRenderer::SafeDownCast(object))
  {
    renderer->ResetCamera();
    return true;
  }
  return false;
}

//-------------------------------------------------------------------------------
bool vtkWasmSceneManager::StartEventLoop(vtkTypeUInt32 identifier)
{
  vtkRenderWindowInteractor::InteractorManagesTheEventLoop = false;
  auto object = this->GetObjectAtId(identifier);
  if (auto renderWindow = vtkRenderWindow::SafeDownCast(object))
  {
    auto interactor = renderWindow->GetInteractor();
    std::cout << "Started event loop id=" << identifier
              << ", interactor=" << interactor->GetObjectDescription() << '\n';
    interactor->Start();
    return true;
  }
  return false;
}

//-------------------------------------------------------------------------------
bool vtkWasmSceneManager::StopEventLoop(vtkTypeUInt32 identifier)
{
  auto object = this->GetObjectAtId(identifier);
  if (auto renderWindow = vtkRenderWindow::SafeDownCast(object))
  {
    auto interactor = renderWindow->GetInteractor();
    std::cout << "Stopping event loop id=" << identifier
              << ", interactor=" << interactor->GetObjectDescription() << '\n';
    interactor->TerminateApp();
    return true;
  }
  return false;
}

namespace
{
struct CallbackBridge
{
  vtkWasmSceneManager::ObserverCallbackF f;
  vtkTypeUInt32 SenderId;
};
}

//-------------------------------------------------------------------------------
unsigned long vtkWasmSceneManager::AddObserver(
  vtkTypeUInt32 identifier, std::string eventName, ObserverCallbackF callback)
{
  auto object = vtkObject::SafeDownCast(this->GetObjectAtId(identifier));
  if (object == nullptr)
  {
    return 0;
  }
  vtkNew<vtkCallbackCommand> callbackCmd;
  callbackCmd->SetClientData(new CallbackBridge{ callback, identifier });
  callbackCmd->SetClientDataDeleteCallback([](void* clientData) {
    auto* bridge = reinterpret_cast<CallbackBridge*>(clientData);
    delete bridge;
  });
  callbackCmd->SetCallback([](vtkObject*, unsigned long eid, void* clientData, void*) {
    auto* bridge = reinterpret_cast<CallbackBridge*>(clientData);
    bridge->f(bridge->SenderId, vtkCommand::GetStringFromEventId(eid));
  });
  return object->AddObserver(eventName.c_str(), callbackCmd);
}

//-------------------------------------------------------------------------------
bool vtkWasmSceneManager::RemoveObserver(vtkTypeUInt32 identifier, unsigned long tag)
{

  auto object = vtkObject::SafeDownCast(this->GetObjectAtId(identifier));
  if (object == nullptr)
  {
    return false;
  }
  object->RemoveObserver(tag);
  return true;
}

VTK_ABI_NAMESPACE_END
