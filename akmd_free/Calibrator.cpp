/*
 * Copyright Antti S. Lankila, 2010, Licensed under GPL.
 */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include <utils/Log.h>

#include "Calibrator.hpp"
#include "math/Matrix.hpp"
#include "util.hpp"

namespace akmd {    

Calibrator::Calibrator(int validity, bool magnetic)
{
    this->validity = validity;
    this->magnetic = magnetic;
    reset();

     if(magnetic) {
        /* Check if calibration data file exsists. */
        calibrated = false;
        FILE *fad = fopen("/data/misc/akmd.dat","rb");
        if(fad != NULL) {
            fad_exists = true;
            LOGI("compass calibration file exsist");
            float akmd_data[6] = {0,};
            /* Read recorded calibration data from file. */
            fread(akmd_data, sizeof(float), 6, fad);
            center = Vector(akmd_data[0], akmd_data[1], akmd_data[2]);
            scale  = Vector(akmd_data[3], akmd_data[4], akmd_data[5]);
            fclose(fad);
            calibrated = true;
            LOGI("compass calibration data obtained from a file");
            LOGI("compass calibrated to offset=(%f %f %f) scale=(%f %f %f)", 
                center.x, center.y, center.z, scale.x, scale.y, scale.z);
        }
        else {
            fad_exists = false;
            LOGI("compass calibration file doesnt exsist");
        }
    }

}

void Calibrator::reset()
{
    fit_time = 0;
    center = Vector(0, 0, 0);
    scale  = Vector(1, 1, 1);
    old_nv = Vector(1, 0, 0);
    minimum_points_needed = PCR/4;
    idx = 0;
    memset(point_cloud, 0, sizeof(point_cloud));
}

Calibrator::~Calibrator()
{
}

void Calibrator::update(int time, Vector v)
{
    if(calibrated) {
        return;
    }
    const float SIMILARITY = 0.8f; /* 36 degrees' deviation, 10 vectors per circle */

    float vl = v.length();
    if (vl == 0.0f) {
        return;
    }

    Vector nv = v.divide(vl);

    /* Require sampled vectors to point to fairly different directions
     * before accepting another. */
    float similarity = nv.dot(old_nv);
    if (similarity > SIMILARITY) {
        return;
    }
    old_nv = nv;

    /* Check if we already have a vector nearly to same direction,
     * if so replace that one. This helps in not destroying our
     * history of vectors if user just jiggles device back and forth. */
    for (int i = 0; i < PCR; i ++) {
        if (point_cloud[i].time < time - validity) {
            idx = i;
            break;
        }

        Vector c = point_cloud[idx].v;
        Vector nc = c.divide(c.length());

        float similarity = nv.dot(nc);
        if (similarity > SIMILARITY) {
            idx = i;
            break;
        }
        
    }

    point_cloud[idx].time = time;
    point_cloud[idx].v = v;
    /* Round-robin vector reuse */
    idx = (idx + 1) & (PCR - 1);
}

bool Calibrator::try_fit(int time)
{
    if(calibrated) {
        return false;
    }
    int n = 0;
    for (int i = 0; i < PCR; i ++) {
        if (point_cloud[i].time >= time - validity) {
            n ++;
        }
    }

    /* Less than required bins filled with recent data? */
    if (n < minimum_points_needed) {
        return false;
    }

    /* Progressively require more and more points until saturated (at 50 %) */
    if (n < PCR/2) {
        minimum_points_needed = n;
    } else {
        minimum_points_needed = PCR/2;
    }

    fit_time = time;

    Matrix a = Matrix(n, 6);
    Matrix b = Matrix(n, 1);

    n = 0;
    for (int i = 0; i < PCR; i ++) {
        if (point_cloud[i].time < time - validity) {
            continue;
        }

        Vector v = point_cloud[i].v;

        b.set(n, 0, -v.x*v.x);
        a.set(n, 0, -2.0f*v.x);
        a.set(n, 1, v.y*v.y);
        a.set(n, 2, -2.0f*v.y);
        a.set(n, 3, v.z*v.z);
        a.set(n, 4, -2.0f*v.z);
        a.set(n, 5, 1.0f);

        n ++;
    }

    float *x = Matrix::leastSquares(&a, &b);
    if (x != NULL) {
        /* Do a little smell test on the values. */
        if (x[1] > 0.5f && x[1] < 2.0f && x[3] > 0.5f && x[3] < 2.0f) {
            center = Vector(x[0], x[2] / x[1], x[4] / x[3]);
            scale = Vector(1, sqrtf(x[1]), sqrtf(x[3]));
            LOGD("center x=%f y=%f z=%f",center.x,center.y,center.z);
            LOGD("scale x=%f y=%f z=%f",scale.x, scale.y, scale.z);
        }
        delete[] x;
    }
    /* Save calibration data to a file. */
    if(!calibrated && magnetic && center.x != 0 && center.y != 0 && center.z != 0) {
        if(!fad_exists) {
            float calibration_data[6] =  {center.x, center.y, center.z,
                                             scale.x, scale.y, scale.z};
            FILE *fac = fopen("/data/misc/akmd.dat","wb");
            if(fac != NULL) {
                fwrite(calibration_data, sizeof(float), 6, fac);
            }
            fclose(fac);
            calibrated = true;
            LOGI("compass calibrated to offset=(%f %f %f) scale=(%f %f %f)", 
                center.x, center.y, center.z, scale.x, scale.y, scale.z);
        }
        calibrated = true;
    }

    
    return true;
}

}
