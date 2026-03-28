#pragma once
#include "ContainerObject.h"
#include "Pl0000Input.h"
#include "Pl0000Body.h"

NS_BEGIN(Engine)
	class Shader;
    class Model;
}

NS_BEGIN(Client)

class Pl0000Movement;
class Pl0000StateMachine;
class Pl0000Body;
class WP0070Body;
class WP0220Body;


class CLIENT_DLL Pl0000 final : public ContainerObject
{
	RTTR_ENABLE(ContainerObject)

public:
    typedef struct tagStateContainer {

        
    } STATE_CONTAINER;

public:
	explicit Pl0000();
	explicit Pl0000(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit Pl0000(const Pl0000& rhs);
	~Pl0000() override = default;

public: /* pl0000 */
    TRANSFORM_FRAME Get_BodyModelTransform() const { return m_MainBody->Get_ModelTransform(); }
    const Matrix& Get_SheathingMatrix() const { return m_SheathMatrix; }


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

private:

private:
    HRESULT Ready_PartObjects();
    HRESULT Ready_Components();

private:
    Shared<Pl0000Body> m_MainBody{ nullptr };
    Shared<Pl0000StateMachine> m_Pl0000States{ nullptr };
    Shared<Pl0000Input> m_Pl0000Input{ nullptr };
    Shared<Pl0000Movement> m_Pl0000Movement{ nullptr };

private:
    Matrix m_SheathMatrix{};

public:
	static Shared<Pl0000> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<GameObject> Clone(void* arg) override;

public:
    enum PL0000_STATE {
        IDLE                = 899,
        RUN                 = 898,
        SPRINT              = 897,
        DASH                = 896,
        JUMP                = 895,

        // IDLE
        IDLE_Neutral            = 46,
        IDLE_STAND_TO_Neutral   = 39,

        // RUN
        RUN_CYCLE           = 2,
        RUN_STOP_L          = 3,
        RUN_STOP_R          = 4,
        
        // SPRINT
        SPRINT_CYCLE        = 5,
        SPRINT_STOP_R       = 6,
        
        // DASH
        STAND_TO_DASH_F     = 61,
        DASH_F              = 62,
        DASH_TO_STAND_F     = 63,
        STAND_TO_DASH_B     = 64,
        DASH_B              = 65,
        DASH_TO_STAND_B     = 66,
        STAND_TO_DASH_R     = 67,
        DASH_R              = 68,
        DASH_TO_STAND_R     = 69,
        STAND_TO_DASH_L     = 70,
        DASH_L              = 71,
        DASH_TO_STAND_L     = 72,
        DASH_F_TO_SPRINT    = 73,
        DASH_B_TO_SPRINT    = 74,
        DASH_R_TO_SPRINT    = 75,
        DASH_L_TO_SPRINT    = 76,
        
        // JUMP
        JUMP_ENTER          = 13, // 20
        JUMP_HOLD           = 14, // 21
        

        STAND_TO_JUMP       = 23,
        RUN_TO_JUMP         = 24,
        SPRINT_TO_JUMP      = 25,

        JUMP_TO_RUN         = 10,
        JUMP_TO_SPRINT      = 11,
        JUMP_TO_STAND = 15, // 22
        //JUMP_TO_STAND       = 114,

        MAIN_ATTACK_2B      = 0x00000020,
        SUB_ATTACK_2B       = 0x00000040,

    };
};

NS_END

// --- 기본 이동 및 시스템 모션 ---
// pl0000_0000 Industry standard (pl0000_0000)
// pl0000_0001 Walk Cycle
// pl0000_0002 Run Cycle
// pl0000_0003 Run Stop L
// pl0000_0004 Run Stop R
// pl0000_0006 Sprint Cycle
// pl0000_0007 Sprint Stop R
// pl0000_0008 Walk to Run
// pl0000_0009 Run Stop R
// pl0000_000a Run Stop L
// pl0000_000b Landing Run
// pl0000_000c roll to sprint after jumping
// pl0000_000d Evade Left

// pl0000_0010 Jump 1/3, pl0000_0011 Jump 2/3, pl0000_0012 Jump 3/3
// pl0000_0014 Double Jump
// pl0000_0015 Double Jump (Start)
// pl0000_0016 Double Jump (Land)
// pl0000_0017 Double Jump (Loop/Other)
// pl0000_0018 Jump 1/3, pl0000_0019 Jump 2/3, pl0000_001a Jump 3/3

// pl0000_001b Stand to Jump
// pl0000_001c Run to Jump
// pl0000_001d Sprint to Jump
// pl0000_001f Pod grab

// pl0000_0024 Evade Front
// pl0000_0025 Evade Backward
// pl0000_0026 Evade Right
// pl0000_0027 Evade Left

// --- 아이들 및 상태 전환 ---
// pl0000_0028 Walk Stop, 
// pl0000_0029 Walk Stop, 
// pl0000_002a Walk Stop, 
// pl0000_002b Walk Stop, 
// pl0000_002c Walk Stop, 
// pl0000_002d Walk Stop, 
// pl0000_002e Walk Stop, 
// pl0000_002f Walk Stop

// pl0000_0030 Stand to Neutral Idle
// pl0000_0031 Neutral Idle Cycle
// pl0000_0032 Neutral Idle to Idle Crossed Arms
// pl0000_0033 Idle Crossed Arms Cycle
// pl0000_0034 Neutral Idle to Idle Crossed Arms
// pl0000_003a Stand to Neutral Idle
// pl0000_003b Stand to Neutral Idle
// pl0000_003f Neutral Idle Cycle

// --- 소형검 (Small Sword) 공격 ---
// pl0000_0100 Light Ground 1
// pl0000_0101 Light Ground 2
// pl0000_0102 Light Ground 3
// pl0000_0103 Light Ground 4
// pl0000_0104 Light Ground 5
// pl0000_0105 Light Ground 6
// pl0000_0106 Light Ground 7
// pl0000_0110 Heavy Ground 1
// pl0000_0111 Heavy Ground Hold (No Contact)
// pl0000_0113 Heavy Ground 2
// pl0000_0116 Heavy Ground 3
// pl0000_0119 Heavy Ground Hold (Contact)
// pl0000_0130 Light Air 1 & 3
// pl0000_0131 Light Air 2 & 4
// pl0000_0132 Light Air 5
// pl0000_0150 Heavy Air Up
// pl0000_0155 Heavy Air Down 1/3, pl0000_0156 Heavy Air Down 2/3, pl0000_0157 Heavy Air Down 3/3
// pl0000_0160 Heavy Sprint
// pl0000_0180 Light Run/Sprint
// pl0000_0188 Heavy Run
// pl0000_018d Heavy Air Down Directional 1/3, pl0000_018e Heavy Air Down Directional 2/3, pl0000_018f Heavy Air Down Directional 3/3
// pl0000_01e0 Counter, pl0000_01e1 Counter
// pl0000_01e8 Taunt Small Sword
// pl0000_01f0 Small Sword Sheathe ?

// --- 대형검 (Heavy Sword) 공격 ---
// pl0000_0200 Light Ground 1
// pl0000_0201 Light Ground 2
// pl0000_0202 Light Ground 3
// pl0000_0203 Light Ground 4
// pl0000_0205 Heavy Ground 1
// pl0000_0208 Heavy Ground 1 (Alt)
// pl0000_0209 Heavy Ground 2
// pl0000_020d Heavy Ground 3
// pl0000_0211 Heavy Ground Hold Unfinished
// pl0000_0212 Heavy Ground Hold Cycle
// pl0000_0213 Heavy Ground Hold No Contact
// pl0000_0214 Heavy Ground Hold Contact
// pl0000_021a Heavy Sprint
// pl0000_0220 Heavy Air Up
// pl0000_0225 Heavy Air Down 1/3, pl0000_0226 Heavy Air Down 2/3, pl0000_0227 Heavy Air Down 3/3
// pl0000_022a Light Air 1
// pl0000_0280 Light Run
// pl0000_028d Heavy Air Down Directional 1/3, pl0000_028e Heavy Air Down Directional 2/3, pl0000_028f Heavy Air Down Directional 3/3
// pl0000_02e0 Counter, pl0000_02e1 Counter
// pl0000_02e8 Taunt Heavy Sword
// pl0000_02f0 Heavy Sword Sheathe ?

// --- 대미지 및 특수 모션 ---
// pl0000_1065 Self-Destruct 2B
// pl0000_1067 Self-Destruct A2
// pl0000_1080 Stumble Right
// pl0000_1081 Stumble Left
// pl0000_1082 Stumble Front, pl0000_1083 Stumble Front
// pl0000_1085 Stumble Front to Walk
// pl0000_108a Stumble Front to Stand
// pl0000_aa0a Jump 1/3 ?, pl0000_aa0b Jump Cycle 2/3, pl0000_aa0c Jump Landing 3/3 ?
// pl0000_b010 Jump 1/3, pl0000_b011 Jump Cycle 2/3, pl0000_b012 Jump Landing 3/3
// pl0000_e090 Vomit Cycle