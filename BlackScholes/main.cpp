#include <iostream>

#include "blackscholes.h"

PricingInfo pricing_info
{
    0.03,        // risk-free interest rate (3%)
    100.0,       // strike price
    100.0,       // underlying price (at-the-money)
    0.5,         // time to maturity (6 months)
    0.30,        // volatility (30%)
    OptionType::Call,
    6.0          // paid price
};

AxisInfo x_axis
{
    Axes::UnderlyingPrice,
    70.0,        // min underlying price
    3.0          // step size
};

AxisInfo y_axis
{
    Axes::Volatility,
    0.10,        // min volatility
    0.03         // step size
};

int main(int argc, char** argv)
{
    TApplication app("BlackScholes", &argc, argv);
    TCanvas canvas("BlackScholes Heatmap", "BlackScholes Heatmap", 800, 600);

    BlackScholes black_scholes(pricing_info);

    black_scholes.drawHeatmap(x_axis, y_axis, 15, 100);

    canvas.Draw();
    app.Run();

	return EXIT_SUCCESS;
}

