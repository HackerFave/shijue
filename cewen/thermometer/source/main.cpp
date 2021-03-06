#include "camera.h"
#include "engine.h"
#include "detect.h"
#include "measure.h"

int main(int argc, char *argv[])
{
    measure::initialize();
    detect::initialize();
    engine::initialize();
    camera::initialize();

    measure::registercallback(engine::pushmeasure);
    detect::registercallback(engine::pushdetect);
    engine::registercallback(engine::CALLBACK_DETECT, detect::push);
    engine::registercallback(engine::CALLBACK_MEASURE, measure::push);
    camera::registercallback(CAMERA_VISIBLE, engine::pushvisible);
    camera::registercallback(CAMERA_INFRARE, engine::pushinfrare);

    measure::start();
    detect::start();
    engine::start();
    camera::start();
}