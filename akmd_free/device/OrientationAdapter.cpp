#include <math.h>
#include <utils/Log.h>

#include "device/OrientationAdapter.hpp"
#include "util.hpp"


namespace akmd {

OrientationAdapter::OrientationAdapter(ChipReader* accelerometer, ChipReader* magnetometer)
{
    this->accelerometer = accelerometer;
    this->magnetometer = magnetometer;
}

OrientationAdapter::~OrientationAdapter()
{
}

Vector OrientationAdapter::read() {
    Vector a = accelerometer->read();
    Vector a_filtered;
    earth_raw.x =  a.y;
    earth_raw.y = -a.x;//-a.x;
    earth_raw.z = -a.z;//-a.z;

    earth = acc.filter_lp2(earth_raw);

    //LOGD("EARTH: x=%d y=%d z=%d",(int)b.x,(int)b.y,(int)b.z);    
    Vector ref(-1, 0, 0);
    Vector o1 = earth.cross(ref);
    Vector o2 = earth.cross(o1);
    //LOGD("o1: x=%d y=%d z=%d",(int)o1.x,(int)o1.y,(int)o1.z);
    //LOGD("o2: x=%d y=%d z=%d",(int)o2.x,(int)o2.y,(int)o2.z);    

    /* Now project magnetic field on components o1 and o2. */
    Vector m = magnetometer->read();
    Vector mag_filtered; 
    Vector m1;

    m1.x = -m.y;
    m1.y = -m.x;
    m1.z =  m.z;

    mag_filtered = mag.filter_lp2(m1);

    //LOGD("m : x=%d y=%d z=%d",(int)m1.x,(int)m1.y,(int)m1.z);

    float o1l = mag_filtered.dot(o1) * o2.length();
    float o2l = mag_filtered.dot(o2) * o1.length();

    /* Establish the angle in E */
    float y = 180.0f - rad2deg(atan2f(o2l, o1l));

    /* pitch x*/
    float p = rad2deg(asinf(earth_raw.y / earth_raw.length()));

    if(earth.z < 0) {
        if (p < 0) {
            p = -180 - p;
        } else if (p > 0) {
            p = 180 - p;
        }
    }

    /* roll y*/
    float r = 90.0f - rad2deg(acosf(earth_raw.x / earth_raw.length()));
    
    
    //LOGD("Ar=%d Azimuth =%d XH=%d YH=%d pitch=%d roll=%d",(int)ar,(int)Azimuth,(int)XH, (int)YH,(int)p,(int)r);
    //LOGD("OR: y=%d p=%d r=%d",(int)y,(int)p,(int)r);
    return Vector(y, p, -r);
}

}
