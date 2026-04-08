# UI Coordinates Binding Failure Analysis

## 📌 Issue Summary
`HpBarWorldUI` (Floating UI) is expected to track the screen coordinates of a 3D target (`Em3100`) using `XMVector3Project`. However, despite the camera moving and the monster standing in the 3D world, the output `screenPos` persistently returns `1280, 304` (the initial static projection) and fails to dynamically update in `Late_Update()`. This causes the HP bar to remain fixed on the screen, acting like a static Orthographic UI rather than an entity-tracking UI.

---

## 🔍 Root Cause Analysis

If `XMVector3Project` continuously evaluates to `1280, 304`, it mathematically guarantees that the inputs (`worldPos`, `viewMat`, `projMat`) are not dynamically changing during the frame execution where `XMVector3Project` is called. We break down the failure points below:

### 1. The Disconnect in `GAME_INSTANCE->Get_Transform(D3DTS::VIEW)`
During `HpBarWorldUI::Late_Update()`, the following variables are gathered:
```cpp
Vector3 worldPos = m_Target.lock()->Get_Transform()->Get_Position() + m_WorldOffset;
Matrix viewMat = GAME_INSTANCE->Get_Transform(D3DTS::VIEW);
```
- The 3D Engine renders Meshes (Terrain, Em3100) correctly when the camera moves. This proves `viewMat` is natively working during the `Render` phase.
- However, `GAME_INSTANCE->Get_Transform(D3DTS::VIEW)` is reading from the `Pipeline` state precisely during `Late_Update()`.
- **In `Game::Update_Engine()`:**
  ```cpp
  m_ObjectManager->LateUpdate(delta);               // <--- HpBarWorldUI accesses Pipeline ViewMatrix here
  m_CameraManager->Bind_MainCamera_Transform();     // <--- Pipeline ViewMatrix is actually updated HERE
  m_Pipeline->Update_Pipeline();
  ```
- **Architectural Lag vs Static Locking:** Because `LateUpdate` runs *before* the `CameraManager` writes its final matrix to the `Pipeline`, `HpBarWorldUI` is reading the previous frame's matrix. A 1-frame delayed matrix should still produce dynamic movement. The fact that the output is entirely static implies that the **Pipeline matrix is fundamentally frozen** during the `Update` phase.

### 2. The EditorManager Hijack & Engine State Bypass
You indicated: *"엔진쪽 업데이트는 무조건 인게임 카메라로만 하는데 중간에 한번 바꾸었다고 그렇게되는건가?"*
The engine runs with an `InGameCamera`. But in `EditorManager`, if the `InGameCamera` is stationary (e.g., waiting for player logic that isn't fully driving it, or it was never moved by input because the EditorCamera is processing the inputs), its WorldMatrix stays static.

- If the user moves the view via the **EditorCamera**, the `viewMat` that moves is native to the `EditorCamera`.
- During `LateUpdate`, the `GAME_INSTANCE` is loaded with `InGameCamera`'s static transform because `EditorManager::Render` restores the `InGameCamera` to `MainCamera` right before the frame ends.
- **Therefore:** The `viewMat` retrieved in `Late_Update` by `HpBarWorldUI` is the perfectly static `InGameCamera` matrix, yielding an eternally static `screenPos` calculation. The visual movement on your screen is exclusively produced in the subsequent `EditorManager::Render` phase which forcefully overrides the `Pipeline` with the `EditorCamera`.

### 3. Verification of 3D `worldPos` Evaluation
```cpp
Vector3 worldPos = m_Target.lock()->Get_Transform()->Get_Position();
```
- If the monster's `Transform(Root)` never moves (`(0,0,0)` permanently) and it uses purely mesh-driven or animation-driven movement without root motion, `worldPos` generates a zero vector.
- A stationary `0,0,0` vector projected onto a stagnant `viewMat` will eternally yield `1280, 304` (with the camera at 0, 5, -10 looking at 0, 0, 0).

---

## ✅ Mathematical Resolution 

If `Update_Engine` absolutely locks to `InGameCamera` and `EditorManager` doesn't sync it until `Render`, the UI **must** actively request the Matrix of the camera that is currently displaying the scene mathematically, regardless of `Pipeline` states. 
Instead of relying on the engine's stale state variable in `Late_Update`, modifying the transformation retrieval precisely guarantees synchronization:

```cpp
// 1. D3DTS::VIEW 파이프라인 의존을 탈피하고 현재 지정된 MainCamera에서 수동 역연산 추출
Shared<Camera> pCurrentCamera = GAME_INSTANCE->Get_MainCamera();
Matrix trueViewMat = pCurrentCamera->Get_Transform()->Get_WorldMatrix().Invert();
Matrix trueProjMat = XMMatrixPerspectiveFovLH(
    pCurrentCamera->Get_FovY(), pCurrentCamera->Get_Aspect(),
    pCurrentCamera->Get_NearPlane(), pCurrentCamera->Get_FarPlane()
);

Vector3 screenPos = XMVector3Project(
    worldPos, 
    viewport.TopLeftX, viewport.TopLeftY,
    viewport.Width, viewport.Height, 
    viewport.MinDepth, viewport.MaxDepth,
    trueProjMat, trueViewMat, Matrix::Identity
);
```

By decoupling the projection algorithm from the `Pipeline` snapshot (`D3DTS::VIEW`) and directly inverting the `Camera`'s actual active transform upon `Late_Update()`, the matrix calculation bypasses the update-order discrepancy entirely.
