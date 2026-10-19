// 출처: https://powerclabman.tistory.com/147  [그래픽스] 2D 카메라 설계
#pragma once

class Camera2D
{
public:
	Camera2D() = default;
	virtual ~Camera2D() = default;

	inline void AddPosition(const Vector2D& _vector);
	inline void SetPosition(const Vector2D& _vector);
	inline Vector2D GetPosition() const;

	inline void AddRotation(float _degree);
	inline void SetRotation(float _degree);
	inline float GetRotation() const;

	inline void AddZoom(float ratio);
	inline void SetZoom(float ratio);
	inline float GetZoomRatio() const;

	inline Matrix3x3 GetViewMatrix() const;

	inline Vector2D GetLocalBasisRight() const;
	inline Vector2D GetLocalBasisUp() const;

	inline Vector2D GetUnitVector() const;
private:
	inline void UpdateLocalBasis();

private:
	Vector2D m_position = Vector2D::s_zeroVector;

	float m_degreeRotation = 0.f;

	float m_zoomRatio = 1.f;

	Vector2D m_localBasisRight = Vector2D::s_basisVectorX;
	Vector2D m_localBasisUp = Vector2D::s_basisVectorY;
};

inline void Camera2D::AddPosition(const Vector2D& _vector)
{
	m_position = m_position + _vector;
}

inline void Camera2D::SetPosition(const Vector2D& _vector)
{
	m_position = _vector;
}

inline Vector2D Camera2D::GetPosition() const
{
	return m_position;
}

inline void Camera2D::AddRotation(float _degree)
{
	m_degreeRotation += _degree;
	UpdateLocalBasis();
}

inline void Camera2D::SetRotation(float _degree)
{
	m_degreeRotation = _degree;
	UpdateLocalBasis();
}

inline float Camera2D::GetRotation() const
{
	return m_degreeRotation;
}

inline void Camera2D::AddZoom(float ratio)
{
	m_zoomRatio += ratio;
}

inline void Camera2D::SetZoom(float ratio)
{
	m_zoomRatio = ratio;
}

inline float Camera2D::GetZoomRatio() const
{
	return m_zoomRatio;
}

Matrix3x3 Camera2D::GetViewMatrix() const {
	float tx = -m_position.X;
	float ty = -m_position.Y;

	float sx = m_zoomRatio;
	float sy = m_zoomRatio;

	float cosV = m_localBasisRight.X;
	float sinV = m_localBasisRight.Y;

	return Matrix3x3{
		{ cosV * sx, sinV * sy, cosV * tx * sx - sinV * ty * sy },
		{ -sinV * sx,  cosV * sy, sinV * tx * sx + cosV * ty * sy },
		{ 0.f,        0.f,        1.f }
	};
}

inline Vector2D Camera2D::GetLocalBasisRight() const
{
	return m_localBasisRight;
}

inline Vector2D Camera2D::GetLocalBasisUp() const
{
	return m_localBasisUp;
}

inline Vector2D Camera2D::GetUnitVector() const
{
	return m_localBasisRight;
}

inline void Camera2D::UpdateLocalBasis()
{
	MM::Circular(m_degreeRotation, -180.f, 180.f);

	float sinV = 0.f;
	float cosV = 0.f;

	MM::GetSinCos(sinV, cosV, m_degreeRotation);

	m_localBasisRight = Vector2D(cosV, sinV);
	m_localBasisUp = Vector2D(-sinV, cosV);
}
