// 출처: https://powerclabman.tistory.com/129  [수학] 외적: 3차원 공간의 분석과 응용 (정답 코드 블록)
#include <iostream>
using namespace std;
class Vector3D
{
public:
Vector3D()
: x(0.f), y(0.f), z(0.f) {}
Vector3D(float x, float y, float z)
: x(x), y(y), z(z) {}
Vector3D operator+(const Vector3D& other) const
{
return Vector3D(x + other.x, y + other.y, z + other.z);
}
Vector3D operator-(const Vector3D& other) const
{
return Vector3D(x - other.x, y - other.y, z - other.z);
}
Vector3D operator*(float scalar) const
{
return Vector3D(x * scalar, y * scalar, z * scalar);
}
float DotProduct(const Vector3D& other) const
{
return x * other.x + y * other.y + z * other.z;
}
Vector3D CrossProduct(const Vector3D& other) const
{
Vector3D rstVector = {};
rstVector.x = (y * other.z - z * other.y);
rstVector.y = -x * other.z + z * other.x;
rstVector.z = (x * other.y - y * other.x);
return rstVector;
}
Vector3D VectorTripleProuct(const Vector3D& vector1, const Vector3D& vector2)const
{
Vector3D returnVec = {};
returnVec = vector1 * (this->DotProduct(vector2)) - vector2 * (this->DotProduct(vector1));
return returnVec;
}
float ScalarTripleProduct(const Vector3D& vector1, const Vector3D& vector2)const
{
return this->DotProduct(vector1.CrossProduct(vector2));
}
float GetLengthSquare()const
{
return x * x + y * y + z * z;
}
float GetLength()const
{
return sqrtf(GetLengthSquare());
}
void Nomalize()
{
float leng = GetLength();
if (leng == 0.f)
return;
x = x / leng;
y = y / leng;
z = z / leng;
}
Vector3D GetNomalizedVector() const
{
float leng = GetLength();
if (leng == 0.f)
return *this;
Vector3D returnVec = *this;
returnVec.x = x / leng;
returnVec.y = y / leng;
returnVec.z = z / leng;
return returnVec;
}
friend std::ostream& operator<<(std::ostream& os, const Vector3D& vec)
{
static const unsigned int BUFFER_MAX = 256;
char buffer[BUFFER_MAX] = {};
sprintf_s(buffer, BUFFER_MAX, "(%.2f, %.2f, %.2f)", vec.x, vec.y, vec.z);
os << buffer;
return os;
}
private:
float x, y, z;
};
int main()
{
}
