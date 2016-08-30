#include "kuplung/utilities/imgui/imgui.h"
#include "kuplung/utilities/imgui/imgui_internal.h"
#include "ImGuizmo.h"

namespace ImGuizmo
{
    const float ZPI = 3.14159265358979323846f;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // utility and math

    void FPU_MatrixF_x_MatrixF(const float *a, const float *b, float *r)
    {
        r[0] = a[0] * b[0] + a[1] * b[4] + a[2] * b[8] + a[3] * b[12];
        r[1] = a[0] * b[1] + a[1] * b[5] + a[2] * b[9] + a[3] * b[13];
        r[2] = a[0] * b[2] + a[1] * b[6] + a[2] * b[10] + a[3] * b[14];
        r[3] = a[0] * b[3] + a[1] * b[7] + a[2] * b[11] + a[3] * b[15];

        r[4] = a[4] * b[0] + a[5] * b[4] + a[6] * b[8] + a[7] * b[12];
        r[5] = a[4] * b[1] + a[5] * b[5] + a[6] * b[9] + a[7] * b[13];
        r[6] = a[4] * b[2] + a[5] * b[6] + a[6] * b[10] + a[7] * b[14];
        r[7] = a[4] * b[3] + a[5] * b[7] + a[6] * b[11] + a[7] * b[15];

        r[8] = a[8] * b[0] + a[9] * b[4] + a[10] * b[8] + a[11] * b[12];
        r[9] = a[8] * b[1] + a[9] * b[5] + a[10] * b[9] + a[11] * b[13];
        r[10] = a[8] * b[2] + a[9] * b[6] + a[10] * b[10] + a[11] * b[14];
        r[11] = a[8] * b[3] + a[9] * b[7] + a[10] * b[11] + a[11] * b[15];

        r[12] = a[12] * b[0] + a[13] * b[4] + a[14] * b[8] + a[15] * b[12];
        r[13] = a[12] * b[1] + a[13] * b[5] + a[14] * b[9] + a[15] * b[13];
        r[14] = a[12] * b[2] + a[13] * b[6] + a[14] * b[10] + a[15] * b[14];
        r[15] = a[12] * b[3] + a[13] * b[7] + a[14] * b[11] + a[15] * b[15];
    }

    template <typename T> T LERP(T x, T y, float z) { return (x + (y - x)*z); }
    template <typename T> T Clamp(T x, T y, T z) { return ((x<y) ? y : ((x>z) ? z : x)); }

    struct matrix_t;
    struct vec_t
    {
    public:
        vec_t(const vec_t& other) : x(other.x), y(other.y), z(other.z), w(other.w) {}
        vec_t() {}
        vec_t(float _x, float _y, float _z = 0.f, float _w = 0.f) : x(_x), y(_y), z(_z), w(_w) {}
        vec_t(int _x, int _y, int _z = 0, int _w = 0) : x((float)_x), y((float)_y), z((float)_z), w((float)_w) {}
        vec_t(float v) : x(v), y(v), z(v), w(v) {}

        float x, y, z, w;

        void Lerp(const vec_t& v, float t)
        {
            x += (v.x - x) * t;
            y += (v.y - y) * t;
            z += (v.z - z) * t;
            w += (v.w - w) * t;
        }

        void set(float v) { x = y = z = w = v; }
        void set(float _x, float _y, float _z = 0.f, float _w = 0.f) { x = _x; y = _y; z = _z; w = _w; }

        vec_t& operator -= (const vec_t& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
        vec_t& operator += (const vec_t& v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
        vec_t& operator *= (const vec_t& v) { x *= v.x; y *= v.y; z *= v.z; w *= v.w; return *this; }
        vec_t& operator *= (float v) { x *= v;	y *= v;	z *= v;	w *= v;	return *this; }

        vec_t operator * (float f) const;
        vec_t operator - () const;
        vec_t operator - (const vec_t& v) const;
        vec_t operator + (const vec_t& v) const;
        vec_t operator * (const vec_t& v) const;

        const vec_t& operator + () const { return (*this); }
        float Length() const { return sqrtf(x*x + y*y + z*z); };
        float LengthSq() const { return (x*x + y*y + z*z); };
        vec_t Normalize() { (*this) *= (1.f / Length()); return (*this); }
        vec_t Normalize(const vec_t& v) { this->set(v.x, v.y, v.z, v.w); this->Normalize(); return (*this); }

        void Cross(const vec_t& v)
        {
            vec_t res;
            res.x = y * v.z - z * v.y;
            res.y = z * v.x - x * v.z;
            res.z = x * v.y - y * v.x;

            x = res.x;
            y = res.y;
            z = res.z;
            w = 0.f;
        }
        void Cross(const vec_t& v1, const vec_t& v2)
        {
            x = v1.y * v2.z - v1.z * v2.y;
            y = v1.z * v2.x - v1.x * v2.z;
            z = v1.x * v2.y - v1.y * v2.x;
            w = 0.f;
        }
        float Dot(const vec_t &v) const
        {
            return (x * v.x) + (y * v.y) + (z * v.z) + (w * v.w);
        }
        float Dot3(const vec_t &v) const
        {
            return (x * v.x) + (y * v.y) + (z * v.z);
        }

        void Transform(const matrix_t& matrix);
        void Transform(const vec_t & s, const matrix_t& matrix);

        void TransformVector(const matrix_t& matrix);
        void TransformPoint(const matrix_t& matrix);
        void TransformVector(const vec_t& v, const matrix_t& matrix) { (*this) = v; this->TransformVector(matrix); }
        void TransformPoint(const vec_t& v, const matrix_t& matrix) { (*this) = v; this->TransformPoint(matrix); }

        float& operator [] (size_t index) { return ((float*)&x)[index]; }
        const float& operator [] (size_t index) const { return ((float*)&x)[index]; }
    };

    vec_t vec_t::operator * (float f) const { return vec_t(x * f, y * f, z * f, w *f); }
    vec_t vec_t::operator - () const { return vec_t(-x, -y, -z, -w); }
    vec_t vec_t::operator - (const vec_t& v) const { return vec_t(x - v.x, y - v.y, z - v.z, w - v.w); }
    vec_t vec_t::operator + (const vec_t& v) const { return vec_t(x + v.x, y + v.y, z + v.z, w + v.w); }
    vec_t vec_t::operator * (const vec_t& v) const { return vec_t(x * v.x, y * v.y, z * v.z, w * v.w); }

    ImVec2 operator+ (const ImVec2& a, const ImVec2& b) { return ImVec2(a.x + b.x, a.y + b.y); }

    vec_t Normalized(const vec_t& v) { vec_t res; res = v; res.Normalize(); return res; }
    vec_t Cross(const vec_t& v1, const vec_t& v2)
    {
        vec_t res;
        res.x = v1.y * v2.z - v1.z * v2.y;
        res.y = v1.z * v2.x - v1.x * v2.z;
        res.z = v1.x * v2.y - v1.y * v2.x;
        res.w = 0.f;
        return res;
    }

    float Dot(const vec_t &v1, const vec_t &v2)
    {
        return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
    }

    vec_t BuildPlan(const vec_t & p_point1, const vec_t & p_normal)
    {
        vec_t normal, res;
        normal.Normalize(p_normal);
        res.w = normal.Dot(p_point1);
        res.x = normal.x;
        res.y = normal.y;
        res.z = normal.z;
        return res;
    }

    struct matrix_t
    {
    public:
        union
        {
            float m[4][4];
            float m16[16];
            struct
            {
                vec_t right, up, dir, position;
            };
            struct
            {
                vec_t line[4];
            };
        };

        matrix_t(const matrix_t& other) { memcpy(&m16[0], &other.m16[0], sizeof(float) * 16); }
        matrix_t() {}

        operator float * () { return m16; }
        operator const float* () const { return m16; }
        void translation(float _x, float _y, float _z) { this->translation(vec_t(_x, _y, _z)); }

        void translation(const vec_t& vt)
        {
            right.set(1.f, 0.f, 0.f, 0.f);
            up.set(0.f, 1.f, 0.f, 0.f);
            dir.set(0.f, 0.f, 1.f, 0.f);
            position.set(vt.x, vt.y, vt.z, 1.f);
        }

        void scale(float _x, float _y, float _z)
        {
            right.set(_x, 0.f, 0.f, 0.f);
            up.set(0.f, _y, 0.f, 0.f);
            dir.set(0.f, 0.f, _z, 0.f);
            position.set(0.f, 0.f, 0.f, 1.f);
        }
        void scale(const vec_t& s) { scale(s.x, s.y, s.z); }

        matrix_t& operator *= (const matrix_t& mat)
        {
            matrix_t tmpMat;
            tmpMat = *this;
            tmpMat.Multiply(mat);
            *this = tmpMat;
            return *this;
        }
        matrix_t operator * (const matrix_t& mat) const
        {
            matrix_t matT;
            matT.Multiply(*this, mat);
            return matT;
        }

        void Multiply(const matrix_t &matrix)
        {
            matrix_t tmp;
            tmp = *this;

            FPU_MatrixF_x_MatrixF((float*)&tmp, (float*)&matrix, (float*)this);
        }

        void Multiply(const matrix_t &m1, const matrix_t &m2)
        {
            FPU_MatrixF_x_MatrixF((float*)&m1, (float*)&m2, (float*)this);
        }

        float GetDeterminant() const
        {
            return m[0][0] * m[1][1] * m[2][2] + m[0][1] * m[1][2] * m[2][0] + m[0][2] * m[1][0] * m[2][1] -
                m[0][2] * m[1][1] * m[2][0] - m[0][1] * m[1][0] * m[2][2] - m[0][0] * m[1][2] * m[2][1];
        }

        float Inverse(const matrix_t &srcMatrix, bool affine = false);
        float Inverse(bool affine = false);
        void SetToIdentity() {
            right.set(1.f, 0.f, 0.f, 0.f);
            up.set(0.f, 1.f, 0.f, 0.f);
            dir.set(0.f, 0.f, 1.f, 0.f);
            position.set(0.f, 0.f, 0.f, 1.f);
        }
        void transpose()
        {
            matrix_t tmpm;
            for (int l = 0; l < 4; l++)
            {
                for (int c = 0; c < 4; c++)
                {
                    tmpm.m[l][c] = m[c][l];
                }
            }
            (*this) = tmpm;
        }

        void RotationAxis(const vec_t & axis, float angle);
    };

    void vec_t::Transform(const matrix_t& matrix)
    {
        vec_t out;

        out.x = x * matrix.m[0][0] + y * matrix.m[1][0] + z * matrix.m[2][0] + w * matrix.m[3][0];
        out.y = x * matrix.m[0][1] + y * matrix.m[1][1] + z * matrix.m[2][1] + w * matrix.m[3][1];
        out.z = x * matrix.m[0][2] + y * matrix.m[1][2] + z * matrix.m[2][2] + w * matrix.m[3][2];
        out.w = x * matrix.m[0][3] + y * matrix.m[1][3] + z * matrix.m[2][3] + w * matrix.m[3][3];

        x = out.x;
        y = out.y;
        z = out.z;
        w = out.w;
    }

    void vec_t::Transform(const vec_t & s, const matrix_t& matrix)
    {
        *this = s;
        Transform(matrix);
    }

    void vec_t::TransformPoint(const matrix_t& matrix)
    {
        vec_t out;

        out.x = x * matrix.m[0][0] + y * matrix.m[1][0] + z * matrix.m[2][0] + matrix.m[3][0];
        out.y = x * matrix.m[0][1] + y * matrix.m[1][1] + z * matrix.m[2][1] + matrix.m[3][1];
        out.z = x * matrix.m[0][2] + y * matrix.m[1][2] + z * matrix.m[2][2] + matrix.m[3][2];
        out.w = x * matrix.m[0][3] + y * matrix.m[1][3] + z * matrix.m[2][3] + matrix.m[3][3];

        x = out.x;
        y = out.y;
        z = out.z;
        w = out.w;
    }


    void vec_t::TransformVector(const matrix_t& matrix)
    {
        vec_t out;

        out.x = x * matrix.m[0][0] + y * matrix.m[1][0] + z * matrix.m[2][0];
        out.y = x * matrix.m[0][1] + y * matrix.m[1][1] + z * matrix.m[2][1];
        out.z = x * matrix.m[0][2] + y * matrix.m[1][2] + z * matrix.m[2][2];
        out.w = x * matrix.m[0][3] + y * matrix.m[1][3] + z * matrix.m[2][3];

        x = out.x;
        y = out.y;
        z = out.z;
        w = out.w;
    }

    float matrix_t::Inverse(const matrix_t &srcMatrix, bool affine)
    {
        float det = 0;

        if (affine)
        {
            det = GetDeterminant();
            float s = 1 / det;
            m[0][0] = (srcMatrix.m[1][1] * srcMatrix.m[2][2] - srcMatrix.m[1][2] * srcMatrix.m[2][1]) * s;
            m[0][1] = (srcMatrix.m[2][1] * srcMatrix.m[0][2] - srcMatrix.m[2][2] * srcMatrix.m[0][1]) * s;
            m[0][2] = (srcMatrix.m[0][1] * srcMatrix.m[1][2] - srcMatrix.m[0][2] * srcMatrix.m[1][1]) * s;
            m[1][0] = (srcMatrix.m[1][2] * srcMatrix.m[2][0] - srcMatrix.m[1][0] * srcMatrix.m[2][2]) * s;
            m[1][1] = (srcMatrix.m[2][2] * srcMatrix.m[0][0] - srcMatrix.m[2][0] * srcMatrix.m[0][2]) * s;
            m[1][2] = (srcMatrix.m[0][2] * srcMatrix.m[1][0] - srcMatrix.m[0][0] * srcMatrix.m[1][2]) * s;
            m[2][0] = (srcMatrix.m[1][0] * srcMatrix.m[2][1] - srcMatrix.m[1][1] * srcMatrix.m[2][0]) * s;
            m[2][1] = (srcMatrix.m[2][0] * srcMatrix.m[0][1] - srcMatrix.m[2][1] * srcMatrix.m[0][0]) * s;
            m[2][2] = (srcMatrix.m[0][0] * srcMatrix.m[1][1] - srcMatrix.m[0][1] * srcMatrix.m[1][0]) * s;
            m[3][0] = -(m[0][0] * srcMatrix.m[3][0] + m[1][0] * srcMatrix.m[3][1] + m[2][0] * srcMatrix.m[3][2]);
            m[3][1] = -(m[0][1] * srcMatrix.m[3][0] + m[1][1] * srcMatrix.m[3][1] + m[2][1] * srcMatrix.m[3][2]);
            m[3][2] = -(m[0][2] * srcMatrix.m[3][0] + m[1][2] * srcMatrix.m[3][1] + m[2][2] * srcMatrix.m[3][2]);
        }
        else
        {
            // transpose matrix
            float src[16];
            for (int i = 0; i < 4; ++i)
            {
                src[i] = srcMatrix.m16[i * 4];
                src[i + 4] = srcMatrix.m16[i * 4 + 1];
                src[i + 8] = srcMatrix.m16[i * 4 + 2];
                src[i + 12] = srcMatrix.m16[i * 4 + 3];
            }

            // calculate pairs for first 8 elements (cofactors)
            float tmp[12]; // temp array for pairs
            tmp[0] = src[10] * src[15];
            tmp[1] = src[11] * src[14];
            tmp[2] = src[9] * src[15];
            tmp[3] = src[11] * src[13];
            tmp[4] = src[9] * src[14];
            tmp[5] = src[10] * src[13];
            tmp[6] = src[8] * src[15];
            tmp[7] = src[11] * src[12];
            tmp[8] = src[8] * src[14];
            tmp[9] = src[10] * src[12];
            tmp[10] = src[8] * src[13];
            tmp[11] = src[9] * src[12];

            // calculate first 8 elements (cofactors)
            m16[0] = (tmp[0] * src[5] + tmp[3] * src[6] + tmp[4] * src[7]) - (tmp[1] * src[5] + tmp[2] * src[6] + tmp[5] * src[7]);
            m16[1] = (tmp[1] * src[4] + tmp[6] * src[6] + tmp[9] * src[7]) - (tmp[0] * src[4] + tmp[7] * src[6] + tmp[8] * src[7]);
            m16[2] = (tmp[2] * src[4] + tmp[7] * src[5] + tmp[10] * src[7]) - (tmp[3] * src[4] + tmp[6] * src[5] + tmp[11] * src[7]);
            m16[3] = (tmp[5] * src[4] + tmp[8] * src[5] + tmp[11] * src[6]) - (tmp[4] * src[4] + tmp[9] * src[5] + tmp[10] * src[6]);
            m16[4] = (tmp[1] * src[1] + tmp[2] * src[2] + tmp[5] * src[3]) - (tmp[0] * src[1] + tmp[3] * src[2] + tmp[4] * src[3]);
            m16[5] = (tmp[0] * src[0] + tmp[7] * src[2] + tmp[8] * src[3]) - (tmp[1] * src[0] + tmp[6] * src[2] + tmp[9] * src[3]);
            m16[6] = (tmp[3] * src[0] + tmp[6] * src[1] + tmp[11] * src[3]) - (tmp[2] * src[0] + tmp[7] * src[1] + tmp[10] * src[3]);
            m16[7] = (tmp[4] * src[0] + tmp[9] * src[1] + tmp[10] * src[2]) - (tmp[5] * src[0] + tmp[8] * src[1] + tmp[11] * src[2]);

            // calculate pairs for second 8 elements (cofactors)
            tmp[0] = src[2] * src[7];
            tmp[1] = src[3] * src[6];
            tmp[2] = src[1] * src[7];
            tmp[3] = src[3] * src[5];
            tmp[4] = src[1] * src[6];
            tmp[5] = src[2] * src[5];
            tmp[6] = src[0] * src[7];
            tmp[7] = src[3] * src[4];
            tmp[8] = src[0] * src[6];
            tmp[9] = src[2] * src[4];
            tmp[10] = src[0] * src[5];
            tmp[11] = src[1] * src[4];

            // calculate second 8 elements (cofactors)
            m16[8] = (tmp[0] * src[13] + tmp[3] * src[14] + tmp[4] * src[15]) - (tmp[1] * src[13] + tmp[2] * src[14] + tmp[5] * src[15]);
            m16[9] = (tmp[1] * src[12] + tmp[6] * src[14] + tmp[9] * src[15]) - (tmp[0] * src[12] + tmp[7] * src[14] + tmp[8] * src[15]);
            m16[10] = (tmp[2] * src[12] + tmp[7] * src[13] + tmp[10] * src[15]) - (tmp[3] * src[12] + tmp[6] * src[13] + tmp[11] * src[15]);
            m16[11] = (tmp[5] * src[12] + tmp[8] * src[13] + tmp[11] * src[14]) - (tmp[4] * src[12] + tmp[9] * src[13] + tmp[10] * src[14]);
            m16[12] = (tmp[2] * src[10] + tmp[5] * src[11] + tmp[1] * src[9]) - (tmp[4] * src[11] + tmp[0] * src[9] + tmp[3] * src[10]);
            m16[13] = (tmp[8] * src[11] + tmp[0] * src[8] + tmp[7] * src[10]) - (tmp[6] * src[10] + tmp[9] * src[11] + tmp[1] * src[8]);
            m16[14] = (tmp[6] * src[9] + tmp[11] * src[11] + tmp[3] * src[8]) - (tmp[10] * src[11] + tmp[2] * src[8] + tmp[7] * src[9]);
            m16[15] = (tmp[10] * src[10] + tmp[4] * src[8] + tmp[9] * src[9]) - (tmp[8] * src[9] + tmp[11] * src[10] + tmp[5] * src[8]);

            // calculate determinant
            det = src[0] * m16[0] + src[1] * m16[1] + src[2] * m16[2] + src[3] * m16[3];

            // calculate matrix inverse
            float invdet = 1 / det;
            for (int j = 0; j < 16; ++j)
            {
                m16[j] *= invdet;
            }
        }

        return det;
    }

    void matrix_t::RotationAxis(const vec_t & axis, float angle)
    {
        float length2 = axis.LengthSq();
        if (length2 < FLT_EPSILON)
        {
            SetToIdentity();
            return;
        }

        vec_t n = axis * (1.f / sqrtf(length2));
        float s = sinf(angle);
        float c = cosf(angle);
        float k = 1.f - c;

        float xx = n.x * n.x * k + c;
        float yy = n.y * n.y * k + c;
        float zz = n.z * n.z * k + c;
        float xy = n.x * n.y * k;
        float yz = n.y * n.z * k;
        float zx = n.z * n.x * k;
        float xs = n.x * s;
        float ys = n.y * s;
        float zs = n.z * s;

        m[0][0] = xx;
        m[0][1] = xy + zs;
        m[0][2] = zx - ys;
        m[0][3] = 0.f;
        m[1][0] = xy - zs;
        m[1][1] = yy;
        m[1][2] = yz + xs;
        m[1][3] = 0.f;
        m[2][0] = zx + ys;
        m[2][1] = yz - xs;
        m[2][2] = zz;
        m[2][3] = 0.f;
        m[3][0] = 0.f;
        m[3][1] = 0.f;
        m[3][2] = 0.f;
        m[3][3] = 1.f;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //

    enum MOVETYPE
    {
        NONE,
        MOVE_X,
        MOVE_Y,
        MOVE_Z,
        MOVE_XY,
        MOVE_XZ,
        MOVE_YZ,
        MOVE_SCREEN,
        ROTATE_X,
        ROTATE_Y,
        ROTATE_Z,
        ROTATE_SCREEN,
        SCALE_X,
        SCALE_Y,
        SCALE_Z,
        SCALE_XYZ,
    };

    struct Context
    {
        Context() : mbUsing(false), mbEnable(true)
        {
        }

        ImDrawList* mDrawList;

        matrix_t mViewMat;
        matrix_t mProjectionMat;
        matrix_t mModel;
        matrix_t mModelInverse;
        matrix_t mMVP;
        matrix_t mViewProjection;

        vec_t mCameraEye;
        vec_t mCameraRight;
        vec_t mCameraDir;
        vec_t mCameraUp;
        vec_t mRayOrigin;
        vec_t mRayVector;

        vec_t mCameraToModel;

        ImVec2 mScreenSquareCenter;
        ImVec2 mScreenSquareMin;
        ImVec2 mScreenSquareMax;

        float mScreenFactor;
        vec_t mRelativeOrigin;

        bool mbUsing;
        bool mbEnable;

        // translation
        vec_t mTranslationPlan;
        vec_t mTranslationPlanOrigin;
        vec_t mMatrixOrigin;

        // rotation
        vec_t mRotationVectorSource;
        float mRotationAngle;
        float mRotationAngleOrigin;

        int mCurrentOperation;
    };

    static Context gContext;

    static const float angleLimit = 0.96f;
    static const float planeLimit = 0.2f;

    static const vec_t direction[3] = { vec_t(1.f,0.f,0.f), vec_t(0.f,1.f,0.f), vec_t(0.f,0.f,1.f) };
    static const ImU32 directionColor[3] = { 0xFF0000AA, 0xFF00AA00, 0xFFAA0000 };
    static const ImU32 selectionColor = 0xFF1080FF;
    static const ImU32 inactiveColor = 0x99999999;
    static const ImU32 translationLineColor = 0xAAAAAAAA;
    static const char *translationInfoMask[] = { "X : %5.2f", "Y : %5.2f", "Z : %5.2f", "X : %5.2f Y : %5.2f", "X : %5.2f Z : %5.2f", "Y : %5.2f Z : %5.2f", "X : %5.2f Y : %5.2f Z : %5.2f" };
    static const char *rotationInfoMask[] = { "X : %5.2f deg %5.2f rad", "Y : %5.2f deg %5.2f rad", "Z : %5.2f deg %5.2f rad", "Screen : %5.2f deg %5.2f rad" };
    static const int translationInfoIndex[] = { 0,0,0, 1,0,0, 2,0,0, 0,1,0, 0,2,0, 1,2,0, 0,1,2 };
    static const float quadMin = 0.5f;
    static const float quadMax = 0.8f;
    static const float quadUV[8] = { quadMin, quadMin, quadMin, quadMax, quadMax, quadMax, quadMax, quadMin };
    static const int halfCircleSegmentCount = 64;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    static int GetMoveType(vec_t *gizmoHitProportion);
    static int GetRotateType();

    static ImVec2 worldToPos(const vec_t& worldPos, const matrix_t& mat)
    {
        ImGuiIO& io = ImGui::GetIO();

        vec_t trans;
        trans.TransformPoint(worldPos, mat);
        trans *= 0.5f / trans.w;
        trans += vec_t(0.5f);
        trans.y = 1.f - trans.y;
        trans.x *= io.DisplaySize.x;
        trans.y *= io.DisplaySize.y;
        return ImVec2(trans.x, trans.y);
    }

    static void ComputeCameraRay(vec_t &rayOrigin, vec_t &rayDir)
    {
        ImGuiIO& io = ImGui::GetIO();

        matrix_t mViewProjInverse;
        mViewProjInverse.Inverse(gContext.mViewMat * gContext.mProjectionMat);

        float mox = (io.MousePos.x / io.DisplaySize.x) * 2.f - 1.f;
        float moy = (1.f - (io.MousePos.y / io.DisplaySize.y)) * 2.f - 1.f;

        rayOrigin.Transform(vec_t(mox, moy, 0.f, 1.f), mViewProjInverse);
        rayOrigin *= 1.f / rayOrigin.w;
        vec_t rayEnd;
        rayEnd.Transform(vec_t(mox, moy, 1.f, 1.f), mViewProjInverse);
        rayEnd *= 1.f / rayEnd.w;
        rayDir = Normalized(rayEnd - rayOrigin);
    }

    static float IntersectRayPlane(const vec_t & rOrigin, const vec_t& rVector, const vec_t& plan)
    {
        float numer = plan.Dot3(rOrigin) - plan.w;
        float denom = plan.Dot3(rVector);

        if (fabsf(denom) < FLT_EPSILON)  // normal is orthogonal to vector, cant intersect
            return -1.0f;

        return -(numer / denom);
    }

    void BeginFrame()
    {
        ImGuiIO& io = ImGui::GetIO();

        ImGui::Begin("gizmo", NULL, io.DisplaySize, 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus);
        gContext.mDrawList = ImGui::GetWindowDrawList();

        ImGui::End();
    }

    bool IsUsing()
    {
        return gContext.mbUsing;
    }

    bool IsOver()
    {
        return (GetMoveType(NULL) != NONE) || GetRotateType() != NONE || IsUsing();
    }

    void Enable(bool enable)
    {
        gContext.mbEnable = enable;
        if (!enable)
            gContext.mbUsing = false;
    }

    static float GetUniform(const vec_t& position, const matrix_t& mat)
    {
        vec_t trf(position.x, position.y, position.z, 1.f);
        trf.Transform(mat);
        return trf.w;
    }

    static void ComputeContext(const float *view, const float *projection, float *matrix)
    {
        gContext.mViewMat = *(matrix_t*)view;
        gContext.mProjectionMat = *(matrix_t*)projection;
        gContext.mModel = *(matrix_t*)matrix;
        gContext.mModelInverse.Inverse(gContext.mModel);
        gContext.mViewProjection = gContext.mViewMat * gContext.mProjectionMat;
        gContext.mMVP = gContext.mModel * gContext.mViewProjection;

        matrix_t viewInverse;
        viewInverse.Inverse(gContext.mViewMat);
        gContext.mCameraDir = viewInverse.dir;
        gContext.mCameraEye = viewInverse.position;
        gContext.mCameraRight = viewInverse.right;
        gContext.mCameraUp = viewInverse.up;
        gContext.mCameraToModel = gContext.mModel.position - gContext.mCameraEye;
        gContext.mScreenFactor = 0.1f * GetUniform(gContext.mModel.position, gContext.mViewProjection);

        ImVec2 centerSSpace = worldToPos(vec_t(0.f), gContext.mMVP);
        gContext.mScreenSquareCenter = centerSSpace;
        gContext.mScreenSquareMin = ImVec2(centerSSpace.x - 10.f, centerSSpace.y - 10.f);
        gContext.mScreenSquareMax = ImVec2(centerSSpace.x + 10.f, centerSSpace.y + 10.f);

        ComputeCameraRay(gContext.mRayOrigin, gContext.mRayVector);
    }

    static void ComputeColors(ImU32 *colors, int type, MODE mode)
    {
        if (gContext.mbEnable)
        {
            switch (mode)
            {
            case TRANSLATE:
                colors[0] = (type == MOVE_SCREEN) ? selectionColor : 0xFFFFFFFF;
                for (int i = 0; i < 3; i++)
                {
                    int colorPlaneIndex = (i + 2) % 3;
                    colors[i + 1] = (type == (int)(MOVE_X + i)) ? selectionColor : directionColor[i];
                    colors[i + 4] = (type == (int)(MOVE_XY + i)) ? selectionColor : directionColor[colorPlaneIndex];
                }
                break;
            case ROTATE:
                colors[0] = (type == ROTATE_SCREEN) ? selectionColor : 0xFFFFFFFF;
                for (int i = 0; i < 3; i++)
                    colors[i + 1] = (type == (int)(ROTATE_X + i)) ? selectionColor : directionColor[i];
                break;
            case SCALE:
                break;
            }
        }
        else
        {
            for (int i = 0; i < sizeof(colors) / sizeof(ImU32); i++)
                colors[i] = inactiveColor;
        }
    }

    static void DrawRotationGizmo(int type)
    {
        ImDrawList* drawList = gContext.mDrawList;
        ImGuiIO& io = ImGui::GetIO();

        // colors
        ImU32 colors[7];
        ComputeColors(colors, type, ROTATE);

        vec_t cameraToModelNormalized = Normalized(gContext.mCameraToModel);
        cameraToModelNormalized.TransformVector(gContext.mModelInverse);

        for (int axis = 0; axis < 3; axis++)
        {
            ImVec2 circlePos[halfCircleSegmentCount];

            float angleStart = atan2f(cameraToModelNormalized[(4-axis)%3], cameraToModelNormalized[(3 - axis) % 3]) + ZPI * 0.5f;

            for (unsigned int i = 0; i < halfCircleSegmentCount; i++)
            {
                float ng = angleStart + ZPI * ((float)i / (float)halfCircleSegmentCount);
                vec_t axisPos(cosf(ng), sinf(ng), 0.f);
                vec_t pos = vec_t(axisPos[axis], axisPos[(axis+1)%3], axisPos[(axis+2)%3]) * gContext.mScreenFactor;
                circlePos[i] = worldToPos(pos, gContext.mMVP);
            }
            drawList->AddPolyline(circlePos, halfCircleSegmentCount, colors[3 - axis], false, 2, true);
        }
        drawList->AddCircle(worldToPos(gContext.mModel.position, gContext.mViewProjection), 0.06f * io.DisplaySize.x, colors[0], 64);

        if (gContext.mbUsing)
        {
            ImVec2 circlePos[halfCircleSegmentCount +1];

            circlePos[0] = worldToPos(gContext.mModel.position, gContext.mViewProjection);
            for (unsigned int i = 1; i < halfCircleSegmentCount; i++)
            {
                float ng = gContext.mRotationAngle * ((float)(i-1) / (float)(halfCircleSegmentCount -1));
                matrix_t rotateVectorMatrix;
                rotateVectorMatrix.RotationAxis(gContext.mTranslationPlan, ng);
                vec_t pos;
                pos.TransformPoint(gContext.mRotationVectorSource, rotateVectorMatrix);
                pos *= gContext.mScreenFactor;
                circlePos[i] = worldToPos(pos + gContext.mModel.position, gContext.mViewProjection);
            }
            drawList->AddConvexPolyFilled(circlePos, halfCircleSegmentCount, 0x801080FF, true);
            drawList->AddPolyline(circlePos, halfCircleSegmentCount, 0xFF1080FF, true, 2, true);

            ImVec2 destinationPosOnScreen = circlePos[1];
            char tmps[512];
            ImFormatString(tmps, sizeof(tmps), rotationInfoMask[type - ROTATE_X], (gContext.mRotationAngle/ZPI)*180.f, gContext.mRotationAngle);
            drawList->AddText(ImVec2(destinationPosOnScreen.x + 15, destinationPosOnScreen.y + 15), 0xFF000000, tmps);
            drawList->AddText(ImVec2(destinationPosOnScreen.x + 14, destinationPosOnScreen.y + 14), 0xFFFFFFFF, tmps);
        }
    }

    static void DrawScaleGizmo(int type)
    {
        ImDrawList* drawList = gContext.mDrawList;

        // colors
        ImU32 colors[7];
        ComputeColors(colors, type, SCALE);

        // draw screen quad
        drawList->AddCircle(gContext.mScreenSquareCenter, 8.f, colors[0]);
        /*
        // draw
        for (unsigned int i = 0; i < 3; i++)
        {
            const int planNormal = (i + 2) % 3;
            const vec_t& dirPlaneX = direction[i];
            const vec_t& dirPlaneY = direction[(i + 1) % 3];
            const vec_t& dirPlaneNormal = direction[planNormal];

            vec_t cameraEyeToGizmo = Normalized(gContext.mModel.position - gContext.mCameraEye);
            const bool belowAxisLimit = (fabsf(cameraEyeToGizmo.Dot3(dirPlaneX)) < angleLimit);
            const bool belowPlaneLimit = (fabsf(cameraEyeToGizmo.Dot3(dirPlaneNormal)) > planeLimit);

            // draw axis
            if (belowAxisLimit)
            {
                ImVec2 baseSSpace = worldToPos(dirPlaneX * 0.1f * gContext.mScreenFactor, gContext.mMVP);
                ImVec2 worldDirSSpace = worldToPos(dirPlaneX * gContext.mScreenFactor, gContext.mMVP);

                drawList->AddLine(baseSSpace, worldDirSSpace, colors[i + 1], 6.f);
            }

            // draw plane
            if (belowPlaneLimit)
            {
                ImVec2 screenQuadPts[4];
                for (int j = 0; j < 4; j++)
                {
                    vec_t cornerWorldPos = (dirPlaneX * quadUV[j * 2] + dirPlaneY  * quadUV[j * 2 + 1]) * gContext.mScreenFactor;
                    screenQuadPts[j] = worldToPos(cornerWorldPos, gContext.mMVP);
                }
                drawList->AddConvexPolyFilled(screenQuadPts, 4, colors[i + 4], true);
            }
        }

        if (gContext.mbUsing)
        {
            ImVec2 sourcePosOnScreen = worldToPos(gContext.mMatrixOrigin, gContext.mViewProjection);
            ImVec2 destinationPosOnScreen = worldToPos(gContext.mModel.position, gContext.mViewProjection);
            vec_t dif(destinationPosOnScreen.x - sourcePosOnScreen.x, destinationPosOnScreen.y - sourcePosOnScreen.y);
            dif.Normalize();
            dif *= 5.f;
            drawList->AddCircle(sourcePosOnScreen, 6.f, translationLineColor);
            drawList->AddCircle(destinationPosOnScreen, 6.f, translationLineColor);
            drawList->AddLine(ImVec2(sourcePosOnScreen.x + dif.x, sourcePosOnScreen.y + dif.y), ImVec2(destinationPosOnScreen.x - dif.x, destinationPosOnScreen.y - dif.y), translationLineColor, 2.f);

            char tmps[512];
            vec_t deltaInfo = gContext.mModel.position - gContext.mMatrixOrigin;
            int componentInfoIndex = (type - MOVE_X) * 3;
            ImFormatString(tmps, sizeof(tmps), translationInfoMask[type - MOVE_X], deltaInfo[translationInfoIndex[componentInfoIndex]], deltaInfo[translationInfoIndex[componentInfoIndex + 1]], deltaInfo[translationInfoIndex[componentInfoIndex + 2]]);
            drawList->AddText(ImVec2(destinationPosOnScreen.x + 15, destinationPosOnScreen.y + 15), 0xFF000000, tmps);
            drawList->AddText(ImVec2(destinationPosOnScreen.x + 14, destinationPosOnScreen.y + 14), 0xFFFFFFFF, tmps);
        }
        */
    }

    static void DrawTranslationGizmo(int type)
    {
        ImDrawList* drawList = gContext.mDrawList;

        // colors
        ImU32 colors[7];
        ComputeColors(colors, type, TRANSLATE);

        // draw screen quad
        drawList->AddCircle(gContext.mScreenSquareCenter, 8.f, colors[0]);

        // draw
        for (unsigned int i = 0; i < 3; i++)
        {
            const int planNormal = (i + 2) % 3;
            const vec_t& dirPlaneX = direction[i];
            const vec_t& dirPlaneY = direction[(i + 1) % 3];
            const vec_t& dirPlaneNormal = direction[planNormal];

            vec_t cameraEyeToGizmo = Normalized(gContext.mModel.position - gContext.mCameraEye);
            const bool belowAxisLimit = (fabsf(cameraEyeToGizmo.Dot3(dirPlaneX)) < angleLimit);
            const bool belowPlaneLimit = (fabsf(cameraEyeToGizmo.Dot3(dirPlaneNormal)) > planeLimit);

            // draw axis
            if (belowAxisLimit)
            {
                ImVec2 baseSSpace = worldToPos(dirPlaneX * 0.1f * gContext.mScreenFactor, gContext.mMVP);
                ImVec2 worldDirSSpace = worldToPos(dirPlaneX * gContext.mScreenFactor, gContext.mMVP);

                drawList->AddLine(baseSSpace, worldDirSSpace, colors[i + 1], 6.f);
            }

            // draw plane
            if (belowPlaneLimit)
            {
                ImVec2 screenQuadPts[4];
                for (int j = 0; j < 4; j++)
                {
                    vec_t cornerWorldPos = (dirPlaneX * quadUV[j * 2] + dirPlaneY  * quadUV[j * 2 + 1]) * gContext.mScreenFactor;
                    screenQuadPts[j] = worldToPos(cornerWorldPos, gContext.mMVP);
                }
                drawList->AddConvexPolyFilled(screenQuadPts, 4, colors[i + 4], true);
            }
        }

        if (gContext.mbUsing)
        {
            ImVec2 sourcePosOnScreen = worldToPos(gContext.mMatrixOrigin, gContext.mViewProjection);
            ImVec2 destinationPosOnScreen = worldToPos(gContext.mModel.position, gContext.mViewProjection);
            vec_t dif(destinationPosOnScreen.x - sourcePosOnScreen.x, destinationPosOnScreen.y - sourcePosOnScreen.y);
            dif.Normalize();
            dif *= 5.f;
            drawList->AddCircle(sourcePosOnScreen, 6.f, translationLineColor);
            drawList->AddCircle(destinationPosOnScreen, 6.f, translationLineColor);
            drawList->AddLine(ImVec2(sourcePosOnScreen.x + dif.x, sourcePosOnScreen.y + dif.y), ImVec2(destinationPosOnScreen.x - dif.x, destinationPosOnScreen.y - dif.y), translationLineColor, 2.f);

            char tmps[512];
            vec_t deltaInfo = gContext.mModel.position - gContext.mMatrixOrigin;
            int componentInfoIndex = (type - MOVE_X) * 3;
            ImFormatString(tmps, sizeof(tmps), translationInfoMask[type - MOVE_X], deltaInfo[translationInfoIndex[componentInfoIndex]], deltaInfo[translationInfoIndex[componentInfoIndex + 1]], deltaInfo[translationInfoIndex[componentInfoIndex + 2]]);
            drawList->AddText(ImVec2(destinationPosOnScreen.x + 15, destinationPosOnScreen.y + 15), 0xFF000000, tmps);
            drawList->AddText(ImVec2(destinationPosOnScreen.x + 14, destinationPosOnScreen.y + 14), 0xFFFFFFFF, tmps);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //

    static int GetScaleType()
    {
        ImGuiIO& io = ImGui::GetIO();
        int type = NONE;
        /*
        vec_t deltaScreen(io.MousePos.x - gContext.mScreenSquareCenter.x, io.MousePos.y - gContext.mScreenSquareCenter.y);
        float dist = deltaScreen.Length();
        if (dist >= 0.058f * io.DisplaySize.x && dist < 0.062f * io.DisplaySize.x)
            type = ROTATE_SCREEN;

        const vec_t planNormals[] = { gContext.mModel.right, gContext.mModel.up, gContext.mModel.dir };

        for (unsigned int i = 0; i < 3 && type == NONE; i++)
        {
            // pickup plan
            vec_t pickupPlan = BuildPlan(gContext.mModel.position, planNormals[i]);

            const float len = IntersectRayPlane(gContext.mRayOrigin, gContext.mRayVector, pickupPlan);
            vec_t localPos = gContext.mRayOrigin + gContext.mRayVector * len - gContext.mModel.position;

            if (Dot(Normalized(localPos), gContext.mRayVector) > FLT_EPSILON)
                continue;

            float distance = localPos.Length() / gContext.mScreenFactor;
            if (distance > 0.9f && distance < 1.1f)
                type = ROTATE_X + i;
        }
        */
        return type;
    }

    static int GetRotateType()
    {
        ImGuiIO& io = ImGui::GetIO();
        int type = NONE;

        vec_t deltaScreen(io.MousePos.x - gContext.mScreenSquareCenter.x, io.MousePos.y - gContext.mScreenSquareCenter.y);
        float dist = deltaScreen.Length();
        if (dist >= 0.058f * io.DisplaySize.x && dist < 0.062f * io.DisplaySize.x)
            type = ROTATE_SCREEN;

        const vec_t planNormals[] = { gContext.mModel.right, gContext.mModel.up, gContext.mModel.dir};

        for (unsigned int i = 0; i < 3 && type == NONE; i++)
        {
            // pickup plan
            vec_t pickupPlan = BuildPlan(gContext.mModel.position, planNormals[i]);

            const float len = IntersectRayPlane(gContext.mRayOrigin, gContext.mRayVector, pickupPlan);
            vec_t localPos = gContext.mRayOrigin + gContext.mRayVector * len - gContext.mModel.position;

            if (Dot(Normalized(localPos), gContext.mRayVector) > FLT_EPSILON)
                continue;

            float distance = localPos.Length() / gContext.mScreenFactor;
            if (distance > 0.9f && distance < 1.1f)
                type = ROTATE_X + i;
        }

        return type;
    }

    static int GetMoveType(vec_t *gizmoHitProportion)
    {
        ImGuiIO& io = ImGui::GetIO();
        int type = NONE;

        // screen
        if (io.MousePos.x >= gContext.mScreenSquareMin.x && io.MousePos.x <= gContext.mScreenSquareMax.x &&
            io.MousePos.y >= gContext.mScreenSquareMin.y && io.MousePos.y <= gContext.mScreenSquareMax.y)
            type = MOVE_SCREEN;

        // compute
        for (unsigned int i = 0; i < 3 && type == NONE; i++)
        {
            const int planNormal = (i + 2) % 3;
            const int nextPlan = (i + 1) % 3;

            vec_t cameraEyeToGizmo = Normalized(gContext.mModel.position - gContext.mCameraEye);
            const bool belowAxisLimit = (fabsf(cameraEyeToGizmo.Dot3(direction[i])) < angleLimit);
            const bool belowPlaneLimit = (fabsf(cameraEyeToGizmo.Dot3(direction[planNormal])) > planeLimit);

            const float len = IntersectRayPlane(gContext.mRayOrigin, gContext.mRayVector, BuildPlan(gContext.mModel.position, direction[planNormal]));
            vec_t posOnPlan = gContext.mRayOrigin + gContext.mRayVector * len;

            const float dx = direction[i].Dot3((posOnPlan - gContext.mModel.position) * (1.f / gContext.mScreenFactor));
            const float dy = direction[nextPlan].Dot3((posOnPlan - gContext.mModel.position) * (1.f / gContext.mScreenFactor));
            if (belowAxisLimit && dy > -0.1f && dy < 0.1f && dx > 0.1f  && dx < 1.f)
                type = MOVE_X + i;

            if (belowPlaneLimit && dx >= quadUV[0] && dx <= quadUV[4] && dy >= quadUV[1] && dy <= quadUV[3])
                type = MOVE_XY + i;

            if (gizmoHitProportion)
                *gizmoHitProportion = vec_t(dx, dy, 0.f);
        }
        return type;
    }

    static void HandleTranslation(float *matrix, float *deltaMatrix, int& type)
    {
        ImGuiIO& io = ImGui::GetIO();

        // move
        if (gContext.mbUsing)
        {
            const float len = IntersectRayPlane(gContext.mRayOrigin, gContext.mRayVector, gContext.mTranslationPlan);
            vec_t newPos = gContext.mRayOrigin + gContext.mRayVector * len;
            vec_t newOrigin = newPos - gContext.mRelativeOrigin * gContext.mScreenFactor;
            vec_t delta = newOrigin - gContext.mModel.position;

            // 1 axis constraint
            if (gContext.mCurrentOperation >= MOVE_X && gContext.mCurrentOperation <= MOVE_Z)
            {
                int axisIndex = gContext.mCurrentOperation - MOVE_X;
                const vec_t& axisValue = gContext.mModel.line[axisIndex];
                float lengthOnAxis = Dot(axisValue, delta);
                delta = axisValue * lengthOnAxis;
            }

            // compute matrix & delta
            gContext.mTranslationPlanOrigin += delta;
            matrix_t deltaMatrixTranslation;
            deltaMatrixTranslation.translation(delta);
            if (deltaMatrix)
                memcpy(deltaMatrix, deltaMatrixTranslation.m16, sizeof(float) * 16);
            matrix_t res = *(matrix_t*)matrix * deltaMatrixTranslation;
            *(matrix_t*)matrix = res;

            if (!io.MouseDown[0])
                gContext.mbUsing = false;

            type = gContext.mCurrentOperation;
        }
        else
        {
            // find new possible way to move
            vec_t gizmoHitProportion;
            type = GetMoveType(&gizmoHitProportion);
            if (io.MouseDown[0] && type != NONE)
            {
                gContext.mbUsing = true;
                gContext.mCurrentOperation = type;
                const vec_t movePlanNormal[] = { gContext.mModel.up, gContext.mModel.dir, gContext.mModel.right, gContext.mModel.dir, gContext.mModel.up, gContext.mModel.right, -gContext.mCameraDir };
                // pickup plan
                gContext.mTranslationPlan = BuildPlan(gContext.mModel.position, movePlanNormal[type - MOVE_X]);
                const float len = IntersectRayPlane(gContext.mRayOrigin, gContext.mRayVector, gContext.mTranslationPlan);
                gContext.mTranslationPlanOrigin = gContext.mRayOrigin + gContext.mRayVector * len;
                gContext.mMatrixOrigin = gContext.mModel.position;

                gContext.mRelativeOrigin = (gContext.mTranslationPlanOrigin - gContext.mModel.position) * (1.f / gContext.mScreenFactor);
            }
        }
    }

    static void HandleScale(float *matrix, float *deltaMatrix, int& type)
    {
        ImGuiIO& io = ImGui::GetIO();

        // move
        if (gContext.mbUsing)
        {
            const float len = IntersectRayPlane(gContext.mRayOrigin, gContext.mRayVector, gContext.mTranslationPlan);
            vec_t newPos = gContext.mRayOrigin + gContext.mRayVector * len;
            vec_t newOrigin = newPos - gContext.mRelativeOrigin * gContext.mScreenFactor;
            vec_t delta = newOrigin - gContext.mModel.position;
            /*
            // 1 axis constraint
            if (gContext.mCurrentOperation >= MOVE_X && gContext.mCurrentOperation <= MOVE_Z)
            {
                int axisIndex = gContext.mCurrentOperation - MOVE_X;
                const vec_t& axisValue = gContext.mModel.line[axisIndex];
                float lengthOnAxis = Dot(axisValue, delta);
                delta = axisValue * lengthOnAxis;
            }

            // compute matrix & delta
            gContext.mTranslationPlanOrigin += delta;
            matrix_t deltaMatrixTranslation;
            deltaMatrixTranslation.translation(delta);
            if (deltaMatrix)
                memcpy(deltaMatrix, deltaMatrixTranslation.m16, sizeof(float) * 16);
            matrix_t res = *(matrix_t*)matrix * deltaMatrixTranslation;
            *(matrix_t*)matrix = res;
            */
            if (!io.MouseDown[0])
                gContext.mbUsing = false;

            type = gContext.mCurrentOperation;
        }
        else
        {
            // find new possible way to move
            vec_t gizmoHitProportion;
            type = GetMoveType(&gizmoHitProportion);
            if (io.MouseDown[0] && type != NONE)
            {
                gContext.mbUsing = true;
                gContext.mCurrentOperation = type;
                const vec_t movePlanNormal[] = { gContext.mModel.up, gContext.mModel.dir, gContext.mModel.right, gContext.mModel.dir, gContext.mModel.up, gContext.mModel.right, -gContext.mCameraDir };
                // pickup plan
                /*
                gContext.mTranslationPlan = BuildPlan(gContext.mModel.position, movePlanNormal[type - MOVE_X]);
                const float len = IntersectRayPlane(gContext.mRayOrigin, gContext.mRayVector, gContext.mTranslationPlan);
                gContext.mTranslationPlanOrigin = gContext.mRayOrigin + gContext.mRayVector * len;
                gContext.mMatrixOrigin = gContext.mModel.position;

                gContext.mRelativeOrigin = (gContext.mTranslationPlanOrigin - gContext.mModel.position) * (1.f / gContext.mScreenFactor);
                */
            }
        }
    }

    static float ComputeAngleOnPlan()
    {
        const float len = IntersectRayPlane(gContext.mRayOrigin, gContext.mRayVector, gContext.mTranslationPlan);
        vec_t localPos = Normalized(gContext.mRayOrigin + gContext.mRayVector * len - gContext.mModel.position);

        vec_t perpendicularVector;
        perpendicularVector.Cross(gContext.mRotationVectorSource, gContext.mTranslationPlan);
        perpendicularVector.Normalize();
        float acosAngle = Clamp(Dot(localPos, gContext.mRotationVectorSource), -0.9999f, 0.9999f);
        float angle = acosf(acosAngle);
        angle *= (Dot(localPos, perpendicularVector) < 0.f) ? 1.f : -1.f;
        return angle;
    }

    static void HandleRotation(float *matrix, float *deltaMatrix, int& type)
    {
        ImGuiIO& io = ImGui::GetIO();

        if (!gContext.mbUsing)
        {
            type = GetRotateType();
            if (io.MouseDown[0] && type != NONE)
            {
                gContext.mbUsing = true;
                gContext.mCurrentOperation = type;
                const vec_t rotatePlanNormal[] = { gContext.mModel.right, gContext.mModel.up, gContext.mModel.dir, -gContext.mCameraDir };
                // pickup plan
                gContext.mTranslationPlan = BuildPlan(gContext.mModel.position, rotatePlanNormal[type - ROTATE_X]);

                const float len = IntersectRayPlane(gContext.mRayOrigin, gContext.mRayVector, gContext.mTranslationPlan);
                vec_t localPos = gContext.mRayOrigin + gContext.mRayVector * len - gContext.mModel.position;
                gContext.mRotationVectorSource = Normalized(localPos);
                gContext.mRotationAngleOrigin = ComputeAngleOnPlan();
            }
        }

        // move
        if (gContext.mbUsing)
        {
            gContext.mRotationAngle = ComputeAngleOnPlan();

            vec_t rotationAxisLocalSpace;
            rotationAxisLocalSpace.TransformVector(vec_t(gContext.mTranslationPlan.x, gContext.mTranslationPlan.y, gContext.mTranslationPlan.z, 0.f), gContext.mModelInverse);

            matrix_t deltaRotation;
            deltaRotation.RotationAxis(rotationAxisLocalSpace, gContext.mRotationAngle - gContext.mRotationAngleOrigin);
            gContext.mRotationAngleOrigin = gContext.mRotationAngle;

            *(matrix_t*)matrix = deltaRotation * gContext.mModel;

            if (deltaMatrix)
                *(matrix_t*)deltaMatrix = deltaRotation;

            if (!io.MouseDown[0])
                gContext.mbUsing = false;

            type = gContext.mCurrentOperation;
        }
    }

    void Mogwai(const float *view, const float *projection, ImGuizmo::MODE mode, float *matrix, float *deltaMatrix)
    {
        ComputeContext(view, projection, matrix);

        // set delta to identity
        if (deltaMatrix)
            ((matrix_t*)deltaMatrix)->SetToIdentity();
        // --
        int type = NONE;
        if (gContext.mbEnable)
        {
            switch (mode)
            {
            case ROTATE:
                HandleRotation(matrix, deltaMatrix, type);
                break;
            case TRANSLATE:
                HandleTranslation(matrix, deltaMatrix, type);
                break;
            case SCALE:
                HandleScale(matrix, deltaMatrix, type);
                break;
            }
        }

        switch (mode)
        {
        case ROTATE:
            DrawRotationGizmo(type);
            break;
        case TRANSLATE:
            DrawTranslationGizmo(type);
            break;
        case SCALE:
            DrawScaleGizmo(type);
            break;
        }
    }

};
