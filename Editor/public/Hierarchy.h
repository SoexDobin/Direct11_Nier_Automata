#pragma once
#include "EditorObject.h"

NS_BEGIN(Engine)
class GameObject;
NS_END

NS_BEGIN(Editor)

class Hierarchy : public EditorObject {
  NO_COPY(Hierarchy)
public:
  Hierarchy();
  ~Hierarchy() override;

public:
  HRESULT Initialize() override;
  void Update() override;
  void Render() override;

private:
  void Render_Node(const Shared<Engine::GameObject> &pObj);
  void Delete_Selected();

public:
  static Shared<Hierarchy> Create();
};

NS_END