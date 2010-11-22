#include <math.h>
#include "math/Vector.hpp"
#include "math/Filter.hpp"

namespace akmd {

Filter::Filter()
{
    i = 0;
}

Vector Filter::filter_ma(Vector in) {
  
    Vector out;
    buffer[i] = in;    
    i = (i+1) % FILTER_SIZE;
    for (int j=0;j<FILTER_SIZE;j++)
    {
        out = out.add(buffer[j]);
    }   
    return out = out.divide(FILTER_SIZE);
}

Vector Filter::filter_lp2(Vector in) {
    
    Vector out;
    Vector p1;
    Vector p2;
    Vector p3;
    p1 = in.multiply(1+A+B);
    p2 = y_old1.multiply(A);
    p3 = y_old2.multiply(B);
    out = p1.sub(p2).sub(p3);
    y_old2 = y_old1;
    y_old1 = out;
    return out;
}

}




