#include "ObjectManager.h"
#include "GameObject.h"

HRESULT ObjectManager::Initialize(void *arg) {
  m_LayerMask = ETOI(LAYER::ALL_LAYER);

  return EngineManager::Initialize(arg);
}

void ObjectManager::On_Destroy() {
  Clear_GameObjects();

  EngineManager::On_Destroy();
}

void ObjectManager::PriorityUpdate(Float timeDelta) {
  for (auto &[layerBit, objects] : m_ObjectByLayer) {
    if ((m_LayerMask & layerBit) == 1)
      continue;

    for (auto &obj : objects) {
      if (obj->Is_Destroy())
        continue;

      if (obj->Is_Active())
        obj->Priority_Update(timeDelta);
    }
  }
}

void ObjectManager::Update(Float timeDelta) {
  for (auto &[layerBit, objects] : m_ObjectByLayer) {
    if ((m_LayerMask & layerBit) == 1)
      continue;

    for (auto &obj : objects) {
      if (obj->Is_Destroy())
        continue;

      if (obj->Is_Active())
        obj->Update(timeDelta);
    }
  }
}

void ObjectManager::LateUpdate(Float timeDelta) {
  for (auto &[layerBit, objects] : m_ObjectByLayer) {
    if ((m_LayerMask & layerBit) == 1)
      continue;

    for (auto &obj : objects) {
      if (obj->Is_Destroy())
        continue;

      if (obj->Is_Active())
        obj->Late_Update(timeDelta);
    }
  }
}

void ObjectManager::FixedUpdate(Float fixedDelta) {
  for (auto &[layerBit, objects] : m_ObjectByLayer) {
    if ((m_LayerMask & layerBit) == 1)
      continue;

    for (auto &obj : objects) {
      if (obj->Is_Destroy())
        continue;

      if (obj->Is_Active())
        obj->Fixed_Update(fixedDelta);
    }
  }
}

void ObjectManager::Submit_RenderGroup() {
  for (auto &[layerBit, objects] : m_ObjectByLayer) {
    if ((m_LayerMask & layerBit) == 1)
      continue;

    for (auto &obj : objects) {
      if (obj->Is_Destroy())
        continue;

      if (obj->Is_Active())
        obj->Submit_RenderGroup();
    }
  }
}

void ObjectManager::Cleanup_GameObjects() {
  vector<Shared<GameObject>> garbages;
  for (auto &[layerBit, objects] : m_ObjectByLayer) {
    for (auto &obj : objects) {
      if (obj->Is_Destroy()) {
        garbages.push_back(obj);
      }
    }
  }

  for (auto &garbage : garbages) {
    garbage->On_Destroy();
  }

  for (auto &[layerBit, objects] : m_ObjectByLayer) {
    std::erase_if(objects, [this](const Shared<GameObject> &object) {
      if (object->Is_Destroy()) // 삭제 대상 처리
      {
        m_ObjectByUnique.erase(object->Get_ObjectID());
        if (m_ObjectByType.contains(object->Get_TypeID())) {
          auto &typeVec = m_ObjectByType[object->Get_TypeID()];
          std::erase(typeVec, object);
        }
        return true;
      }
      return false;
    });
  }
}

HRESULT ObjectManager::Add_GameObject(const Shared<GameObject> &object) {
  m_ObjectByLayer[object->Get_LayerMask().Get_Layer()].push_back(object);
  m_ObjectByType[object->Get_TypeID()].push_back(object);
  m_ObjectByUnique.emplace(object->Get_ObjectID(), object);

  return S_OK;
}

HRESULT ObjectManager::Clear_GameObjects() {
  for (auto &layer : m_ObjectByLayer) {
    for (auto &obj : layer.second) {
      if (!obj->Is_Destroy()) {
        obj->On_Destroy();
        Object::Destroy(obj);
      }
    }
    layer.second.clear();
  }

  m_ObjectByType.clear();

  m_ObjectByUnique.clear();

  m_LayerMask = ETOI(LAYER::ALL_LAYER);
  return S_OK;
}

Shared<GameObject> ObjectManager::Find_GameObjectByType(uint32 typeID) {
  if (!m_ObjectByType.contains(typeID) || m_ObjectByType[typeID].empty()) {
    MSG_BOX("Failed To Find GameObject By ObjectID");
    return nullptr;
  }

  return m_ObjectByType[typeID].front();
}

Shared<GameObject> ObjectManager::Find_GameObjectByID(uint32 objectID) {
  if (!m_ObjectByUnique.contains(objectID)) {
    MSG_BOX("Failed To Find GameObject By ObjectID");
    return nullptr;
  }

  return m_ObjectByUnique[objectID];
}

unordered_map<uint32, Shared<GameObject>> &ObjectManager::Get_GameObjects() {
  return m_ObjectByUnique;
}

Unique<ObjectManager> ObjectManager::Create() {
  auto objectManager = make_unique<ObjectManager>();

  if (FAILED(objectManager->Initialize(nullptr))) {
    MSG_BOX("Failed To Create ObjectManager");
    return nullptr;
  }

  return objectManager;
}
