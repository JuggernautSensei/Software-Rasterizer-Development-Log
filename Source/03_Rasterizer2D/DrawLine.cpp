// 출처: https://powerclabman.tistory.com/144  [그래픽스] 선 그리기 (브레젠험 알고리즘, 코헨-서더랜드 클리핑 알고리즘)

// ===== 블록 1: 브레젠험 (단색) =====
void RenderManager::DrawLine(const Vertex2D& src, const Vertex2D& dst) const
{
	//브레젠험 알고리즘
	ScreenPos startPos = Vector2DToScreenPos(src.Position);
	ScreenPos EndPos = Vector2DToScreenPos(dst.Position);

	int32 w = EndPos.X - startPos.X;
	int32 h = EndPos.Y - startPos.Y;

	//그리고자 하는 직선이 완만한지 급경사인지 판단합니다.
	//true 라면 완만한 경사입니다.
	bool bDrawFlag = MM::Abs(w) >= MM::Abs(h);

	//x값의 변화량입니다.
	//w가 양수라면 양의 방향으로 뻗습니다.
	int32 dx = (w >= 0) ? 1 : -1;
	int32 dy = (h >= 0) ? 1 : -1;

	//판별식에 사용되는 w와 h는 길이입니다. 절댓값을 사용해야 합니다.
	int32 fw = MM::Abs(w);
	int32 fh = MM::Abs(h);

	//f는 판별식, t와 s는 각각 판별식의 변화량입니다.
	int32 f = bDrawFlag ? 2 * fh - fw : 2 * fw - fh;
	int32 t = bDrawFlag ? 2 * fh : 2 * fw;
	int32 s = bDrawFlag ? -2 * fw : -2 * fh;

	int32 curX = startPos.X;
	int32 curY = startPos.Y;

	if (bDrawFlag)
	{
		//완만한 경사일 경우
		while (curX != EndPos.X)
		{
			DrawPixel(ScreenPos(curX, curY), src.Color);

			if (f >= 0)
			{
				//아래로 이동하고, 판별식을 수정
				curY += dy;
				f += s;
			}

			//x는 항상 증가
			curX += dx;
			f += t;
		}
	}
	else
	{
		while (curY != EndPos.Y)
		{
			DrawPixel(ScreenPos(curX, curY), src.Color);

			if (f >= 0)
			{
				curX += dx;
				f += s;
			}

			curY += dy;
			f += t;
		}
	}

	//마지막 점을 그립니다.
	DrawPixel(ScreenPos(curX, curY), src.Color);
}

// ===== 블록 2: 브레젠험 + 색 보간 =====
void RenderManager::DrawLine(const Vertex2D& src, const Vertex2D& dst) const
{
	//브레젠험 알고리즘
	ScreenPos startPos = Vector2DToScreenPos(src.Position);
	ScreenPos EndPos = Vector2DToScreenPos(dst.Position);

	int32 w = EndPos.X - startPos.X;
	int32 h = EndPos.Y - startPos.Y;

	//그리고자 하는 직선이 완만한지 급경사인지 판단합니다.
	//true 라면 완만한 경사입니다.
	bool bDrawFlag = MM::Abs(w) >= MM::Abs(h);

	//x값의 변화량입니다.
	//w가 양수라면 양의 방향으로 뻗습니다.
	int32 dx = (w >= 0) ? 1 : -1;
	int32 dy = (h >= 0) ? 1 : -1;

	//판별식에 사용되는 w와 h는 길이입니다. 절댓값을 사용해야 합니다.
	int32 fw = MM::Abs(w);
	int32 fh = MM::Abs(h);

	//f는 판별식, t와 s는 각각 판별식의 변화량입니다.
	int32 f = bDrawFlag ? 2 * fh - fw : 2 * fw - fh;
	int32 t = bDrawFlag ? 2 * fh : 2 * fw;
	int32 s = bDrawFlag ? -2 * fw : -2 * fh;


	//색 보간 코드
	int32 curX = startPos.X;
	int32 curY = startPos.Y;

	Color32 srcColor = src.Color;
	Color32 dstColor = dst.Color;

	int32 length = bDrawFlag ? fw : fh;

	Color128 curColor = srcColor.ToColor128();
	Color128 step = (dstColor.ToColor128() - srcColor.ToColor128()) / length;

	if (bDrawFlag)
	{
		//완만한 경사일 경우
		while (curX != EndPos.X)
		{
			DrawPixel(ScreenPos(curX, curY),curColor.ToColor32());
			curColor = curColor + step;

			if (f >= 0)
			{
				//아래로 이동하고, 판별식을 수정
				curY += dy;
				f += s;
			}

			//x는 항상 증가
			curX += dx;
			f += t;
		}
	}
	else
	{
		while (curY != EndPos.Y)
		{
			DrawPixel(ScreenPos(curX, curY), curColor.ToColor32());
			curColor = curColor + step;

			if (f >= 0)
			{
				curX += dx;
				f += s;
			}

			curY += dy;
			f += t;
		}
	}

	//마지막 점을 그립니다.
	DrawPixel(ScreenPos(curX, curY), curColor.ToColor32());
}

// ===== 블록 3: 코헨-서더랜드 클리핑 =====
inline bool RenderManager::CohenSutherlandClip(OUT Vector2D& outWorldVectorSrc, OUT Vector2D& outWorldVectorDst)
{
	int32 srcTest = TestRegion(outWorldVectorSrc);
	int32 dstTest = TestRegion(outWorldVectorDst);

	Vector2D minPos = { -m_worldOrigin2D.X,-m_worldOrigin2D.Y };
	Vector2D maxPos = { m_worldOrigin2D.X, m_worldOrigin2D.Y };

	float w = outWorldVectorDst.X - outWorldVectorSrc.X;
	float h = outWorldVectorDst.Y - outWorldVectorSrc.Y;

	while (true)
	{

		if ((srcTest == 0) && (dstTest == 0))
		{
			return true;
		}
		else if (srcTest & dstTest)
		{
			return false;
		}
		else
		{
			bool isSrc = (srcTest != 0); //test값이 0이 아닌 점 선택
			int testBit = isSrc ? srcTest : dstTest;

			Vector2D clipedVecotor = Vector2D::s_zeroVector;

			//새로운 좌표 선택
			if (testBit < 0b0100)
			{
				//X값 선택
				if (testBit & 0b0001)
				{
					clipedVecotor.X = minPos.X;
				}
				else
				{
					clipedVecotor.X = maxPos.X;
				}

				//x축과 평행한 직선인 경우
				if (MM::Abs(h) <= MM::ERROR_BOUND)
				{
					clipedVecotor.Y = outWorldVectorSrc.Y;
				}
				else
				{
					//아닐 경우 직선의 방정식을 활용
					clipedVecotor.Y = outWorldVectorSrc.Y + h * (clipedVecotor.X - outWorldVectorSrc.X) / w;
				}
			}
			else
			{
				//Y값 선택
				if (testBit & 0b0100)
				{
					clipedVecotor.Y = minPos.Y;
				}
				else
				{
					clipedVecotor.Y = maxPos.Y;
				}

				//Y축과 평행한 직선인 경우
				if (MM::Abs(h) <= MM::ERROR_BOUND)
				{
					clipedVecotor.X = outWorldVectorSrc.X;
				}
				else
				{
					//아닐 경우 직선의 방정식을 활용
					clipedVecotor.X = outWorldVectorSrc.X + w * (clipedVecotor.Y - outWorldVectorSrc.Y) / h;
				}
			}

			if (isSrc)
			{
				outWorldVectorSrc = clipedVecotor;
				srcTest = TestRegion(outWorldVectorSrc);
			}
			else
			{
				outWorldVectorDst = clipedVecotor;
				dstTest = TestRegion(outWorldVectorDst);
			}
		}
	}
}
