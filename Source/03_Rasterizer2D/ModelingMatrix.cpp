// 출처: https://powerclabman.tistory.com/145  [그래픽스] 모델링 행렬 설계
// 코드 스크린샷(이미지 21~28)을 보고 옮겨 적은 코드

// ----- 이미지 21: Transformer 멤버 (초기 버전) -----
private:
	//이동
	Vector2D m_position = Vector2D::s_zeroVector;

	//각도
	float m_degreeRotation = 0.f;

	//크기
	Vector2D m_scale = Vector2D{ 0.f,0.f };

	//로컬 기저 벡터
	Vector2D m_localBasisX = Vector2D::s_basisVectorX;
	Vector2D m_localBasisY = Vector2D::s_basisVectorY;
};

// ----- 이미지 22: Transformer 멤버 (수정 버전) -----
private:
	//이동
	Vector2D m_position = Vector2D::s_zeroVector;

	//각도
	float m_degreeRotation = 0.f;

	//크기
	Vector2D m_scale = Vector2D{ 0.f,0.f };

	//로컬 기저 벡터
	Vector2D m_localBasisRight = Vector2D::s_basisVectorX;
	Vector2D m_localBasisUp = Vector2D::s_basisVectorY;
};

// ----- 이미지 23: Transformer 선언 (이미지가 여기서 잘려 있음) -----
class Transformer : public Component
{
	using Super = Component;
public:
	inline Transformer() = default;
	virtual ~Transformer() = default;

	void Init() override;
	void Update() override;
	void Render() override;

	inline void SetRotation(float degree);
	inline void AddRotation(float degree);
	inline float GetRotation() const;

	inline void SetPosition(const Vector2D& position);
	inline void AddPosition(const Vector2D& position);
	inline Vector2D GetPosition() const;

	inline void SetScale(const Vector2D& scale);
	inline Vector2D GetScale() const;

	inline Vector2D GetLocalBasisRight() const;
	inline Vector2D GetLocalBasisUp() const;

	inline Matrix3x3 GetModelMatrix() const;

private:
	void UpdateLocalBasis();
	// ... (이하 이미지 잘림 - 멤버는 이미지 22 참고)

// ----- 이미지 24 -----
inline Matrix3x3 Transformer::GetModelMatrix() const
{
	return Matrix3x3{
		{m_scale.X * m_localBasisRight.X,   m_scale.Y * m_localBasisUp.X, m_position.X},
		{m_scale.X * m_localBasisRight.Y,   m_scale.Y * m_localBasisUp.Y, m_position.Y},
		{0.f,0.f,1.f}
	};
}

// ----- 이미지 25 -----
void RenderManager::Render()
{
	while (m_renderingQueue.IsEmpty() == false)
	{
		const RenderingObject* obj = m_renderingQueue.Front();

		Wireframe* wf = obj->GetWireframe();

		vector<Vertex2D> vBuffer = wf->GetVertexBuffer();
		const vector<Index>& iBuffer = wf->GetIndexBuffer();

		//정점 변환
		const Transformer* trans = obj->GetTransformer();

		const Matrix3x3& modelMatrix = trans->GetModelMatrix();

		VertexShader2D(vBuffer, modelMatrix);

		//정점 처리
		for (const Index& idx : iBuffer)
		{
			const Vertex2D& srcVertex = vBuffer[idx.Src];
			const Vertex2D& dstVertex = vBuffer[idx.Dst];

			DrawLine(srcVertex, dstVertex);
		}

		m_renderingQueue.Pop();
	}

	SwapChain();
}

// ----- 이미지 26 -----
inline void VertexShader2D(vector<Vertex2D>& vertexBuffer, const Matrix3x3& modelMatrix)
{
	for (Vertex2D& vertex : vertexBuffer)
	{
		vertex.Position = modelMatrix * vertex.Position;
	}
}

// ----- 이미지 27 -----
inline constexpr Vector2D Matrix3x3::operator*(const Vector2D& other) const
{
	Vector3D tmp = other.ToAffinePoint();
	return (*this * tmp).ToVector2D();
}

// ----- 이미지 28 -----
inline constexpr Vector3D Vector2D::ToAffinePoint() const
{
	return Vector3D(X, Y, 1);
}
