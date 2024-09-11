#include "mat4.h"
#include "mat3.h"
#include "vec4.h"

#include <stdio.h>
#include <math.h>

#define DEG2RAD 0.01745329252

Mat4 mat4_create(float szam)
{
    Mat4 mat;
    for (int i = 0; i < 16; i++)
        mat.data[i] = 0.0f;
    mat4_set(&mat, 0, 0, szam);
    mat4_set(&mat, 1, 1, szam);
    mat4_set(&mat, 2, 2, szam);
    mat4_set(&mat, 3, 3, szam);
    return mat;
}
Mat4 mat4_create2(float* data)
{
    Mat4 mat;
    for (int i = 0; i < 16; i++)
        mat.data[i] = data[i];
    return mat;
}

Mat4 mat4_createFromVec(Vec4 col0, Vec4 col1, Vec4 col2, Vec4 col3)
{
    Mat4 vissza;

    vissza.data[0] = col0.x;
    vissza.data[1] = col0.y;
    vissza.data[2] = col0.z;
    vissza.data[3] = col0.w;

    vissza.data[4] = col1.x;
    vissza.data[5] = col1.y;
    vissza.data[6] = col1.z;
    vissza.data[7] = col1.w;

    vissza.data[8] = col2.x;
    vissza.data[9] = col2.y;
    vissza.data[10] = col2.z;
    vissza.data[11] = col2.w;

    vissza.data[12] = col3.x;
    vissza.data[13] = col3.y;
    vissza.data[14] = col3.z;
    vissza.data[15] = col3.w;

    return vissza;
}

Mat4 mat4_createFromMat(Mat3 mat)
{
    Mat4 vissza = mat4_create(1);

    vissza.data[0] = mat.data[0];
    vissza.data[1] = mat.data[1];
    vissza.data[2] = mat.data[2];

    vissza.data[4] = mat.data[3];
    vissza.data[5] = mat.data[4];
    vissza.data[6] = mat.data[5];

    vissza.data[8] = mat.data[6];
    vissza.data[9] = mat.data[7];
    vissza.data[10] = mat.data[8];

    return vissza;
}

float mat4_get(Mat4* mat, int row, int col, float value)
{
    return mat->data[col * 4 + row];
}
void  mat4_set(Mat4* mat, int row, int col, float value)
{
    mat->data[col * 4 + row] = value;
}


float mat4_determinant(Mat4* mat)
{
    float det = 0;
    float* v = mat->data;//to keep things short
    Mat3 submat;

    submat = mat3_create2((float[]){
        v[5],v[6],v[7],
        v[9],v[10],v[11],
        v[13],v[14],v[15]
        });
    det += v[0] * mat3_determinant(&submat);
    
    submat = mat3_create2((float[]) {
        v[1], v[2], v[3],
        v[9], v[10], v[11],
        v[13], v[14], v[15]
    });
    det -= v[4] * mat3_determinant(&submat);

    submat = mat3_create2((float[]) {
        v[1], v[2], v[3],
        v[5], v[6], v[7],
        v[13], v[14], v[15]
    });
    det += v[8] * mat3_determinant(&submat);

    submat = mat3_create2((float[]) {
        v[1], v[2], v[3],
        v[5], v[6], v[7],
        v[9], v[10], v[11]
    });
    det -= v[12] * mat3_determinant(&submat);

    return det;
}


Mat4 mat4_sum(Mat4 egy, Mat4 katto)
{
    Mat4 mat;
    for (int i = 0; i < 16; i++)
        mat.data[i] = egy.data[i] + katto.data[i];
    return mat;
}
Mat4 mat4_multiply(Mat4 egy, Mat4 katto)
{
    Mat4 mat=mat4_create(0);
    
    for (int i = 0; i < 4; i++) //row
        for (int j = 0; j < 4; j++) //column
            for (int k = 0; k < 4; k++) //index
                mat.data[j * 4 + i] += egy.data[k * 4 + i] * katto.data[j * 4 + k];

    return mat;
}

Mat4 mat4_transpose(Mat4 mat)
{
    Mat4 mat2;
    for (int i = 0; i < 4; i++) //row
        for (int j = 0; j < 4; j++) //column
            mat2.data[i * 4 + j] = mat.data[j * 4 + i];
    return mat2;
}
Mat4 mat4_inverse(Mat4 mat)
{
    Mat4 newMat;
    Mat3 subMat;
    float* d = mat.data;//to keep things simple
    int sorIndex[3];
    int oszlopIndex[3];
    int temp;

    float szam = mat4_determinant(&mat);
    if (szam == 0)
        return mat;//mindegy mi megy vissza, mert úgy sincs inverze a mátrixnak
    szam = 1 / szam;

    //calculating adjugate matrix
    for (int i = 0; i < 4; i++)//column
    {
        for (int j = 0; j < 4; j++)//row
        {
            //setting up submatrix
            temp = 0;
            for (int k = 0; k < 4; k++) 
            {
                if (k == j)
                    continue;
                sorIndex[temp] = k;
                temp++;
            }
            temp = 0;
            for (int k = 0; k < 4; k++)
            {
                if (k == i)
                    continue;
                oszlopIndex[temp] = k;
                temp++;
            }
            
            subMat = mat3_create2((float[]){
                d[4 * oszlopIndex[0] + sorIndex[0]],d[4 * oszlopIndex[0] + sorIndex[1]],d[4 * oszlopIndex[0] + sorIndex[2]],
                d[4 * oszlopIndex[1] + sorIndex[0]],d[4 * oszlopIndex[1] + sorIndex[1]],d[4 * oszlopIndex[1] + sorIndex[2]],
                d[4 * oszlopIndex[2] + sorIndex[0]],d[4 * oszlopIndex[2] + sorIndex[1]],d[4 * oszlopIndex[2] + sorIndex[2]]
                });


            //set value
            if ((i + j) % 2 == 0)
                newMat.data[4 * i + j] = mat3_determinant(&subMat);
            else
                newMat.data[4 * i + j] = -1*mat3_determinant(&subMat);
        }
    }

    newMat = mat4_transpose(newMat);

    //finishing it
    for (int i = 0; i < 16; i++)
        newMat.data[i] *= szam;

    return newMat;
}

Mat4 mat4_scale(Mat4 mat, Vec3 scale)
{
    return mat4_multiply(
        mat,
        mat4_create2((float[]) {
            scale.x,0,0,0,
            0,scale.y,0,0,
            0,0,scale.z,0,
            0,0,0,1
    }));
}
Mat4 mat4_rotate(Mat4 mat, Vec3 axis, float angle)
{
    //https://learnopengl.com/Getting-started/Transformations
    
    axis = vec3_normalize(axis);

    float rad = DEG2RAD * angle;
    float cosine = cosf(rad);
    float oneMinusCosine = 1 - cosine;
    float sine = sinf(rad);
    
    float values[] = {
        cosine+axis.x*axis.x*oneMinusCosine, axis.y*axis.x*oneMinusCosine+axis.z*sine, axis.z*axis.x*oneMinusCosine-axis.y*sine,0,
        axis.x*axis.y*oneMinusCosine-axis.z*sine,cosine+axis.y*axis.y*oneMinusCosine,axis.z*axis.y*oneMinusCosine+axis.x*sine,0,
        axis.x*axis.z*oneMinusCosine+axis.y*sine,axis.y*axis.z*oneMinusCosine-axis.x*sine, cosine+axis.z*axis.z*oneMinusCosine,0,
        0,0,0,1
    };

    return mat4_multiply(mat, mat4_create2(values));
}
Mat4 mat4_translate(Mat4 mat, Vec3 pos)
{
    return mat4_multiply(mat, mat4_create2((float[]){1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, pos.x, pos.y, pos.z, 1}));
    //return mat4_sum(mat, mat4_create2((float[]) { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, pos.x, pos.y, pos.z, 1 }));
}

Mat4 mat4_lookAt(Vec3 pos, Vec3 direction, Vec3 up)
{
    //look mom, i did it by myself

    direction = vec3_normalize(direction);
    Vec3 right = vec3_normalize(vec3_cross(direction, up));
    up = vec3_normalize(vec3_cross(right, direction));
    direction = vec3_create2(-direction.x, -direction.y, -direction.z);

    Mat4 worldToView = mat4_create2((float[]) {
        right.x,up.x,direction.x,0,
        right.y,up.y,direction.y,0,
        right.z,up.z,direction.z,0,
        0,0,0,1
    }); //ortonormalt matrix inverze a transzponalt

    worldToView = mat4_translate(worldToView, vec3_create2(-pos.x, -pos.y, -pos.z));

    return worldToView;
}

Mat4 mat4_perspective(float fov, float aspectXY, float near, float far)
{
    //http://www.songho.ca/opengl/gl_projectionmatrix.html

    float t = tanf(DEG2RAD * 0.5f * fov) * near;
    float r = t * aspectXY;

    float values[] = {
        near/r,0,0,0,
        0,near/t,0,0,
        0,0,-(far+near)/(far-near),-1,
        0,0,-2*far*near/(far-near),0
    };

    return mat4_create2(values);
}
Mat4 mat4_ortho(float left, float right, float bottom, float top, float near, float far)
{
    //http://www.songho.ca/opengl/gl_projectionmatrix.html

    return mat4_create2((float[]) {
        2/(right-left),0,0,0,
        0,2/(top-bottom),0,0,
        0,0,-2/(far-near),0,
        -(right+left)/(right-left),-(top+bottom)/(top-bottom),-(far+near)/(far-near),1
    });
}

void mat4_print(Mat4* mat)
{
    for (int i = 0; i < 4; i++) {
        printf("%.2f %.2f %.2f %.2f\n", mat->data[i], mat->data[4 + i], mat->data[8 + i], mat->data[12 + i]);
    }
}