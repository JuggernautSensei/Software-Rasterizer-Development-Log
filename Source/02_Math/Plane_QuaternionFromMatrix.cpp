// 출처: https://powerclabman.tistory.com/135  [수학] 절두체 : 최적화된 3차원 공간
// (//1, //2 부분은 블로그 원문에서도 비어 있음 - 연습문제 형식)
class Plane
{
public:
	Plane() {}
	Plane(Vector3D point, Vector3D directVector) { DefinePlane(point, directVector); }
	Plane(float a, float b, float c, float d) : normalVector(a, b, c), D(d) {}

	void DefinePlane(Vector3D point, Vector3D directVector)
	{
		//1
	}

	void Normalize()
	{
		//2
	}

	friend std::ostream& operator<<(std::ostream& os, const Plane& plane)
	{
		static const unsigned int BUFFER_MAX = 256;
		char buffer[BUFFER_MAX] = {};
		sprintf_s(buffer, BUFFER_MAX, "법선벡터 (%.2f, %.2f, %.2f)    d : %.2f\n", plane.normalVector.x, plane.normalVector.y, plane.normalVector.z, plane.D);
		os << buffer;
		return os;
	}

	float GetD() const
	{
		return D;
	}

	Vector3D GetNormalVector() const
	{
		return normalVector;
	}

private:
	Vector3D normalVector = {};
	float D = 0.f;
};


// 출처: https://powerclabman.tistory.com/140  [수학] 사원수: 4차원 수로 설계한 3차원 회전
// 회전 행렬 -> 사원수 변환 (발췌)
float root = 0.f;
float trace = InMatrix[0][0] + InMatrix[1][1] + InMatrix[2][2];

root = sqrtf(trace + 1.f);
W = 0.5f * root;
root = 0.5f / root;

X = (InMatrix[1][2] - InMatrix[2][1]) * root;
Y = (InMatrix[2][0] - InMatrix[0][2]) * root;
Z = (InMatrix[0][1] - InMatrix[1][0]) * root;
