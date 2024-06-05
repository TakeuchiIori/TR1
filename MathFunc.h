#pragma once
#include "Matrix4x4.h"
#include <assert.h>
#include <cmath>
#include <math.h>
#include <iostream>
#include <vector>
#include<cmath>
#include<algorithm>
#include <corecrt_math_defines.h>
#include "Vector3.h"
#include <numbers>
using namespace std::numbers;
// Vector3 : 加算
Vector3 Add(const Vector3& v1, const Vector3& v2);
// Vector3 : 減算
Vector3 Subtract(const Vector3& v1, const Vector3& v2);
// クロス積
Vector3 Cross(const Vector3& v1, const Vector3& v2);
// ベクトルの内積を計算する関数
float Dot(const Vector3& a, const Vector3& b);
// ベクトルの大きさの2乗を計算する関数
float magnitudeSquared(const Vector3& v);
// ベクトルの引き算を行う関数
Vector3 subtract(const Vector3& a, const Vector3& b);
// ベクトルの掛け算を行う関数
Vector3 multiply(const Vector3& v, float scalar);
// ベクトルの大きさを計算する関数
float magnitude(const Vector3& v);
// ベクトルの正規化を行う関数
Vector3 normalize(const Vector3& v);
// ベクトル間の距離を計算する関数
float distance(const Vector3& a, const Vector3& b);
// radianに変換
float Degree(float& degree);
// 1. 行列の加法 
Matrix4x4 Add(Matrix4x4 matrix1, Matrix4x4 matrix2);
// 2. 行列の減法
Matrix4x4 Subtract(Matrix4x4 matrix1, Matrix4x4 matrix2);
// 3. 行列の積
Matrix4x4 Multiply(Matrix4x4 matrix1, Matrix4x4 matrix2);
// 4. 逆行列
Matrix4x4 Inverse(Matrix4x4 matrix);
// 5. 転置行列
Matrix4x4 TransPose(Matrix4x4 matrix);
// 6. 単位行列
Matrix4x4 MakeIdentity4x4();
// 7.拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale);
// 8.平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);
// 9.座標変換
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);
// 10.回転行列
// 1. X軸回転行列
Matrix4x4 MakeRotateMatrixX(float radian);
// 2. Y軸回転行列
Matrix4x4 MakeRotateMatrixY(float radian);
// 3. Z軸回転行列
Matrix4x4 MakeRotateMatrixZ(float radian);

// 11. 3次元のアフィン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
//ベクトル変換データ
Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);