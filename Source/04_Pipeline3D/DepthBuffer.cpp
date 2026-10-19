// 출처: https://powerclabman.tistory.com/154  [그래픽스] 깊이 구현
// 코드 스크린샷(이미지 13~20)을 보고 옮겨 적은 코드 (이미지 15는 디버거 화면이라 생략)

// ----- 이미지 13: Camera3D 선언 -----
class Camera3D
{
public:
	Camera3D() { SetViewAngle(45.f); };
	~Camera3D() = default;

	// 뷰 행렬
	Matrix4x4 GetViewMatrix() const;
	Matrix4x4 GetRotateMatrix() const;

	inline Transformer3D GetTransfomer3D();
	inline Transformer3D GetTransfomer3D() const;
	inline Transformer3D& GetTransfomer3DRef();

	//초점거리 정의
	inline void SetViewAngle(float degree);

	//원근 투영 행렬
	inline Matrix4x4 GetPerspectiveMatrix(const ScreenSize& screenSize) const;

	// 카메라 시선 설정
	// 카메라가 바라보는 지점을 정합니다.
	inline void SetCameraLookAt(const Vector3D& point, bool isCameraFlipped = false);

	inline Vector3D GetViewSpaceRight();
	inline Vector3D GetViewSpaceOut();
	inline Vector3D GetViewSpaceUp();

	inline void SetViewVolume(float nearPlaneDist, float farPlaneDist);

private:
	Transformer3D m_trans;

	float m_viewAngle = 0.f; //화각
	float m_distance = 0.f; //초점거리

	float m_nearPlaneDistance = 1.f; //근평면
	float m_farPlaneDistance = 1000.f; //원평면
};

// ----- 이미지 14: 근평면/원평면을 반영한 원근 투영 행렬 -----
//원근 투영 행렬
inline Matrix4x4 Camera3D::GetPerspectiveMatrix(const ScreenSize& screenSize) const
{
	float invA = screenSize.height / static_cast<float>(screenSize.width);
	float d = m_distance;

	float nfInv = 1.f / (m_nearPlaneDistance - m_farPlaneDistance);
	float e1 = (m_nearPlaneDistance + m_farPlaneDistance) * nfInv;
	float e2 = (2 * m_nearPlaneDistance * m_farPlaneDistance) * nfInv;

	return Matrix4x4({
		{ d * invA, 0.f, 0.f, 0.f },
		{ 0.f, d, 0.f, 0.f },
		{ 0.f, 0.f, e1, e2 },
		{ 0.f, 0.f, -1.f,  0.f }
		});
}

// ----- 이미지 16 -----
	// 깊이 버퍼 초기화
	m_depthBuffer = new float[m_screenSize.width * m_screenSize.height];
	ClearDepthBuffer();

// ----- 이미지 17 -----
//깊이 버퍼 초기화
inline void ClearDepthBuffer()
{
	UINT32 totalCount = m_screenSize.height * m_screenSize.width;
	std::fill(m_depthBuffer, m_depthBuffer + totalCount, INFINITY);
}

// ----- 이미지 18 -----
//깊이 테스트
inline bool DepthTest(const ScreenPos& point, float depth)
{
	float* depthPtr = &m_depthBuffer[point.Y * m_screenSize.width + point.X];

	// 깊이 테스팅 통과
	if (*depthPtr > depth)
	{
		*depthPtr = depth;

		return true;
	}

	// 깊이 테스팅 실패
	return false;
}

// ----- 이미지 19: DrawTextureTriangle 내부 (깊이 테스트 추가) -----
			if ((s >= 0 && s <= 1) && (t >= 0 && t <= 1) && (left >= 0 && left <= 1))
			{
				// 뷰 공간의 무게중심 계산
				float z = 1.f / (z0Inv * left + z1Inv * s + z2Inv * t);

				float newS = s * z1Inv * z;
				float newT = t * z2Inv * z;
				float newLeft = left * z0Inv * z;

				// 깊이 테스트

				// 깊이 보간
				float newDepth = worldVertex3DList[0].Position.Z * newLeft + worldVertex3DList[1].Position.Z * newS +
					worldVertex3DList[2].Position.Z * newT;

				if (DepthTest(pixel, newDepth) == false)
				{
					//  테스트 실패
					continue;
				}

				const Vector2D& fst = worldVertex3DList[0].UV;
				const Vector2D& sec = worldVertex3DList[1].UV;
				const Vector2D& trd = worldVertex3DList[2].UV;

				//최종 UV 좌표 결정
				Vector2D finalUV = fst * newLeft + sec * newS + trd * newT;

				Color32 finalColor = texture->GetColor32(finalUV);

				DrawPixel(pixel, finalColor);
			}

// ----- 이미지 20 -----
inline void RenderManager::SwapChain()
{
	::BitBlt(m_hdc, 0, 0, m_screenSize.width, m_screenSize.height, m_backHdc, 0, 0, SRCCOPY);
	::PatBlt(m_backHdc, 0, 0, m_screenSize.width, m_screenSize.height, WHITENESS);

	//깊이 값 초기화
	ClearDepthBuffer();
}
