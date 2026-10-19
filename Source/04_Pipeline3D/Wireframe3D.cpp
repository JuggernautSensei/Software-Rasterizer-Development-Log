// 출처: https://powerclabman.tistory.com/151  [그래픽스] 가볍게 구현해본 3D 와이어 프레임 랜더링
// 코드 스크린샷을 보고 옮겨 적은 코드

// ----- 이미지 0: Mesh2D -----
class Mesh2D : public Component
{
	using Super = Component;
public:
	inline Mesh2D();
	virtual ~Mesh2D() = default;

	void Init() override;
	void Update() override;
	void Render() override;

	inline void ReserveVertexBuffer(size_t reserve);
	inline void ReserveIndexBuffer(size_t reserve);

	inline std::vector<Vertex2D> GetVertexBuffer() const;

	inline std::vector<MeshIndex> GetIndexBuffer() const;

	inline void AddVertex(const Vertex2D& vertex);
	inline void AddIndex(const MeshIndex& index);

	inline eMeshDrawMode GetDrawMode() const { return m_mode; }
	inline void SetDrawMode(eMeshDrawMode mode) { m_mode = mode; }

	inline Texture* GetTextureOrNull() const { return m_texture; }
	void SetTexture(const wstring& namne);

private:
	std::vector<Vertex2D> m_vertices;
	std::vector<MeshIndex> m_indices;

	eMeshDrawMode m_mode = eMeshDrawMode::WIREFRAME;

	Texture* m_texture = nullptr;
};

// ----- 이미지 1: RenderManager 멤버 -----
	//Mesh Buffer
	MC::Queue<const Mesh2DObject*> m_mash2DObjectQueue;

// ----- 이미지 2: Matrix4x4 -----
class Matrix4x4
{
public:
	// 기본 생성자
	inline constexpr Matrix4x4();

	// 초기화 생성자
	inline constexpr explicit Matrix4x4(const float values[4][4]);
	inline constexpr explicit Matrix4x4(const int values[4][4]);
	inline constexpr Matrix4x4(const initializer_list<initializer_list<float>>& values);
	inline constexpr Matrix4x4(const initializer_list<initializer_list<int>>& values);

	bool operator==(const Matrix4x4& other) const = default;

	inline constexpr Vector4D operator*(const Vector4D& other) const;
	inline constexpr Vector3D operator*(const Vector3D& other) const;

	inline constexpr Matrix4x4(const Matrix4x4& other);

	inline constexpr Matrix4x4 operator+(const Matrix4x4& other) const;
	inline constexpr Matrix4x4 operator*(const Matrix4x4& other) const;
	inline constexpr Matrix4x4 operator-(const Matrix4x4& other) const;
	inline constexpr Matrix4x4 operator*(float scalar) const;
	inline constexpr Matrix4x4 operator/(float scalar) const;
	inline constexpr Matrix4x4 Transpose() const;

public:
	static const Matrix4x4 s_identityMatrix;

	union
	{
		// 행
		struct
		{
			float e11, e12, e13, e14,
				e21, e22, e23, e24,
				e31, e32, e33, e34,
				e41, e42, e43, e44;
		};

		float matrix[4][4];
	};
};

// ----- 이미지 3: Vector3D -----
class Vector3D
{
public:
	constexpr Vector3D();
	constexpr Vector3D(float x, float y, float z);
	constexpr Vector3D(const Vector3D& other);
	constexpr Vector3D operator+(const Vector3D& other) const;
	constexpr Vector3D operator-(const Vector3D& other) const;
	constexpr Vector3D operator*(float scalar) const;
	constexpr Vector3D operator/(float scalar) const;
	constexpr float LengthSquared() const;
	constexpr float Length() const;
	constexpr void Normalize();
	constexpr Vector3D GetNormalized() const;

	constexpr inline float Dot(const Vector3D& other) const
	{
		return X * other.X + Y * other.Y + Z * other.Z;
	}

	constexpr inline Vector3D Cross(const Vector3D& other) const
	{
		return Vector3D(
			Y * other.Z - Z * other.Y,
			Z * other.X - X * other.Z,
			X * other.Y - Y * other.X
		);
	}

	constexpr inline Vector3D TripleVectorProduct(const Vector3D& B, const Vector3D& C) const
	{
		return B * (this->Dot(C)) - C * (this->Dot(B));
	}

	constexpr inline Vector2D ToVector2D() const;

	constexpr inline Vector4D ToVector4DPoint() const;
	constexpr inline Vector4D ToVector4DVector() const;

public:
	constexpr static int32_t s_dimension = 3;
	const static Vector3D s_zeroVector;
	const static Vector3D s_basisVectorRight;
	const static Vector3D s_basisVectorUp;
	const static Vector3D s_basisVectorOut;
	const static Vector3D s_oneVector;

	union
	{
		struct
		{
			float X, Y, Z;
		};

		std::array<float, s_dimension> axis;
	};
};

// ----- 이미지 4: Vertex3D -----
struct Vertex3D
{
	constexpr Vertex3D() = default;
	constexpr Vertex3D(const Vector3D& Position, const Color32& Color)
		: Position(Position), Color(Color), UV(Vector2D::s_zeroVector)
	{
	}

	constexpr Vertex3D(const Vector3D& Position, const Vector2D& UVvector)
		: Position(Position), Color(Color32::s_blue), UV(UVvector)
	{
	}

	constexpr Vertex3D(const Vector3D& Position, const Color32& Color, const Vector2D& UVvector)
		: Position(Position), Color(Color), UV(UVvector)
	{
	}

	constexpr inline Vertex2D ToVertex2D() const
	{
		return Vertex2D(Position.ToVector2D(), Color, UV);

	}

	Vector3D Position;
	Color32 Color;

	Vector2D UV;
};

// ----- 이미지 10 + 11: EulerAngle -----
class EulerAngle
{
public:
	constexpr inline EulerAngle() = default;
	~EulerAngle() = default;

	constexpr inline EulerAngle(float yaw, float roll, float pitch)
		: m_yaw(yaw)
		, m_roll(roll)
		, m_pitch(pitch)
	{}

	constexpr inline void GetLocalBasisAxis(OUT Vector3D& outRight, OUT Vector3D& outUp, OUT Vector3D& outOut) const
	{
		// 회전 순서: 롤 -> 피치 -> 요
		// 행렬 곱 순서: 요 * 피치 * 롤
		float sinY = 0.f, cosY = 0.f;
		float sinP = 0.f, cosP = 0.f;
		float sinR = 0.f, cosR = 0.f;

		MM::GetSinCos(sinY, cosY, m_yaw);
		MM::GetSinCos(sinR, cosR, m_roll);
		MM::GetSinCos(sinP, cosP, m_pitch);

		outRight = Vector3D{
			cosY * cosR + sinY * sinP * sinR,
			cosP * sinR,
			-sinY * cosR + cosY * sinP * sinR,
		};

		outUp = Vector3D{
			-cosY * sinR + sinY * sinP * cosR,
			cosP * cosR,
			sinY * sinR + cosY * sinP * cosR,
		};

		outOut = Vector3D{
			sinY * cosP,
			-sinP,
			cosY * cosP,
		};
	}
	// (이미지 10은 여기서 잘림)

public:
	static const EulerAngle s_zeroAngle;

	//단위 Degree
	float m_yaw = 0.f;
	float m_roll = 0.f;
	float m_pitch = 0.f;
};

// ----- 이미지 13: Transformer3D::GetModelMatrix -----
inline Matrix4x4 GetModelMatrix() const
{
	return Matrix4x4{
		{m_localBasisRight.X * m_scale.X,m_localBasisUp.X * m_scale.Y,m_localBasisOut.X * m_scale.Z,m_position.X},
		{m_localBasisRight.Y * m_scale.X,m_localBasisUp.Y * m_scale.Y,m_localBasisOut.Y * m_scale.Z,m_position.Y},
		{m_localBasisRight.Z * m_scale.X,m_localBasisUp.Z * m_scale.Y,m_localBasisOut.Z * m_scale.Z,m_position.Z},
		{0.f,0.f,0.f,1.f}
	};
}

// ----- 이미지 14: 육면체 메시 (Dummy) -----
void Dummy::Init()
{
	Super::Init();

	Mesh3D* mesh = GetMesh();
	m_trans = GetTransformer();
	mesh->SetDrawMode(eMeshDrawMode::WIREFRAME);
	// 정점 추가
	mesh->AddVertex(Vertex3D{ Vector3D{50.f, 50.f, 50.f}, Color32::s_blue });   // 0번 정점
	mesh->AddVertex(Vertex3D{ Vector3D{-50.f, 50.f, 50.f}, Color32::s_blue });  // 1번 정점
	mesh->AddVertex(Vertex3D{ Vector3D{-50.f, -50.f, 50.f}, Color32::s_blue }); // 2번 정점
	mesh->AddVertex(Vertex3D{ Vector3D{50.f, -50.f, 50.f}, Color32::s_blue });  // 3번 정점
	mesh->AddVertex(Vertex3D{ Vector3D{50.f, 50.f, -50.f}, Color32::s_blue });  // 4번 정점
	mesh->AddVertex(Vertex3D{ Vector3D{-50.f, 50.f, -50.f}, Color32::s_blue }); // 5번 정점
	mesh->AddVertex(Vertex3D{ Vector3D{-50.f, -50.f, -50.f}, Color32::s_blue }); // 6번 정점
	mesh->AddVertex(Vertex3D{ Vector3D{50.f, -50.f, -50.f}, Color32::s_blue }); // 7번 정점

	// 앞면
	mesh->AddIndex(MeshIndex{ 0, 1, 2 });
	mesh->AddIndex(MeshIndex{ 2, 3, 0 });

	// 뒷면
	mesh->AddIndex(MeshIndex{ 4, 5, 6 });
	mesh->AddIndex(MeshIndex{ 6, 7, 4 });

	// 왼쪽면
	mesh->AddIndex(MeshIndex{ 1, 5, 6 });
	mesh->AddIndex(MeshIndex{ 6, 2, 1 });

	// 오른쪽면
	mesh->AddIndex(MeshIndex{ 0, 4, 7 });
	mesh->AddIndex(MeshIndex{ 7, 3, 0 });

	// 윗면
	mesh->AddIndex(MeshIndex{ 0, 1, 5 });
	mesh->AddIndex(MeshIndex{ 5, 4, 0 });

	// 아랫면
	mesh->AddIndex(MeshIndex{ 2, 6, 7 });
	mesh->AddIndex(MeshIndex{ 7, 3, 2 });

}

// ----- 이미지 16: Camera3D -----
class Camera3D
{
public:
	Camera3D() = default;
	~Camera3D() = default;

	const Transformer3D& GetTransfomer3D() const
	{
		return m_trans;
	}

	Transformer3D& GetTransfomer3DRef()
	{
		return m_trans;
	}

	Matrix4x4 GetViewMatrix() const
	{
		Vector3D rightAxis = m_trans.GetLocalBasisRight();
		Vector3D upAxis = m_trans.GetLocalBasisUp();
		Vector3D outAxis = m_trans.GetLocalBasisOut();

		Vector3D position = m_trans.GetPosition();

		return Matrix4x4{
			{rightAxis.X * m_zoom, rightAxis.Y * m_zoom, rightAxis.Z * m_zoom, -position.X * m_zoom},
			{upAxis.X * m_zoom, upAxis.Y * m_zoom, upAxis.Z * m_zoom, -position.Y * m_zoom},
			{outAxis.X * m_zoom, outAxis.Y * m_zoom, outAxis.Z * m_zoom, -position.Z * m_zoom},
			{0.f, 0.f, 0.f, 1.f}
		};

	}

	void SetZoom(float zoom)
	{
		m_zoom = zoom;
		m_trans.SetScale(Vector3D{ m_zoom, m_zoom, m_zoom });
	}

	float GetZoom() const
	{
		return m_zoom;
	}

	void AddZoom(float zoom)
	{
		m_zoom += zoom;
		m_trans.SetScale(Vector3D{ m_zoom, m_zoom, m_zoom });
	}

private:
	Transformer3D m_trans;
	float m_zoom = 1.f;
};

// ----- 이미지 15 (+17에서 수정된 부분 반영): RenderManager::Render3D -----
void RenderManager::Render3D()
{
	while (m_mash3DObjectQueue.IsEmpty() == false)
	{
		const Mesh3DObject* obj = m_mash3DObjectQueue.Front();

		Mesh3D* mesh = obj->GetMesh();

		vector<Vertex3D> vBuffer = mesh->GetVertexBuffer();
		const vector<MeshIndex>& iBuffer = mesh->GetIndexBuffer();

		//정점 변환
		const Transformer3D* trans = obj->GetTransformer();

		// [이미지 15 원본]
		// Matrix4x4 finalMatrix = m_camera3D.GetViewMatrix() * trans->GetModelMatrix();
		// [이미지 17 수정본]
		const Matrix4x4& viewMat = m_camera3D.GetViewMatrix();
		Matrix4x4 finalMatrix = viewMat * trans->GetModelMatrix();

		VertexShader3D(vBuffer, finalMatrix);

		//정점 처리
		for (const MeshIndex& idx : iBuffer)
		{
			const Vertex3D& fst = vBuffer[idx.First];
			const Vertex3D& sec = vBuffer[idx.Second];
			const Vertex3D& trd = vBuffer[idx.Third];

			array<Vertex2D, 3> vertexArray = { fst.ToVertex2D(),sec.ToVertex2D(),trd.ToVertex2D() };

			if (Engine::GetInst()->GetDebugFlag().bWireFrameRenderFlag)
			{
				DrawWireTriangle(vertexArray);
				continue;
			}

			switch (mesh->GetDrawMode())
			{
			case eMeshDrawMode::WIREFRAME: DrawWireTriangle(vertexArray);  break;
			case eMeshDrawMode::VERTEXCOLOR: DrawVertexColorTriangle(vertexArray);  break;
			case eMeshDrawMode::TEXTURE: DrawTextureTriangle(vertexArray, mesh->GetTextureOrNull());  break;
			}
		}

		m_mash3DObjectQueue.Pop();
	}
}
