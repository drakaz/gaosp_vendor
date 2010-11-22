#pragma once

#define FILTER_SIZE 5
#define A -1
#define B 0.06

namespace akmd {

class Filter {
    private:
    int size;
    Vector buffer[FILTER_SIZE];
    Vector y_old1;
    Vector y_old2;
    int i;
    

    public:
    Filter();
    Vector filter_ma(Vector in);
    Vector filter_lp2(Vector in);

};

}
