// 출처: https://powerclabman.tistory.com/157  [그래픽스] 짐벌락 없는 사원수 회전
// 코드 스크린샷(이미지 52~65)을 보고 옮겨 적은 코드

// ----- 이미지 52: Quaternion 선언 -----
#pragma once
struct Quaternion
{
public:
	inline constexpr Quaternion() = default;
	inline constexpr Quaternion(float inRealPart, Vector3D inImaginaryPart);

	~Quaternion() = default;

	inline constexpr Quaternion operator+(const Quaternion& other) const;
	inline constexpr Quaternion operator*(const Quaternion& other) const;

	inline constexpr Vector3D RotateVector(const Vector3D& inVector) const;

public:
	float RealPart;
	Vector3D ImaginaryPart;
};

// ----- 이미지 53: Quaternion 구현 -----
inline constexpr Quaternion::Quaternion(float inRealPart, Vector3D inImaginaryPart)
	: RealPart(inRealPart)
	, ImaginaryPart(inImaginaryPart)
{
}

inline constexpr Quaternion Quaternion::operator+(const Quaternion& other) const
{
	return Quaternion{
		RealPart + other.RealPart,
		ImaginaryPart + other.ImaginaryPart
	};
}

inline constexpr Quaternion Quaternion::operator*(const Quaternion& other) const
{
	return Quaternion{
		RealPart * other.RealPart - ImaginaryPart.Dot(other.ImaginaryPart),
		other.ImaginaryPart * RealPart + ImaginaryPart * other.RealPart + ImaginaryPart.Cross(other.ImaginaryPart)
	};
}

inline constexpr Vector3D Quaternion::RotateVector(const Vector3D& inVector) const
{
	Vector3D t = ImaginaryPart.Cross(inVector) * 2.f;

	return inVector + t * RealPart + ImaginaryPart.Cross(t);
}

// ----- 이미지 58: 오일러 각 -> 사원수 -----
inline constexpr Quaternion EulerAngle::ToQuaternion() const
{
	Quaternion q = {};

	float sinY, cosY; // Yaw
	float sinP, cosP; // Pitch
	float sinR, cosR; // Roll

	MM::GetSinCos(sinY, cosY, Yaw * 0.5f);
	MM::GetSinCos(sinP, cosP, Pitch * 0.5f);
	MM::GetSinCos(sinR, cosR, Roll * 0.5f);

	q.RealPart = cosY * cosP * cosR + sinY * sinP * sinR;
	q.ImaginaryPart.X = cosY * sinP * cosR + sinY * cosP * sinR;
	q.ImaginaryPart.Y = sinY * cosP * cosR - cosY * sinP * sinR;
	q.ImaginaryPart.Z = cosY * cosP * sinR - sinY * sinP * cosR;

	return q;
}

// ----- 이미지 60: 사원수 -> 오일러 각 -----
inline constexpr EulerAngle Quaternion::ToEulerAngle() const
{
	float yaw = 0.f;
	float pitch = 0.f;
	float roll = 0.f;

	float W = RealPart;
	float X = ImaginaryPart.X;
	float Y = ImaginaryPart.Y;
	float Z = ImaginaryPart.Z;

	// 요 (Yaw)
	yaw = MM::RadianToDegree(::atan2f(2 * (W * Y + Z * X), 1 - 2 * (Y * Y + Z * Z)));

	// 피치 (Pitch)
	float sinP = 2 * (W * X - Z * Y);
	if (sinP > 1.0f) sinP = 1.0f;
	else if (sinP < -1.0f) sinP = -1.0f;
	pitch = MM::RadianToDegree(::asinf(sinP));

	// 롤 (Roll)
	roll = MM::RadianToDegree(::atan2f(2 * (W * Z + X * Y), 1 - 2 * (X * X + Z * Z)));

	return EulerAngle(pitch, yaw, roll);
}

// ----- 이미지 63: 사원수 -> 회전 행렬 -----
inline constexpr Matrix4x4 Quaternion::ToRotateMatrix() const
{
	Vector3D right = RotateVector(Vector3D::s_basisVectorRight);
	Vector3D up = RotateVector(Vector3D::s_basisVectorUp);
	Vector3D out = RotateVector(Vector3D::s_basisVectorOut);

	return Matrix4x4{
		{ right.X,up.X,out.X,0.f },
		{ right.Y,up.Y,out.Y,0.f },
		{ right.Z,up.Z,out.Z,0.f },
		{ 0.f,0.f,0.f,1.f }
	};
}

// ----- 이미지 54 -> 64: Transformer3D 멤버 변경 -----
// [이미지 54]
public:
	//이동
	Vector3D m_position = Vector3D::s_zeroVector;

	//회전
	//EulerAngle m_eulerAngle;
	Quaternion m_rotator;

	//크기
	Vector3D m_scale = Vector3D::s_oneVector;

	//로컬 기저 벡터
	Vector3D m_localBasisRight = Vector3D::s_basisVectorRight;
	Vector3D m_localBasisUp = Vector3D::s_basisVectorUp;
	Vector3D m_localBasisOut = Vector3D::s_basisVectorOut;
};

// [이미지 64]
public:
	//이동
	Vector3D m_position = Vector3D::s_zeroVector;

	//회전
	Quaternion m_rotator = Quaternion::s_identity;

	//크기
	Vector3D m_scale = Vector3D::s_oneVector;

	//로컬 기저 벡터
	Vector3D m_localBasisRight = Vector3D::s_basisVectorRight;
	Vector3D m_localBasisUp = Vector3D::s_basisVectorUp;
	Vector3D m_localBasisOut = Vector3D::s_basisVectorOut;
};

// ----- 이미지 65 -----
	// 회전 변환 (쿼터니언 기반)
	inline void SetRotation(const EulerAngle& angle);
	inline void SetRotation(const Quaternion& angle);
	inline void AddRotation(const EulerAngle& angle);
	inline Quaternion GetRotator()const;
