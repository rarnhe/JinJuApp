#include "../xaml_experiment/weather_state.h"
#include <iostream>
#include <limits>
#include <string_view>

int main(int argc, char** argv) {
    using namespace layout;
    if (argc==2 && std::string_view(argv[1])=="placement") {
        Box button{-12,0,216,48}, weather{4,4,136,40};
        auto right=besideWeather(button,weather,42,18);
        if (!right || right->x!=146 || right->y!=15 || !fits(*right,button)) return 11;
        if (besideWeather({0,0,152,48},weather,42,18)) return 12;
        if (besideWeather(button,weather,42,64) || besideWeather(button,weather,0,18)) return 13;
        weather.x=120;
        if (besideWeather(button,weather,42,18)) return 14;
        return 0;
    }
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
