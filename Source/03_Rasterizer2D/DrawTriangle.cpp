// 출처: https://powerclabman.tistory.com/149  색칠된 삼각형 그리기
void RenderManager::DrawTriangle(const array<Vertex2D, 3> worldVertex2DList) const
{
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

	leftDown.X = max(0.f, leftDown.X);
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
			float oneMinusST = 1.f - s - t;

			if ((s >= 0 && s <= 1) && (t >= 0 && t <= 1) && (oneMinusST >= 0 && oneMinusST <= 1))
			{
				DrawPixel(pixel, Color32::s_red);
			}
		}
	}
}
