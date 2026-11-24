// chart.cpp
#include "chart.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>

static std::string num(double x) {
    std::ostringstream o;
    o.precision(1);
    o << std::fixed << x;
    return o.str();
}

void write_chart(const std::string& path, const std::string& title,
                 const std::string& nameA, const std::string& nameB,
                 const ZScores& zA, const ZScores& zB) {
    const int W = 1000, H = 560;
    const int left = 80, right = 30, top = 70, bottom = 80;
    const double plotW = W - left - right, plotH = H - top - bottom;

    // y range: must include the +-2 lines and 0
    double lo = -2.0, hi = 2.0;
    for (int k = 0; k < 13; k++) {
        lo = std::min(lo, std::min(zA[k], zB[k]));
        hi = std::max(hi, std::max(zA[k], zB[k]));
    }
    double pad = (hi - lo) * 0.08;
    lo -= pad;
    hi += pad;

    // pick a tick step of 1, 2 or 5 times a power of ten
    double rough = (hi - lo) / 8.0;
    double mag = std::pow(10.0, std::floor(std::log10(rough)));
    double step = mag;
    if (rough / mag > 5) step = 10 * mag;
    else if (rough / mag > 2) step = 5 * mag;
    else if (rough / mag > 1) step = 2 * mag;

    // y pixel for a value
    #define YPX(v) (top + (hi - (v)) / (hi - lo) * plotH)

    std::ofstream f(path.c_str());
    f << "<svg xmlns='http://www.w3.org/2000/svg' width='" << W << "' height='" << H
      << "' font-family='Arial, sans-serif'>\n";
    f << "<rect width='100%' height='100%' fill='white'/>\n";
    f << "<text x='" << W / 2 << "' y='30' text-anchor='middle' font-size='18' font-weight='bold'>"
      << title << "</text>\n";

    // grid lines and y labels
    for (double v = std::ceil(lo / step) * step; v <= hi; v += step) {
        double y = YPX(v);
        f << "<line x1='" << left << "' y1='" << y << "' x2='" << W - right << "' y2='" << y
          << "' stroke='#dddddd'/>\n";
        f << "<text x='" << left - 8 << "' y='" << y + 4 << "' text-anchor='end' font-size='12'>"
          << num(v) << "</text>\n";
    }

    // zero line
    f << "<line x1='" << left << "' y1='" << YPX(0.0) << "' x2='" << W - right << "' y2='"
      << YPX(0.0) << "' stroke='black'/>\n";

    // bars
    double groupW = plotW / 13.0;
    double barW = groupW * 0.35;
    for (int k = 0; k < 13; k++) {
        double gx = left + k * groupW;
        double vals[2] = {zA[k], zB[k]};
        const char* col[2] = {"#2f6db5", "#e07b24"};
        for (int s = 0; s < 2; s++) {
            double x = gx + groupW * 0.1 + s * barW;
            double y0 = YPX(0.0), y1 = YPX(vals[s]);
            f << "<rect x='" << x << "' y='" << std::min(y0, y1) << "' width='" << barW
              << "' height='" << std::fabs(y1 - y0) << "' fill='" << col[s] << "'/>\n";
        }
        f << "<text x='" << gx + groupW / 2 << "' y='" << H - bottom + 20
          << "' text-anchor='middle' font-size='13'>" << k + 1 << "</text>\n";
    }

    // dashed significance lines at +2 and -2
    for (int s = -1; s <= 1; s += 2) {
        double y = YPX(2.0 * s);
        f << "<line x1='" << left << "' y1='" << y << "' x2='" << W - right << "' y2='" << y
          << "' stroke='red' stroke-dasharray='6,4' stroke-width='1.5'/>\n";
    }
    f << "<text x='" << W - right - 4 << "' y='" << YPX(2.0) - 4
      << "' text-anchor='end' font-size='11' fill='red'>Z = +2</text>\n";
    f << "<text x='" << W - right - 4 << "' y='" << YPX(-2.0) + 14
      << "' text-anchor='end' font-size='11' fill='red'>Z = -2</text>\n";

    // axis labels
    f << "<text x='" << left + plotW / 2 << "' y='" << H - bottom + 45
      << "' text-anchor='middle' font-size='14'>Triad class</text>\n";
    f << "<text transform='translate(22," << top + plotH / 2
      << ") rotate(-90)' text-anchor='middle' font-size='14'>Z-score</text>\n";

    // legend
    f << "<rect x='" << left + 10 << "' y='" << top - 28 << "' width='14' height='14' fill='#2f6db5'/>\n";
    f << "<text x='" << left + 30 << "' y='" << top - 16 << "' font-size='13'>" << nameA << "</text>\n";
    f << "<rect x='" << left + 190 << "' y='" << top - 28 << "' width='14' height='14' fill='#e07b24'/>\n";
    f << "<text x='" << left + 210 << "' y='" << top - 16 << "' font-size='13'>" << nameB << "</text>\n";

    f << "</svg>\n";
    #undef YPX
}
