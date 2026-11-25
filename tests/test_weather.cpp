#include "../xaml_experiment/weather_state.h"
#include <iostream>
#include <limits>

int main() {
    using namespace layout;
    Stability gate;
    WeatherSample sample{216,48,152,48,136,40,2,0,0,2,2};
    if (gate.observe({})!=Readiness::waiting) return 1;
    for (int i=0; i<3; ++i) if (gate.observe(sample)!=Readiness::measuring) return 2;
    sample.gridWidth+=1;
    for (int i=0; i<3; ++i) if (gate.observe(sample)!=Readiness::measuring) return 3;
    if (gate.observe(sample)!=Readiness::ready) return 4;
    ++sample.children;
    if (gate.observe(sample)!=Readiness::measuring) return 5;
    sample.measuredCards=0;
    if (gate.observe(sample)!=Readiness::waiting) return 6;
    sample.measuredCards=1;
    for (int i=0; i<3; ++i) if (gate.observe(sample)!=Readiness::measuring) return 7;
    sample.width=std::numeric_limits<double>::quiet_NaN();
    if (gate.observe(sample)!=Readiness::waiting) return 8;
    gate.reset();
    std::cout<<"Stable layout requires four consecutive measured samples.\n";
}
