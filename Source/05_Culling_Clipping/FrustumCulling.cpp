// 출처: https://powerclabman.tistory.com/155  [그래픽스] 가시부피와 절단
// 코드 스크린샷을 보고 옮겨 적은 코드

// ----- 이미지 16: Plane -----
#pragma once

struct Plane
{
public:
	Plane() = delete;
	~Plane() = default;

	constexpr inline Plane(const Vector3D& normalVector, const Vector3D& point);
	constexpr inline Plane(float a, float b, float c, float d);

	//점과 평면 내외부 판정
	inline bool IsOutside(const Vector3D& point) const
	{
		return NormalVector.Dot(point) + D > 0.f;
	}

public:
	Vector3D NormalVector = Vector3D::s_zeroVector;
	float D = 0.f;
};


inline constexpr Plane::Plane(const Vector3D& normalVector, const Vector3D& point)
	: NormalVector(normalVector)
{
	if (NormalVector.LengthSquared() != 1.f)
		NormalVector.Normalize();

	D = -NormalVector.Dot(point);
}

inline constexpr Plane::Plane(float a, float b, float c, float d)
	: NormalVector(Vector3D(a, b, c))
	, D(d)
{
	Assert(MM::Abs(NormalVector.LengthSquared() - 1.f) < MM::ERROR_BOUND, "You Define Plane With unNormalized Vector");
}

// ----- 이미지 17 + 45: Frustum -----
class Frustum
{
public:
	constexpr inline Frustum(const std::array<Plane, 6>& planes)
		: m_planes(planes)
	{
	}

	inline bool IsOutside(const Vector3D& point) const
	{
		size_t len = m_planes.size();

		for (int i = 0; i < len; i++)
		{
			if (m_planes[i].IsOutside(point))
				return true;
		}

		return false;
	}

	// 이미지 45: AABB 판정
	inline eBoundFlag IsOutside(const AABB& point) const
	{
		Vector3D nearPoint, farPoint;

		for (const Plane& plane : m_planes)
		{
			Vector3D normal = plane.NormalVector;

			nearPoint.X = (normal.X >= 0.f) ? point.MinPoint.X : point.MaxPoint.X;
			nearPoint.Y = (normal.Y >= 0.f) ? point.MinPoint.Y : point.MaxPoint.Y;
			nearPoint.Z = (normal.Z >= 0.f) ? point.MinPoint.Z : point.MaxPoint.Z;

			farPoint.X = (normal.X >= 0.f) ? point.MaxPoint.X : point.MinPoint.X;
			farPoint.Y = (normal.Y >= 0.f) ? point.MaxPoint.Y : point.MinPoint.Y;
			farPoint.Z = (normal.Z >= 0.f) ? point.MaxPoint.Z : point.MinPoint.Z;

			if (plane.IsOutside(nearPoint))
			{
				return eBoundFlag::OUTSIDE;
			}

			if (plane.IsOutside(farPoint))
			{
				return eBoundFlag::INTERSECT;
			}
		}
		return eBoundFlag::INSIDE;
	}

private:
	std::array<Plane, 6> m_planes;
};

// ----- 이미지 18: 뷰 공간 절두체 (초기 버전, 종횡비 고려 X) -----
void RenderManager::Render3D()
{
	//절두체
	float sint = 0.f;
	float cost = 0.f;

	MM::GetSinCos(sint, cost, m_camera3D.GetViewAngle() * 0.5f);

	float n = m_camera3D.GetNearPlaneDistance();
	float f = m_camera3D.GetFarPlaneDistance();

	Frustum frustum{ {
		Plane(0.f, cost, sint,0.f),
		Plane(0.f, -cost, sint,0.f),
		Plane(cost, 0.f, sint,0.f),
		Plane(-cost, 0.f, sint,0.f),
		Plane(0.f, 0.f, 1.f,n),
		Plane(0.f, 0.f, -1.f,-f),
		} };

	// ----- 이미지 19: 오브젝트 중심점으로 절단 -----
	while (m_mash3DObjectQueue.IsEmpty() == false)
	{
		const Mesh3DObject* obj = m_mash3DObjectQueue.Front();
		m_mash3DObjectQueue.Pop();

		Mesh3D* mesh = obj->GetMesh();

		vector<Vertex3D> vBuffer = mesh->GetVertexBuffer();
		const vector<MeshIndex>& iBuffer = mesh->GetIndexBuffer();

		//정점 변환
		const Transformer3D* trans = obj->GetTransformer();

		//절단
		{
			Vector3D point = m_camera3D.GetViewMatrix() * trans->m_position;

			if (frustum.IsOutside(point))
			{
				continue;
			}
		}

		const Matrix4x4& viewMat = m_camera3D.GetViewMatrix();
		const Matrix4x4& perspectiveMat = m_camera3D.GetPerspectiveMatrix(m_screenSize);
		Matrix4x4 finalMatrix = perspectiveMat * viewMat * trans->GetModelMatrix();

		VertexShader3D(vBuffer, finalMatrix);

		//정점 처리
		for (const MeshIndex& idx : iBuffer)
		{
			const Vertex3D& fst = vBuffer[idx.First];
			const Vertex3D& sec = vBuffer[idx.Second];
			const Vertex3D& trd = vBuffer[idx.Third];
			// ... (이하 생략, 153번 글과 동일)

// ----- 이미지 23: 종횡비를 고려한 절두체 -----
	//상하 절두체
	float sinY = 0.f;
	float cosY = 0.f;

	MM::GetSinCos(sinY, cosY, m_camera3D.GetViewAngle() * 0.5f);

	//좌우측 절두체
	float a = static_cast<float>(m_screenSize.width) / m_screenSize.height;
	float d = m_camera3D.m_distance;

	float lenInv = MM::SqrtInv(d * d + a * a);

	float cosX = d * lenInv;
	float sinX = a * lenInv;

	float n = m_camera3D.GetNearPlaneDistance();
	float f = m_camera3D.GetFarPlaneDistance();

	Frustum frustum = {{
			Plane(0.f, cosY, sinY, 0.f),
			Plane(0.f, -cosY, sinY, 0.f),
			Plane(cosX,0.f , sinX, 0.f),
			Plane(-cosX, 0.f, sinX, 0.f),
			Plane(0.f, 0.f, 1.f, n),
			Plane(0.f, 0.f, -1.f, -f),
	}};

// ----- 이미지 34: 원근 투영 행렬의 행으로 절두체 평면 추출 -----
void RenderManager::Render3D()
{
	// 뷰행렬 및 원근투영 행렬
	const Matrix4x4& viewMat = m_camera3D.GetViewMatrix();
	const Matrix4x4& perspectiveMat = m_camera3D.GetPerspectiveMatrix(m_screenSize);

	const Vector4D& row0 = perspectiveMat.GetRow(0);
	const Vector4D& row1 = perspectiveMat.GetRow(1);
	const Vector4D& row2 = perspectiveMat.GetRow(2);
	const Vector4D& row3 = perspectiveMat.GetRow(3);

	Frustum frustum = {
		{
		Plane{row0 - row3},
		Plane{-row0 - row3},
		Plane{row1 - row3},
		Plane{-row1 - row3},
		Plane{row2 - row3},
		Plane{-row2 - row3}
		}
	};

	while (m_mash3DObjectQueue.IsEmpty() == false)
	{
		const Mesh3DObject* obj = m_mash3DObjectQueue.Front();
		m_mash3DObjectQueue.Pop();

		Mesh3D* mesh = obj->GetMesh();
		// ...

// ----- 이미지 39: AABB -----
struct AABB
{
	AABB() = default;
	~AABB() = default;

	inline void UpdateBoundVolume(const std::vector<Vertex3D>& vertexBuffer)
	{
		for (const Vertex3D& v : vertexBuffer)
		{
			MinPoint.X = min(MinPoint.X, v.Position.X);
			MinPoint.Y = min(MinPoint.Y, v.Position.Y);
			MinPoint.Z = min(MinPoint.Z, v.Position.Z);

			MaxPoint.X = max(MaxPoint.X, v.Position.X);
			MaxPoint.Y = max(MaxPoint.Y, v.Position.Y);
			MaxPoint.Z = max(MaxPoint.Z, v.Position.Z);
		}
	}

public:
	Vector3D MinPoint = Vector3D{ MM::LARGE_FLOAT,MM::LARGE_FLOAT ,MM::LARGE_FLOAT };
	Vector3D MaxPoint = Vector3D{ -MM::LARGE_FLOAT,-MM::LARGE_FLOAT ,-MM::LARGE_FLOAT };
};

// ----- 이미지 43 -----
enum class eBoundFlag
{
	INSIDE,
	INTERSECT,
	OUTSIDE
};

// ----- 이미지 44: Mesh3DObject -----
#include "GameObject.h"

class Transformer3D;
class Texture;
class Mesh3D;

class Mesh3DObject : public GameObject
{
	using Super = GameObject;
public:
	Mesh3DObject() = default;
	virtual ~Mesh3DObject() = default;

	void Init() override;
	void Update() override;
	void Render() override;

	inline Mesh3D* GetMesh() const;
	inline Transformer3D* GetTransformer() const;

	inline AABB& GetBoundVolumeRef();
	inline AABB GetBoundVolume() const;

private:
	Mesh3D* m_mesh = nullptr;
	Transformer3D* m_transfomer = nullptr;

	AABB m_boundVolume = {};
};

// ----- 이미지 46 / 47: 뷰 공간 AABB 절단 -----
	//절단
	{
		AABB box = obj->GetBoundVolume();

		//box 정점 변환
		box.MaxPoint = m_camera3D.GetViewMatrix() * trans->GetModelMatrix() * box.MaxPoint;
		box.MinPoint = m_camera3D.GetViewMatrix() * trans->GetModelMatrix() * box.MinPoint;

		swap(box.MaxPoint.X, box.MinPoint.X);
		swap(box.MaxPoint.Z, box.MinPoint.Z);

		eBoundFlag flag = frustum.IsOutside(box);

		if (flag == eBoundFlag::OUTSIDE)
		{
			continue;
		}
		// 이미지 46에서는 활성, 이미지 47에서는 주석 처리됨
		//else if (flag == eBoundFlag::INTERSECT)
		//{
		//	drawMode = eMeshDrawMode::WIREFRAME;
		//}
	}

// ----- 이미지 51: 최종 버전 (PV 행렬 * 모델 행렬의 행으로 로컬 공간 절단) -----
		const Matrix4x4& pvMatrix = m_camera3D.GetPVMatrix(m_screenSize);
		Matrix4x4 finalMatrix = pvMatrix * trans->GetModelMatrix();

		eMeshDrawMode drawMode = mesh->GetDrawMode();

		// 절단
		const Vector4D& row0 = finalMatrix.GetRow(0);
		const Vector4D& row1 = finalMatrix.GetRow(1);
		const Vector4D& row2 = finalMatrix.GetRow(2);
		const Vector4D& row3 = finalMatrix.GetRow(3);

		Frustum frustum = {
			{
			Plane{row0 - row3},
			Plane{-row0 - row3},
			Plane{row1 - row3},
			Plane{-row1 - row3},
			Plane{row2 - row3},
			Plane{-row2 - row3}
			}
		};

		const AABB& box = obj->GetBoundVolume();
		eBoundFlag flag = frustum.IsOutside(box);

		if (flag == eBoundFlag::OUTSIDE)
		{
			continue;
		}

		if (flag == eBoundFlag::INTERSECT)
		{
			drawMode = eMeshDrawMode::WIREFRAME;
		}

		VertexShader3D(vBuffer, finalMatrix);
