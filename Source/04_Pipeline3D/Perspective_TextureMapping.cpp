// 출처: https://powerclabman.tistory.com/153  [그래픽스] 3D 카메라 설계 수정, 원근법 구현
// 코드 스크린샷을 보고 옮겨 적은 코드 (이미지 6, 28, 34는 다른 이미지와 중복이라 생략)

// ----- 이미지 0: Camera3D (수정 버전 선언) -----
class Camera3D
{
public:
	Camera3D() = default;
	~Camera3D() = default;

	const Transformer3D& GetTransfomer3D() const;
	Transformer3D& GetTransfomer3DRef();

	// 뷰 행렬
	Matrix4x4 GetViewMatrix() const
	{
		// 크기 변환
		float sx = m_zoom;
		float sy = m_zoom;
		float sz = m_zoom;

		// 로컬 기저 축 (회전변환)
		Vector3D xx = m_trans.GetLocalBasisRight();
		Vector3D yx = m_trans.GetLocalBasisUp();
		Vector3D zx = m_trans.GetLocalBasisOut();

		// 이동 변환
		Vector3D t = m_trans.GetPosition();

		return Matrix4x4{
			{xx.X * sx, xx.Y * sy, xx.Z * sz, -t.X * sx},
			{yx.X * sx,  yx.Y * sy,  yx.Z * sz, -t.Y * sy},
			{zx.X * sx, zx.Y * sy, zx.Z * sz, -t.Z * sz},
			{ 0.f,        0.f,        0.f,       1.f}
		};
	}

	void SetZoom(float zoom);
	float GetZoom() const;

	void AddZoom(float zoom);
	Vector3D GetCameraLook() const;

	// ----- 이미지 17: 화각 / 초점거리 -----
	//초점거리 정의
	void SetViewAngle(float degree)
	{
		m_viewAngle = degree; //육십도법으로 화각 정의

		Assert(degree != 0.f, "View Angle Is Zero.");

		m_distance = 1.f / MM::Tan(m_viewAngle * 0.5f)
	}

private:
	Transformer3D m_trans;
	float m_zoom = 1.f;

	float m_viewAngle = 0.f; //화각
	float m_distance = 0.f; //초점거리
};

// ----- 이미지 4: 뷰 행렬 (좌표계 수정 버전) -----
// 뷰 행렬
Matrix4x4 GetViewMatrix() const
{
	// 크기 변환
	float sx = m_zoom;
	float sy = m_zoom;
	float sz = m_zoom;

	// 로컬 기저 축 (회전변환)
	Vector3D xx = m_trans.GetLocalBasisRight();
	Vector3D yx = m_trans.GetLocalBasisUp();
	Vector3D zx = m_trans.GetLocalBasisOut();

	// 이동 변환
	Vector3D t = m_trans.GetPosition();

	return Matrix4x4{
		{-xx.X * sx, -xx.Y * sy, -xx.Z * sz, t.X * sx},
		{ yx.X * sx,  yx.Y * sy,  yx.Z * sz, -t.Y * sy},
		{-zx.X * sx, -zx.Y * sy, -zx.Z * sz, t.Z * sz},
		{ 0.f,        0.f,        0.f,       1.f}
	};
}

// ----- 이미지 18: 원근 투영 행렬 (종횡비 고려 X) -----
//원근 투영 행렬
Matrix4x4 GetPerspective() const
{
	float d = m_distance;

	return Matrix4x4{
		{-d, 0, 0, 0 },
		{0, -d, 0, 0 },
		{0, 0, -1, 0 },
		{0, 0, 0,  1 }
	};
}

// ----- 이미지 27: 원근 투영 행렬 (종횡비 고려 O) -----
//원근 투영 행렬
Matrix4x4 GetPerspectiveMatrix(const ScreenSize& screenSize) const
{
	float invA = screenSize.height / static_cast<float>(screenSize.width);
	float d = m_distance;

	return Matrix4x4({
		{-d * invA, 0.f, 0.f, 0.f },
		{0.f, -d, 0.f, 0.f },
		{0.f, 0.f, -1.f, 0.f },
		{0.f, 0.f, 0.f,  1.f }
		});
}

// ----- 이미지 19: Render3D 정점 변환부 -----
		//정점 변환
		const Transformer3D* trans = obj->GetTransformer();

		const Matrix4x4& viewMat = m_camera3D.GetViewMatrix();
		const Matrix4x4& perspectiveMat = m_camera3D.GetPerspectiveMatrix();
		Matrix4x4 finalMatrix = perspectiveMat * viewMat * trans->GetModelMatrix();

		VertexShader3D(vBuffer, finalMatrix);

// ----- 이미지 20: VertexShader3D (원근 나눗셈) -----
inline void VertexShader3D(vector<Vertex3D>& vertexBuffer, const Matrix4x4& finalMatrix)
{
	for (Vertex3D& vertex : vertexBuffer)
	{
		vertex.Position = finalMatrix * vertex.Position;

		//만약 Z값이 0.f 일 경우 보정
		if (vertex.Position.Z == 0.f)
		{
			vertex.Position.Z = MM::SMALL_NUM;
		}

		//클립 좌표 -> NDC 좌표 변환
		float invZ = 1.f / vertex.Position.Z;
		vertex.Position.X *= invZ;
		vertex.Position.Y *= invZ;
		vertex.Position.Z *= invZ;
	}
}

// ----- 이미지 22: NDC -> 스크린 크기로 늘리기 -----
		//정점 처리
		for (const MeshIndex& idx : iBuffer)
		{
			const Vertex3D& fst = vBuffer[idx.First];
			const Vertex3D& sec = vBuffer[idx.Second];
			const Vertex3D& trd = vBuffer[idx.Third];

			//백페이스 컬링
			if (IsBackFace(fst.Position, sec.Position, trd.Position))
			{
				continue;
			}

			array<Vertex2D, 3> vertexArray = { fst.ToVertex2D(),sec.ToVertex2D(),trd.ToVertex2D() };

			//늘리기
			for (auto& vertex : vertexArray)
			{
				vertex.Position.X *= m_screenSize.width * 0.5f;
				vertex.Position.Y *= m_screenSize.height * 0.5f;
			}

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

// ----- 이미지 29: Texture -----
class Texture
{
public:
	Texture() = default;
	~Texture() = default;

	inline void InitTexture(HDC srcHdc, const VecInt2D& size)
	{
		m_size = size;
		m_pixelBuffer.resize(static_cast<size_t>(m_size.X) * m_size.Y, Color32::s_black);

		for (int32 iy = 0; iy < m_size.Y; ++iy)
		{
			int32 rowIndex = m_size.X * iy;
			for (int32 ix = 0; ix < m_size.X; ++ix)
			{
				int32 idx = rowIndex + ix;
				Color32 c(::GetPixel(srcHdc, ix, iy));
				m_pixelBuffer[idx] = c;
			}
		}
	}

	inline Color32 GetColor32(const Vector2D& UV) const
	{
		int32 ix = static_cast<int32>(UV.X * m_size.X);
		int32 iy = static_cast<int32>(UV.Y * m_size.Y) ;

		if (ix < 0 || ix >= m_size.X || iy < 0 || iy >= m_size.Y)
		{
			return Color32::s_black;
		}

		int32 idx = m_size.X * (m_size.Y - iy - 1) + ix;

		return m_pixelBuffer[idx];
	}

private:
	VecInt2D m_size;

	vector<Color32> m_pixelBuffer;
};

// ----- 이미지 32: 텍스처 UV가 있는 육면체 -----
	//전면
	mesh->AddVertex(Vertex3D{ Vector3D{0.5f, 0.5f, 0.5f},   Vector2D(1.f,1.f) });
	mesh->AddVertex(Vertex3D{ Vector3D{-0.5f, 0.5f, 0.5f},  Vector2D(0.5f,1.f) });
	mesh->AddVertex(Vertex3D{ Vector3D{-0.5f, -0.5f, 0.5f}, Vector2D(0.5f,0.5f) });
	mesh->AddVertex(Vertex3D{ Vector3D{0.5f, -0.5f, 0.5f},  Vector2D(1.f,0.5f) });

	//후면
	mesh->AddVertex(Vertex3D{ Vector3D{0.5f, 0.5f, -0.5f},  Vector2D(1.f,1.f) });
	mesh->AddVertex(Vertex3D{ Vector3D{-0.5f, 0.5f, -0.5f}, Vector2D(0.5f,1.f) });
	mesh->AddVertex(Vertex3D{ Vector3D{-0.5f, -0.5f, -0.5f},Vector2D(0.5f,0.5f) });
	mesh->AddVertex(Vertex3D{ Vector3D{0.5f, -0.5f, -0.5f}, Vector2D(1.f,0.5f) });

	//우측면
	mesh->AddVertex(Vertex3D{ Vector3D{0.5f, 0.5f, 0.5f},   Vector2D(1.f,1.f) });
	mesh->AddVertex(Vertex3D{ Vector3D{0.5f, -0.5f, 0.5f},  Vector2D(0.5f,1.f) });
	mesh->AddVertex(Vertex3D{ Vector3D{0.5f, -0.5f, -0.5f}, Vector2D(0.5f,0.5f) });
	mesh->AddVertex(Vertex3D{ Vector3D{0.5f, 0.5f, -0.5f},  Vector2D(1.f,0.5f) });

	//좌측면
	mesh->AddVertex(Vertex3D{ Vector3D{-0.5f, 0.5f, 0.5f},   Vector2D(1.f,1.f) });
	mesh->AddVertex(Vertex3D{ Vector3D{-0.5f, -0.5f, 0.5f},  Vector2D(0.5f,1.f) });
	mesh->AddVertex(Vertex3D{ Vector3D{-0.5f, -0.5f, -0.5f}, Vector2D(0.5f,0.5f) });
	mesh->AddVertex(Vertex3D{ Vector3D{-0.5f, 0.5f, -0.5f},  Vector2D(1.f,0.5f) });

	//윗면
	mesh->AddVertex(Vertex3D{ Vector3D{0.5f, 0.5f, -0.5f},  Vector2D(0.5f, 1.f) });
	mesh->AddVertex(Vertex3D{ Vector3D{-0.5f, 0.5f, -0.5f}, Vector2D(0.f, 1.f) });
	mesh->AddVertex(Vertex3D{ Vector3D{-0.5f, 0.5f, 0.5f},  Vector2D(0.f, 0.5f) });
	mesh->AddVertex(Vertex3D{ Vector3D{0.5f, 0.5f, 0.5f},   Vector2D(0.5f, 0.5f) });

	//아랫면
	mesh->AddVertex(Vertex3D{ Vector3D{0.5f, -0.5f, 0.5f},  Vector2D(0.5f,0.5f) });
	mesh->AddVertex(Vertex3D{ Vector3D{-0.5f, -0.5f, 0.5f}, Vector2D(0.f,0.5f) });
	mesh->AddVertex(Vertex3D{ Vector3D{-0.5f, -0.5f, -0.5f},Vector2D(0.f,0.f) });
	mesh->AddVertex(Vertex3D{ Vector3D{0.5f, -0.5f, -0.5f}, Vector2D(0.5f,0.f) });

	//전면
	mesh->AddIndex(MeshIndex{ 0, 1, 2 });
	mesh->AddIndex(MeshIndex{ 2, 3, 0 });

	//후면
	mesh->AddIndex(MeshIndex{ 4, 7, 6 });
	mesh->AddIndex(MeshIndex{ 6, 5, 4 });

	//우측면
	mesh->AddIndex(MeshIndex{ 8, 9, 10 });
	mesh->AddIndex(MeshIndex{ 10, 11, 8 });

	//좌측면
	mesh->AddIndex(MeshIndex{ 14, 13, 12 });
	mesh->AddIndex(MeshIndex{ 12, 15, 14 });

	//윗면
	mesh->AddIndex(MeshIndex{ 16, 17, 18 });
	mesh->AddIndex(MeshIndex{ 18, 19, 16 });

	//아랫면
	mesh->AddIndex(MeshIndex{ 22, 23, 20 });
	mesh->AddIndex(MeshIndex{ 20, 21, 22 });

	m_trans->SetPosition(Vector3D(0, 0, 3.f));
	m_trans->SetRotation(EulerAngle(0, 0, 180.f));
}

// ----- 이미지 33: DrawTextureTriangle (원근 보정 X) -----
void RenderManager::DrawTextureTriangle(array<Vertex2D, 3> worldVertex2DList, const Texture* texture) const
{
	Assert(texture != nullptr, L"DrawTextureTriangle Is Nullptr.");

	// 벡터 u, v
	Vector2D u = worldVertex2DList[1].Position - worldVertex2DList[0].Position;
	Vector2D v = worldVertex2DList[2].Position - worldVertex2DList[0].Position;

	//분모
	float uDotv = u.Dot(v);
	float uDotu = u.Dot(u);
	float vDotv = v.Dot(v);

	float denominator = uDotv * uDotv - uDotu * vDotv;

	// 퇴화 삼각형이면 그리기 생략
	if (MM::Abs(denominator) <= MM::ERROR_BOUND)
	{
		return;
	}

	float invDenominator = 1.f / denominator;

	// 삼각형의 최소 및 최대 좌표 계산
	Vector2D minPos = Vector2D::s_zeroVector;
	Vector2D maxPos = Vector2D::s_zeroVector;

	MM::FindMinMax(minPos.X, maxPos.X, { worldVertex2DList[0].Position.X, worldVertex2DList[1].Position.X, worldVertex2DList[2].Position.X });
	MM::FindMinMax(minPos.Y, maxPos.Y, { worldVertex2DList[0].Position.Y, worldVertex2DList[1].Position.Y, worldVertex2DList[2].Position.Y });

	// 스크린 좌표로 변환 후 클리핑
	ScreenPos leftDown = WorldVector2DToScreenPos(minPos);
	ScreenPos rightUp = WorldVector2DToScreenPos(maxPos);

	leftDown.X = max(0, leftDown.X);
	leftDown.Y = min(m_screenSize.height, leftDown.Y);
	rightUp.X = min(m_screenSize.width, rightUp.X);
	rightUp.Y = max(0, rightUp.Y);

	// 삼각형 영역 내 모든 점을 검사하고 색칠
	for (int32 iy = rightUp.Y; iy <= leftDown.Y; ++iy)
	{
		for (int32 ix = leftDown.X; ix <= rightUp.X; ++ix)
		{
			ScreenPos pixel(ix, iy);

			Vector2D toWorldVector = ScreenPosToWorldVector2D(pixel);
			Vector2D w = toWorldVector - worldVertex2DList[0].Position;

			float wDotv = w.Dot(v);
			float wDotu = w.Dot(u);

			float s = (wDotv * uDotv - wDotu * vDotv) * invDenominator;
			float t = (wDotu * uDotv - wDotv * uDotu) * invDenominator;
			float left = 1.f - s - t;

			if ((s >= 0 && s <= 1) && (t >= 0 && t <= 1) && (left >= 0 && left <= 1))
			{
				const Vector2D& fst = worldVertex2DList[0].UV;
				const Vector2D& sec = worldVertex2DList[1].UV;
				const Vector2D& trd = worldVertex2DList[2].UV;

				Vector2D finalUV = fst * left + sec * s + trd * t;

				Color32 finalColor = texture->GetColor32(finalUV);

				DrawPixel(pixel, finalColor);
			}
		}
	}
}

// ----- 이미지 46: 원근 보정 텍스처 매핑 (DrawTextureTriangle 후반부) -----
	// 스크린 좌표로 변환 후 클리핑
	ScreenPos leftDown = WorldVector2DToScreenPos(minPos);
	ScreenPos rightUp = WorldVector2DToScreenPos(maxPos);

	leftDown.X = max(0, leftDown.X);
	leftDown.Y = min(m_screenSize.height, leftDown.Y);
	rightUp.X = min(m_screenSize.width, rightUp.X);
	rightUp.Y = max(0, rightUp.Y);

	//원근 보정을 위한 변수
	float z0Inv = 1 / worldVertex3DList[0].Position.W;
	float z1Inv = 1 / worldVertex3DList[1].Position.W;
	float z2Inv = 1 / worldVertex3DList[2].Position.W;

	// 삼각형 영역 내 모든 점을 검사하고 색칠
	for (int32 iy = rightUp.Y; iy <= leftDown.Y; ++iy)
	{
		for (int32 ix = leftDown.X; ix <= rightUp.X; ++ix)
		{
			ScreenPos pixel(ix, iy);

			Vector2D toWorldVector = ScreenPosToWorldVector2D(pixel);
			Vector2D w = toWorldVector - worldVertex3DList[0].Position.ToVector2D();

			float wDotv = w.Dot(v);
			float wDotu = w.Dot(u);

			float s = (wDotv * uDotv - wDotu * vDotv) * invDenominator;
			float t = (wDotu * uDotv - wDotv * uDotu) * invDenominator;
			float left = 1.f - s - t;

			if ((s >= 0 && s <= 1) && (t >= 0 && t <= 1) && (left >= 0 && left <= 1))
			{
				// 뷰 공간의 무게중심 계산
				float z = 1.f / (z0Inv * left  + z1Inv * s + z2Inv * t);

				float newS = s * z1Inv * z;
				float newT = t * z2Inv * z;
				float newLeft = left * z0Inv * z;

				const Vector2D& fst = worldVertex3DList[0].UV;
				const Vector2D& sec = worldVertex3DList[1].UV;
				const Vector2D& trd = worldVertex3DList[2].UV;

				//최종 UV 좌표 결정
				Vector2D finalUV = fst * newLeft + sec * newS + trd * newT;

				Color32 finalColor = texture->GetColor32(finalUV);

				DrawPixel(pixel, finalColor);
			}
		}
	}
}
