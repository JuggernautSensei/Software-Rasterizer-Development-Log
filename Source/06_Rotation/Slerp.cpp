// 출처: https://powerclabman.tistory.com/159  [그래픽스] 구형 선형 보간으로 회전 보간
// 코드 스크린샷(이미지 9)을 보고 옮겨 적은 코드
inline constexpr Quaternion Quaternion::Slerp(const Quaternion& inQuaternion1, const Quaternion& inQuaternion2, float ratio, bool useShortAngle)
{
	Quaternion q1 = inQuaternion1;
	Quaternion q2 = inQuaternion2;

	// 내적값으로 긴 각 짧은 각 판단
	float dot = q1.X * q2.X + q1.Y * q2.Y + q1.Z * q2.Z + q1.W * q2.W;

	// 각 선택
	if ((useShortAngle && dot < 0.f) || (!useShortAngle && dot > 0.f))
	{
		q1 = -q1;
		dot = -dot;
	}

	float t1, t2;

	// 만약 두 쿼터니언이 거의 비슷한 각인 경우
	if (dot > 0.9995f)
	{
		// 빠르게 선형으로 보간
		t1 = 1.f - ratio;
		t2 = ratio;
	}
	else
	{
		float theta = ::acosf(dot);
		float sinInv = 1.f / ::sinf(theta);

		t1 = ::sinf((1 - ratio) * theta) * sinInv;
		t2 = ::sinf(ratio * theta) * sinInv;
	}

	Quaternion q;
	q.W = q1.W * t1 + q2.W * t2;
	q.X = q1.X * t1 + q2.X * t2;
	q.Y = q1.Y * t1 + q2.Y * t2;
	q.Z = q1.Z * t1 + q2.Z * t2;

	return q;
}
