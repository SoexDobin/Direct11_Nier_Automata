#pragma once
#include "AmusementParkLight.h"
#include "Monster.h"

NS_BEGIN(Engine)
class SphereCollider;
class Navigation;
NS_END

NS_BEGIN(Client)
	class Em3000MeleeSight;

	class Em3000Body;
class Em3000Movement;
class Em3000StateMachine;
class MonsterSight;

class CLIENT_DLL Em3000 final : public Monster
{
    RTTR_ENABLE(Monster)
public:
    typedef struct tagEm3000Desc : public MONSTER_CONTAINER_DESC
    {

    } EM3000_DESC;
public:
    explicit Em3000() = default;
    explicit Em3000(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
    explicit Em3000(const Em3000& rhs);
    ~Em3000() override = default;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(void* arg) override;
    void On_Destroy() override;

public:
    void Priority_Update(Float timeDelta) override;
    void Update(Float timeDelta) override;
    void Late_Update(Float timeDelta) override;
    void Fixed_Update(Float fixedDelta) override;
    HRESULT Render() override;
    void Submit_RenderGroup() override;

public:
    const TRANSFORM_FRAME& Get_BodyModelTransform() const;
    void Set_Navigation(const Shared<Navigation>& navigation) override;
    void TakeDamage(const DAMAGE_INFO& dmgInfo) override;
    void OnDeath() override;

public:
    void OnCollisionEnter(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider) override;
    void OnCollisionStay(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider) override;
    void OnCollisionExit(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider) override;

public:
    const list<Shared<Entity>>& Get_Puppets() { return m_Puppets; }
    void SummonPuppet(uint32 summonCount = 1);

public:
    void Set_IntroLight(const Shared<class AmusementParkLight>& light) { m_light = light; }
    Shared<class AmusementParkLight> Get_IntroLight() const { return m_light; }
    Shared<GameObject> Get_Target() const { return m_TargetPlayer.lock(); }


private:
    HRESULT Ready_PartObjects();
    HRESULT Ready_Components();

private:
    Shared<Em3000Body> m_MainBody{ nullptr };
    Shared<Em3000Movement> m_Em3000Movement{ nullptr };
    Shared<SphereCollider> m_PhysicalZone{ nullptr };
    Shared<Navigation> m_Navigation{ nullptr };

    Shared<Em3000MeleeSight> m_MeleeSight{ nullptr };
    Shared<MonsterSight> m_Sight{ nullptr };

    Shared<AmusementParkLight> m_light{ nullptr };
    
    std::list<Shared<Entity>> m_Puppets;
    Bool m_IsDestroying{ false };

public:
    static Shared<Em3000> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
    Shared<GameObject> Clone(void* arg) override;

public:
    enum class EM3000_STATE
    {
        CENEMATIC = 500,
        IDLE = 0,               // EM3000_0000,
        PHASE1_INTRO,
        PHASE1_GROGGY, 
        PHASE1_MELEE,
        PHASE1_RANGE,
        PHASE2_TRANSFORM,
        PHASE2_STOMP,
        PHASE2_CHASE, 
        PHASE2_PUPPET,
        PHASE2_GROGGY,
        DEAD, 


        MOVE_START = 1,         // EM3000_0010
        MOVE_2 = 2,             // EM3000_0011
        MOVE_3 = 3,             // EM3000_0012
        MOVE_END = 4,           // EM3000_0013

        BEFORE_GROGGY = 14,     // EM3000_0150
        GROGGY_IN = 15,         // EM3000_0300
        GROGGY_LOOP_1 = 16,     // EM3000_0301
        GROGGY_END = 17,        // EM3000_0302
        GROGGY_LOOP_2 = 18,     // EM3000_0305
        GROGGY_LOOP_3 = 19,     // EM3000_0306

        HAND_WAVE = 20,         // EM3000_0500
        MIXER_START = 21,       // EM3000_0510
        MIXING = 22,            // EM3000_0511
        MIXER_END = 23,         // EM3000_0512
        FRONT_STOMP = 24,       // EM3000_0520

        INTRO = 26, // EM3000_0556

        DANMAK_START_1 = 29,    // EM3000_05A0
        DANMAK_LOOP_1 = 30,     // EM3000_05A1
        DANMAK_END_1 = 31,      // EM3000_05A2

        DANMAK_START_2 = 32,      // EM3000_05A5
        DANMAK_LOOP_2 = 33,      // EM3000_05A6
        DANMAK_END_2 = 34,      // EM3000_05A7

        DANMAK_START_3 = 35,    // EM3000_05C0
        DANMAK_LOOP_3 = 36,     // EM3000_05C1
        DANMAK_END_3 = 37,      // EM3000_05C2

        PHASE_CHANGE1 = 38,          // EM3000_0600 (예시의 PHASE1과 동일한 인덱스)
        PHASE_CHANGE2 = 39,          // EM3000_0610
        PHASE_CHANGE3 = 42,          // EM3000_100A

        PHASE2_IDLE = 41,       // EM3000_1000

        CHASE = 43,             // EM3000_1010
        READY_CHASE = 44,       // EM3000_101A

        JUMP_STOMP = 51,        // EM3000_1500
        SCREAMING = 52,         // EM3000_1550

        FLOATING = 53,          // EM3000_E000
        // EM3000_E001
        // EM3000_E005
        SHOUTING  = 56, //// EM3000_E008

        END
    };
};

/*
Animation Mapping List (Based on em300f.dat)
Index | Animation Name | Description
---------------------------------------
0  | em3000_0000 | idle
1  | em3000_0010 | move
2  | em3000_0011 | move
3  | em3000_0012 | move
4  | em3000_0013 | move
14 | em3000_0150 | before groggy
15 | em3000_0300 | groggy in
16 | em3000_0301 | groggy ~
17 | em3000_0302 | groggy end
18 | em3000_0305 | groggy ~
19 | em3000_0306 | groggy ~
20 | em3000_0500 | ?? 손짓 휙
21 | em3000_0510 | start 믹서기
22 | em3000_0511 | 믹서기중
23 | em3000_0512 | 믹서기 end
24 | em3000_0520 | 전방 찍기
29 | em3000_05a0 | start danmak
30 | em3000_05a1 | danmak
31 | em3000_05a2 | end danmak
32 | em3000_05a5 | end danmak
33 | em3000_05a6 | end danmak
34 | em3000_05a7 | end danmak
35 | em3000_05c0 | start danmak
36 | em3000_05c1 | danmak
37 | em3000_05c2 | end danmak
38 | em3000_0600 | phase2
39 | em3000_0610 | phase2
42 | em3000_100a | phase2
43 | em3000_1010 | chase
44 | em3000_101a | ready chase
51 | em3000_1500 | jump 찍기
52 | em3000_1550 | screaming
53 | em3000_e000 | floating
*/

NS_END