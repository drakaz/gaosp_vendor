#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "Calibrator.hpp"
#include "device/ChipReader.hpp"
#include "device/DataPublisher.hpp"
#include "util.hpp"
#include "math/Vector.hpp"

namespace akmd {

class Akmd {
    private:
    /* Magnetic */
    ChipReader* magnetometer_reader;
    Calibrator magnetometer;

    /* Gyroscope */
    ChipReader* orientation_reader;
  
    /* Accelerometer */  
    ChipReader* accelerometer_reader;
    
    /* Temperature */
    ChipReader* temperature_reader;

    /* Data publishing */
    DataPublisher* result_writer;

    /* Desired start point of next measurement interval. */
    struct timeval next_update;

    /* Mininum and maximum delays between 2 measurements*/
    int delay_min;
    int delay_max;

    void calibrate_magnetometer(Vector a, Vector* m);
    void fill_result_vector(Vector o, Vector a, Vector m, short temperature, short* out);

    int parse_delay(char *d);

    public:
    Akmd(ChipReader* o, ChipReader* a, ChipReader* m, ChipReader* t, DataPublisher* r);
    ~Akmd();
    void measure();
    void sleep_until_next_update();

    void set_delays();

    void start();
    void stop();
};

}
