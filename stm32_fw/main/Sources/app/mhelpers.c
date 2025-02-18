#include "mhelpers.h"
#include <string.h>
#include <math.h>

void FS_Integate3D(Vec3D_t *y, Vec3D_t *y_, Vec3D_t *x, Vec3D_t *x_, float h)
{
    for (int i = 0; i < FS_NUM_AXIS; i++)
    {
        FS_Integate(&y->x[i], y_->x[i], x->x[i], x_->x[i], h);
    }
}

void FS_Integate(float *y, float y_, float x, float x_, float h)
{
    *y = y_ + (x + x_) * h / 2.0;
}

// Quternion to World-to-Body
void FS_QuatToRot(Matrix3D_t *r, Quaternion_t *q)
{
    r->r[0][0] = 1 - 2 * (q->y * q->y + q->z * q->z);
    r->r[0][1] = 2 * (q->x * q->y + q->w * q->z);
    r->r[0][2] = 2 * (q->x * q->z - q->w * q->y);

    r->r[1][0] = 2 * (q->x * q->y - q->w * q->z);
    r->r[1][1] = 1 - 2 * (q->x * q->x + q->z * q->z);
    r->r[1][2] = 2 * (q->y * q->z + q->w * q->x);

    r->r[2][0] = 2 * (q->x * q->z + q->w * q->y);
    r->r[2][1] = 2 * (q->y * q->z - q->w * q->x);
    r->r[2][2] = 1 - 2 * (q->x * q->x + q->y * q->y);
}

void FS_MatTranspose(Matrix3D_t *r)
{
    for (int i = 0; i < FS_NUM_AXIS; i++)
    {
        for (int j = i + 1; j < FS_NUM_AXIS; j++)
        {
            float temp = r->r[i][j];
            r->r[i][j] = r->r[j][i];
            r->r[j][i] = temp;
        }
    }
}

void FS_MatMulVec(Vec3D_t *y, Matrix3D_t *yWx, Vec3D_t *x)
{
    for (int i = 0; i < FS_NUM_AXIS; i++)
    {
        y->x[i] = 0;
        for (int j = 0; j < FS_NUM_AXIS; j++)
        {
            y->x[i] += yWx->r[i][j] * x->x[j];
        }
    }
}

void FS_ScaleVec(float a, Vec3D_t *x)
{
    for (int i = 0; i < FS_NUM_AXIS; i++)
    {
        x->x[i] *= a;
    }
}

void FS_ZeroVec(Vec3D_t *x)
{
    memset(x->x, 0, sizeof(x->x));
}

float FS_NormVec(Vec3D_t *a)
{
    return sqrt(a->x[0] * a->x[0] + a->x[1] * a->x[1] + a->x[2] * a->x[2]);
}

void FS_RmaUpdate(RMA_t *a, float val)
{
    a->n++;
    float alpha = 1.0 / ((a->n < a->windowSz) ? a->n : a->windowSz);
    a->val += alpha * (val - a->val);
}

void FS_ConjQuat(Quaternion_t *q_, Quaternion_t *q)
{
    q_->w = q->w;
    q_->x = -q->x;
    q_->y = -q->y;
    q_->z = -q->z;
}

void FS_QuatMul(Quaternion_t *r, Quaternion_t *q1, Quaternion_t *q2)
{
    r->w = q1->w * q2->w - q1->x * q2->x - q1->y * q2->y - q1->z * q2->z;
    r->x = q1->w * q2->x + q1->x * q2->w + q1->y * q2->z - q1->z * q2->y;
    r->y = q1->w * q2->y - q1->x * q2->z + q1->y * q2->w + q1->z * q2->x;
    r->z = q1->w * q2->z + q1->x * q2->y - q1->y * q2->x + q1->z * q2->w;
}

void FS_VecRotQuat(Vec3D_t *x, Quaternion_t *q)
{
    Quaternion_t qx = {x->x[0], x->x[1], x->x[2], 0};
    Quaternion_t q_ = {-q->x, -q->y, -q->z, q->w};

    Quaternion_t q1;
    FS_QuatMul(&q1, &q_, &qx);

    Quaternion_t q2;
    FS_QuatMul(&q2, &q1, q);

    x->x[0] = q2.x;
    x->x[1] = q2.y;
    x->x[2] = q2.z;
}