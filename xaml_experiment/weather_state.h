#pragma once
#include "layout_policy.h"
#include <optional>

namespace layout {
struct WeatherSample {
    double width, height, panelWidth, panelHeight, gridWidth, gridHeight;
    unsigned children, rows, columns, cards, measuredCards;
    bool ready() const {
        return validSize(width,height) && validSize(panelWidth,panelHeight) &&
               validSize(gridWidth,gridHeight) && children && cards && measuredCards;
    }
    bool matches(WeatherSample const& other) const {
        const auto close=[](double a, double b) { return std::abs(a-b) < 0.5; };
        return close(width,other.width) && close(height,other.height) &&
               close(panelWidth,other.panelWidth) && close(panelHeight,other.panelHeight) &&
               close(gridWidth,other.gridWidth) && close(gridHeight,other.gridHeight) &&
               children==other.children && rows==other.rows && columns==other.columns &&
               cards==other.cards && measuredCards==other.measuredCards;
    }
};
enum class Readiness { waiting, measuring, ready };
class Stability {
public:
    void reset() { baseline.reset(); count=0; }
    Readiness observe(WeatherSample const& sample) {
        if (!sample.ready()) { reset(); return Readiness::waiting; }
        if (!baseline || !baseline->matches(sample)) { baseline=sample; count=0; }
        if (count<4) ++count;
        return count==4 ? Readiness::ready : Readiness::measuring;
    }
private:
    std::optional<WeatherSample> baseline;
    unsigned count=0;
};
}
