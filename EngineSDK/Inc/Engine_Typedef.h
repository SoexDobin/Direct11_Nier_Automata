#ifndef Engine_Typedef_h__
#define Engine_Typedef_h__

namespace Engine
{

	using Bool			= bool;
	
	using Byte			= signed char;
	using uByte			= unsigned char;
	using Char			= char;
	using tChar			= wchar_t;

	using Short			= signed short;
	using uShort		= unsigned short;

	using uint8			= unsigned __int8;
	using uint16		= unsigned __int16;
	using uint32		= unsigned __int32;

	using int8			= signed __int8;
	using int16			= signed __int16;
	using int32			= signed __int32;


	using Float			= float;
	using Double		= double;

	template <typename T>
	using Shared = std::shared_ptr<T>;

	template <typename T>
	using Unique = std::unique_ptr<T>;

	template <typename T>
	using Weak = std::weak_ptr<T>;

	typedef		XMFLOAT2				Float2;
	typedef		XMFLOAT3				Float3;
	typedef		XMFLOAT4				Float4;
	typedef		XMFLOAT4X4				Float4x4;

	typedef		XMVECTOR				Vector;
	typedef		XMMATRIX				XMatrix;

	typedef		DirectX::SimpleMath::Vector2	Vector2;
	typedef		DirectX::SimpleMath::Vector3	Vector3;
	typedef		DirectX::SimpleMath::Vector4	Vector4;
	typedef		DirectX::SimpleMath::Matrix 	Matrix;
	typedef		DirectX::SimpleMath::Quaternion Quaternion;
	typedef		DirectX::SimpleMath::Color 		Color;
}

#endif // Engine_Typedef_h__
