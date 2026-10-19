// 출처: https://powerclabman.tistory.com/116  공부목적으로 사용할 수학 라이브러리
// 블로그 원문 그대로 (오타/버그 포함, 예: Dot 함수의 쉼표 연산자)

// ===== 1. Vector3D.h =====
#pragma once
class Vector3
{
public:
// 생성자
inline constexpr Vector3(void) : _x(0.f), _y(0.f), _z(0.f) {}
inline explicit constexpr Vector3(float x, float y, float z) : _x(x), _y(y), _z(z) {}
inline explicit constexpr Vector3(int32 x, int32 y, int32 z) : _x((float)x), _y((float)y), _z((float)z) {}
inline constexpr Vector3(const Vector3& other);
// 연산자
inline constexpr Vector3& operator=(const Vector3& other);
inline constexpr Vector3& operator+=(const Vector3& other);
inline constexpr Vector3& operator-=(const Vector3& other);
inline constexpr Vector3& operator*=(float scalar);
inline constexpr Vector3& operator/=(float scalar);
inline constexpr Vector3 operator+(const Vector3& other) const;
inline constexpr Vector3 operator-(const Vector3& other) const;
inline constexpr Vector3 operator*(float scalar) const;
inline constexpr Vector3 operator/(float scalar) const;
// 메서드
inline constexpr float GetLengthSquare();
inline constexpr float GetLength();
inline constexpr void Nomalize();
inline constexpr void Reverse();
inline constexpr float Dot(const Vector3& other);
inline constexpr Vector3 Cross(const Vector3& other);
// 정적 변수
static const Vector3 ZeroVector;
static const float ErrorValue; //오차 허용치
public:
float _x, _y, _z;
};
//static 변수 초기화
const Vector3 Vector3::ZeroVector = Vector3(0.f, 0.f, 0.f);
const float Vector3::ErrorValue = 0.0001f; //오차 허용치
inline constexpr Vector3::Vector3(const Vector3& other)
:_x(other._x), _y(other._y), _z(other._z)
{ }
inline constexpr Vector3& Vector3::operator=(const Vector3& other)
{
_x = other._x;
_y = other._y;
_z = other._z;
return *this;
}
inline constexpr Vector3& Vector3::operator+=(const Vector3& other)
{
_x += other._x;
_y += other._y;
_z += other._z;
return *this;
}
inline constexpr Vector3& Vector3::operator-=(const Vector3& other)
{
_x -= other._x;
_y -= other._y;
_z -= other._z;
return *this;
}
inline constexpr Vector3& Vector3::operator*=(float scalar)
{
_x *= scalar;
_y *= scalar;
_z *= scalar;
return *this;
}
inline constexpr Vector3& Vector3::operator/=(float scalar)
{
assert(scalar != 0);
_x /= scalar;
_y /= scalar;
_z /= scalar;
return *this;
}
inline constexpr Vector3 Vector3::operator+(const Vector3& other) const
{
return Vector3(_x + other._x, _y + other._y, _z + other._z);
}
inline constexpr Vector3 Vector3::operator-(const Vector3& other)const
{
return Vector3(_x - other._x, _y - other._y, _z - other._z);
}
inline constexpr Vector3 Vector3::operator*(float scalar) const
{
return Vector3(_x * scalar, _y * scalar, _z * scalar);
}
inline constexpr Vector3 Vector3::operator/(float scalar) const
{
assert(scalar != 0);
return  Vector3(_x / scalar, _y / scalar, _z / scalar);
}
inline constexpr float Vector3::GetLengthSquare()
{
//노름 제곱을 구한다.
return _x * _x + _y * _y + _z * _z;
}
inline constexpr float Vector3::GetLength()
{
//노름을 구한다.
return MM::Sqrt(GetLengthSquare());
}
inline constexpr void Vector3::Nomalize()
{
//벡터의 정규화
float len = GetLength();
if (len <= ErrorValue)
len = 1;
_x /= len;
_y /= len;
_z /= len;
if (_x <= ErrorValue) _x = 0.f;
if (_y <= ErrorValue) _y = 0.f;
if (_z <= ErrorValue) _z = 0.f;
}
inline constexpr void Vector3::Reverse()
{
_x = -_x;
_y = -_y;
_z = -_z;
}
inline constexpr float Vector3::Dot(const Vector3& other)
{
//내적
return _x * other._x, _y* other._y, _z* other._z;
}
inline constexpr Vector3 Vector3::Cross(const Vector3& other)
{
return Vector3(
this->_y * other._z - this->_z * other._y,
this->_z * other._x - this->_x * other._z,
this->_x * other._y - this->_y * other._x
);
}

// ===== 2. MM (MathModule) =====
#pragma once
namespace MM
{
    inline constexpr float Sqrt(float x)
    {
        if (x < 0) return -1;
        if (x == 0 || x == 1) return x;
        float low = (x < 1) ? x : 1;
        float high = (x < 1) ? 1 : x;
        float mid = 0;
        for (int i = 0; i < 100; ++i)
        {
            mid = (low + high) / 2;
            float mid_squared = mid * mid;
            if (mid_squared == x)
            {
                return mid;
            }
            else if (mid_squared < x)
            {
                low = mid;
            }
            else
            {
                high = mid;
            }
        }
        return mid;
    }
}

// ===== 3. Matrix3x3 =====
#pragma once
class Matrix3x3
{
public:
    // 기본 생성자
    inline constexpr Matrix3x3();
    // 초기화 생성자
    inline constexpr explicit Matrix3x3(const float values[3][3]);
    inline constexpr explicit Matrix3x3(const int values[3][3]);
    inline constexpr Matrix3x3(const initializer_list<initializer_list<float>>& values);
    inline constexpr Matrix3x3(const initializer_list<initializer_list<int>>& values);
    // 복사 생성자
    inline constexpr Matrix3x3(const Matrix3x3& other);
    // 행렬 덧셈
    inline constexpr Matrix3x3 operator+(const Matrix3x3& other) const;
    // 행렬 곱셈
    inline constexpr Matrix3x3 operator*(const Matrix3x3& other) const;
    // 행렬 뺄셈
    inline constexpr Matrix3x3 operator-(const Matrix3x3& other) const;
    // 스칼라 곱셈
    inline constexpr Matrix3x3 operator*(float scalar) const;
    // 스칼라 나눗셈
    inline constexpr Matrix3x3 operator/(float scalar) const;
    // 전치 행렬
    inline constexpr Matrix3x3 Transpose() const;
    // 행렬 출력
    inline void PrintMatrix() const;
    static const int MATRIX3X3_SIZE;
public:
    union
    {
        // 행
        struct
        {
            float e11, e12, e13,
                e21, e22, e23,
                e31, e32, e33;
        };
        float _matrix[3][3];
    };
};
const int Matrix3x3::MATRIX3X3_SIZE = 3;
// 기본 생성자
inline constexpr Matrix3x3::Matrix3x3() : _matrix{ {0, 0, 0}, {0, 0, 0}, {0, 0, 0} } {}
// 초기화 생성자
inline constexpr Matrix3x3::Matrix3x3(const float values[3][3]) : _matrix{
    {values[0][0], values[0][1], values[0][2]},
    {values[1][0], values[1][1], values[1][2]},
    {values[2][0], values[2][1], values[2][2]}
} {}
inline constexpr Matrix3x3::Matrix3x3(const int values[3][3]) : _matrix{
    {static_cast<float>(values[0][0]), static_cast<float>(values[0][1]), static_cast<float>(values[0][2])},
    {static_cast<float>(values[1][0]), static_cast<float>(values[1][1]), static_cast<float>(values[1][2])},
    {static_cast<float>(values[2][0]), static_cast<float>(values[2][1]), static_cast<float>(values[2][2])}
} {}
inline constexpr Matrix3x3::Matrix3x3(const initializer_list<initializer_list<float>>& values) {
    ASSERT(values.size() == 3, "Initializer_list must have 3 rows");
    auto row = values.begin();
    for (int i = 0; i < 3; ++i) {
        ASSERT(row->size() == 3, "Each row in Initializer_list must have 3 elements");
        auto col = row->begin();
        for (int j = 0; j < 3; ++j) {
            _matrix[i][j] = *col++;
        }
        ++row;
    }
}
inline constexpr Matrix3x3::Matrix3x3(const std::initializer_list<std::initializer_list<int>>& values) {
    assert(values.size() == 3 && "Initializer_list must have 3 rows");
    auto row = values.begin();
    for (int i = 0; i < 3; ++i) {
        assert(row->size() == 3 && "Each row in Initializer_list must have 3 elements");
        auto col = row->begin();
        for (int j = 0; j < 3; ++j) {
            _matrix[i][j] = static_cast<float>(*col++);
        }
        ++row;
    }
}
// 복사 생성자
inline constexpr Matrix3x3::Matrix3x3(const Matrix3x3& other) : _matrix{
    {other._matrix[0][0], other._matrix[0][1], other._matrix[0][2]},
    {other._matrix[1][0], other._matrix[1][1], other._matrix[1][2]},
    {other._matrix[2][0], other._matrix[2][1], other._matrix[2][2]}
} {}
// 행렬 덧셈
inline constexpr Matrix3x3 Matrix3x3::operator+(const Matrix3x3& other) const {
    return Matrix3x3({
        { _matrix[0][0] + other._matrix[0][0], _matrix[0][1] + other._matrix[0][1], _matrix[0][2] + other._matrix[0][2] },
        { _matrix[1][0] + other._matrix[1][0], _matrix[1][1] + other._matrix[1][1], _matrix[1][2] + other._matrix[1][2] },
        { _matrix[2][0] + other._matrix[2][0], _matrix[2][1] + other._matrix[2][1], _matrix[2][2] + other._matrix[2][2] }
        });
}
// 행렬 곱셈
inline constexpr Matrix3x3 Matrix3x3::operator*(const Matrix3x3& other) const {
    return Matrix3x3({
        {
            _matrix[0][0] * other._matrix[0][0] + _matrix[0][1] * other._matrix[1][0] + _matrix[0][2] * other._matrix[2][0],
            _matrix[0][0] * other._matrix[0][1] + _matrix[0][1] * other._matrix[1][1] + _matrix[0][2] * other._matrix[2][1],
            _matrix[0][0] * other._matrix[0][2] + _matrix[0][1] * other._matrix[1][2] + _matrix[0][2] * other._matrix[2][2]
        },
        {
            _matrix[1][0] * other._matrix[0][0] + _matrix[1][1] * other._matrix[1][0] + _matrix[1][2] * other._matrix[2][0],
            _matrix[1][0] * other._matrix[0][1] + _matrix[1][1] * other._matrix[1][1] + _matrix[1][2] * other._matrix[2][1],
            _matrix[1][0] * other._matrix[0][2] + _matrix[1][1] * other._matrix[1][2] + _matrix[1][2] * other._matrix[2][2]
        },
        {
            _matrix[2][0] * other._matrix[0][0] + _matrix[2][1] * other._matrix[1][0] + _matrix[2][2] * other._matrix[2][0],
            _matrix[2][0] * other._matrix[0][1] + _matrix[2][1] * other._matrix[1][1] + _matrix[2][2] * other._matrix[2][1],
            _matrix[2][0] * other._matrix[0][2] + _matrix[2][1] * other._matrix[1][2] + _matrix[2][2] * other._matrix[2][2]
        }
        });
}
// 행렬 뺄셈
inline constexpr Matrix3x3 Matrix3x3::operator-(const Matrix3x3& other) const {
    return Matrix3x3({
        { _matrix[0][0] - other._matrix[0][0], _matrix[0][1] - other._matrix[0][1], _matrix[0][2] - other._matrix[0][2] },
        { _matrix[1][0] - other._matrix[1][0], _matrix[1][1] - other._matrix[1][1], _matrix[1][2] - other._matrix[1][2] },
        { _matrix[2][0] - other._matrix[2][0], _matrix[2][1] - other._matrix[2][1], _matrix[2][2] - other._matrix[2][2] }
        });
}
// 스칼라 곱셈
inline constexpr Matrix3x3 Matrix3x3::operator*(float scalar) const {
    return Matrix3x3({
        { _matrix[0][0] * scalar, _matrix[0][1] * scalar, _matrix[0][2] * scalar },
        { _matrix[1][0] * scalar, _matrix[1][1] * scalar, _matrix[1][2] * scalar },
        { _matrix[2][0] * scalar, _matrix[2][1] * scalar, _matrix[2][2] * scalar }
        });
}
// 스칼라 나눗셈
inline constexpr Matrix3x3 Matrix3x3::operator/(float scalar) const {
    ASSERT(scalar == 0, "Scalar Is Zero.");
    return Matrix3x3({
        { _matrix[0][0] / scalar, _matrix[0][1] / scalar, _matrix[0][2] / scalar },
        { _matrix[1][0] / scalar, _matrix[1][1] / scalar, _matrix[1][2] / scalar },
        { _matrix[2][0] / scalar, _matrix[2][1] / scalar, _matrix[2][2] / scalar }
        });
}
// 전치 행렬
inline constexpr Matrix3x3 Matrix3x3::Transpose() const {
    return Matrix3x3({
        { _matrix[0][0], _matrix[1][0], _matrix[2][0] },
        { _matrix[0][1], _matrix[1][1], _matrix[2][1] },
        { _matrix[0][2], _matrix[1][2], _matrix[2][2] }
        });
}
// 전치 행렬
inline void Matrix3x3::PrintMatrix() const {
    for (int i = 0; i < MATRIX3X3_SIZE; ++i) {
        for (int j = 0; j < MATRIX3X3_SIZE; ++j) {
            cout << setw(10) << _matrix[i][j] << " ";
        }
        cout << endl;
    }
}
