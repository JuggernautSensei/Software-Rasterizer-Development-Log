// 출처: https://powerclabman.tistory.com/156  [그래픽스] 삼각형 클리핑
// 코드 스크린샷(이미지 14~27)을 보고 옮겨 적은 코드

// ----- 이미지 15, 16: 각 평면과 선분의 교점 계산 -----
inline Vertex3D RenderManager::GetVertexFromFrontFace(const Vertex3D& inStartVertex, const Vertex3D& inEndVertex)
{
	Vector4D v1 = inStartVertex.Position;
	Vector4D v2 = inEndVertex.Position;

	float p1 = v2.W + v2.Z;
	float p2 = v1.W + v1.Z;

	float t = p1 / (p1 - p2);

	return Vertex3D{
			Vector4D{inStartVertex.Position * t + inEndVertex.Position * (1 - t)},
			Color32{inStartVertex.Color * t + inEndVertex.Color * (1 - t)},
			Vector2D{inStartVertex.UV * t + inEndVertex.UV * (1 - t)},
	};
}

inline Vertex3D RenderManager::GetVertexFromBackFace(const Vertex3D& inStartVertex, const Vertex3D& inEndVertex)
{
	Vector4D v1 = inStartVertex.Position;
	Vector4D v2 = inEndVertex.Position;

	float p1 = v2.W - v2.Z;
	float p2 = v1.W - v1.Z;

	float t = p1 / (p1 - p2);

	return Vertex3D{
			Vector4D{inStartVertex.Position * t + inEndVertex.Position * (1 - t)},
			Color32{inStartVertex.Color * t + inEndVertex.Color * (1 - t)},
			Vector2D{inStartVertex.UV * t + inEndVertex.UV * (1 - t)},
	};
}

inline Vertex3D RenderManager::GetVertexFromRightFace(const Vertex3D& inStartVertex, const Vertex3D& inEndVertex)
{
	Vector4D v1 = inStartVertex.Position;
	Vector4D v2 = inEndVertex.Position;

	float p1 = v2.W - v2.X;
	float p2 = v1.W - v1.X;

	float t = p1 / (p1 - p2);

	return Vertex3D{
			Vector4D{inStartVertex.Position * t + inEndVertex.Position * (1 - t)},
			Color32{inStartVertex.Color * t + inEndVertex.Color * (1 - t)},
			Vector2D{inStartVertex.UV * t + inEndVertex.UV * (1 - t)},
	};
}

inline Vertex3D RenderManager::GetVertexFromLeftFace(const Vertex3D& inStartVertex, const Vertex3D& inEndVertex)
{
	Vector4D v1 = inStartVertex.Position;
	Vector4D v2 = inEndVertex.Position;

	float p1 = v2.W + v2.X;
	float p2 = v1.W + v1.X;

	float t = p1 / (p1 - p2);

	return Vertex3D{
			Vector4D{inStartVertex.Position * t + inEndVertex.Position * (1 - t)},
			Color32{inStartVertex.Color * t + inEndVertex.Color * (1 - t)},
			Vector2D{inStartVertex.UV * t + inEndVertex.UV * (1 - t)},
	};
}

inline Vertex3D RenderManager::GetVertexFromTopFace(const Vertex3D& inStartVertex, const Vertex3D& inEndVertex)
{
	Vector4D v1 = inStartVertex.Position;
	Vector4D v2 = inEndVertex.Position;

	float p1 = v2.W - v2.Y;
	float p2 = v1.W - v1.Y;

	float t = p1 / (p1 - p2);

	return Vertex3D{
			Vector4D{inStartVertex.Position * t + inEndVertex.Position * (1 - t)},
			Color32{inStartVertex.Color * t + inEndVertex.Color * (1 - t)},
			Vector2D{inStartVertex.UV * t + inEndVertex.UV * (1 - t)},
	};
}

inline Vertex3D RenderManager::GetVertexFromBottomFace(const Vertex3D& inStartVertex, const Vertex3D& inEndVertex)
{
	Vector4D v1 = inStartVertex.Position;
	Vector4D v2 = inEndVertex.Position;

	float p1 = v2.W + v2.Y;
	float p2 = v1.W + v1.Y;

	float t = p1 / (p1 - p2);

	return Vertex3D{
			Vector4D{inStartVertex.Position * t + inEndVertex.Position * (1 - t)},
			Color32{inStartVertex.Color * t + inEndVertex.Color * (1 - t)},
			Vector2D{inStartVertex.UV * t + inEndVertex.UV * (1 - t)},
	};
}

// ----- 이미지 18: 평면 외부 판정 -----
//삼각형 클리핑 테스트 함수
inline bool RenderManager::TestToFrontFace(const Vector4D& inVector)
{
	return -inVector.W > inVector.Z;
}

inline bool RenderManager::TestToBackFace(const Vector4D& inVector)
{
	return inVector.W < inVector.Z;
}

inline bool RenderManager::TestToLeftFace(const Vector4D& inVector)
{
	return -inVector.W > inVector.X;
}

inline bool RenderManager::TestToRightFace(const Vector4D& inVector)
{
	return inVector.W < inVector.X;
}

inline bool RenderManager::TestToBottomFace(const Vector4D& inVector)
{
	return -inVector.W > inVector.Y;
}

inline bool RenderManager::TestToTopFace(const Vector4D& inVector)
{
	return inVector.W < inVector.Y;
}

// ----- 이미지 22~24: TestAndClip -----
bool RenderManager::TestAndClip(
	VertexVector& outInVertices,
	const std::function<bool(const Vector4D&)>& inTestFuntion,
	const std::function<Vertex3D(const Vertex3D&, const Vertex3D&)>& inClippingFunction)
{
	MC::FixedVector<uint32, 3> externalPointIdx = {};
	MC::FixedVector<uint32, 3> internalPointIdx = {};

	//테스트
	for (uint32 i = 0; i < 3; i++)
	{
		const Vector4D& vector = outInVertices[i].Position;
		if (inTestFuntion(vector))
		{
			externalPointIdx.PushBack(i);
		}
		else
		{
			internalPointIdx.PushBack(i);
		}
	}

	size_t cnt = externalPointIdx.Size();

	if (cnt == 1)
	{
		//한 점이 외부에 있을 경우
		//두 개의 삼각형으로 분할해야함
		uint32 inIdx1 = internalPointIdx[0];
		uint32 inIdx2 = internalPointIdx[1];
		uint32 outIdx = externalPointIdx[0];

		const Vertex3D& inVertex1 = outInVertices[inIdx1];
		const Vertex3D& inVertex2 = outInVertices[inIdx2];
		const Vertex3D& outVertex = outInVertices[outIdx];

		//새로운 정점 계산
		Vertex3D clippedVertex1 = inClippingFunction(inVertex1, outVertex);
		Vertex3D clippedVertex2 = inClippingFunction(inVertex2, outVertex);

		outInVertices[outIdx] = clippedVertex1;

		outInVertices.PushBack(inVertex2);
		outInVertices.PushBack(clippedVertex1);
		outInVertices.PushBack(clippedVertex2);

	}
	else if (cnt == 2)
	{
		//두 점이 외부에 있을 경우
		//삼각형을 축소해야 함

		uint32 outIdx1 = externalPointIdx[0];
		uint32 outIdx2 = externalPointIdx[1];
		uint32 inIdx = internalPointIdx[0];

		const Vertex3D& outVertex1 = outInVertices[outIdx1];
		const Vertex3D& outVertex2 = outInVertices[outIdx2];
		const Vertex3D& inVertex = outInVertices[inIdx];

		//새로운 정점 계산
		Vertex3D clippedVertex1 = inClippingFunction(inVertex, outVertex1);
		Vertex3D clippedVertex2 = inClippingFunction(inVertex, outVertex2);

		outInVertices[outIdx1] = clippedVertex1;
		outInVertices[outIdx2] = clippedVertex2;
	}
	else  if (cnt == 3)
	{
		return false;
	}

	return true;
}

// ----- 이미지 25: 삼각형 클리핑 본체 -----
//삼각형 클리핑 본체
inline bool RenderManager::TriangleClipping(VertexVector& outInVertices)
{
	size_t len = s_clippingTestFunc.size();
	for (size_t i = 0; i < len; ++i)
	{
		//만약 어떤 면에 대해서, 세 점이 모두 밖에 위치할 경우 그리기 스킵
		if (TestAndClip(outInVertices, s_clippingTestFunc[i], s_clippingFunc[i]) == false)
		{
			return false;
		}
	}

	return true;
}

// ----- 이미지 26 (아래쪽 일부는 이미지에서 잘림) -----
const array<std::function<bool(const Vector4D&)>, 6> RenderManager::s_clippingTestFunc = {
	TestToFrontFace,
	TestToBackFace,
	TestToLeftFace,
	TestToRightFace,
	TestToTopFace,
	TestToBottomFace,
};

const array<std::function<Vertex3D(const Vertex3D&, const Vertex3D&)>, 6> RenderManager::s_clippingFunc = {
	GetVertexFromFrontFace,
	GetVertexFromBackFace,
	GetVertexFromLeftFace,
	GetVertexFromRightFace,
	GetVertexFromTopFace,
	GetVertexFromBottomFace,   // (이미지 잘림 - 순서상 추정)
};

// ----- 이미지 27: Render3D에서 사용 (일부) -----
		//정점 처리
		VertexShader3D(vBuffer, finalMatrix);

		for (const auto& idx : iBuffer)
		{
			//정점 집합 정의
			VertexVector vertexList = { vBuffer[idx.First] , vBuffer[idx.Second],  vBuffer[idx.Third] };

			//클리핑 테스트 및 클리핑 수행
			if (TriangleClipping(vertexList) == false)
			{
				continue;
			}

			//해상도에 맞게 보정 및 NDC 좌표 변환
			size_t vertexListSize = static_cast<uint32>(vertexList.Size());

			for (int32 i = 0; i < vertexListSize; ++i)
			{
				Vertex3D& vertex = vertexList[i];

				//해상도 보정
				vertex.Position.X *= m_screenSize.width * 0.5f;
				vertex.Position.Y *= m_screenSize.height * 0.5f;
				// ... (이미지 잘림)
