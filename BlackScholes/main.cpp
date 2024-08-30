#include <iostream>

#include "blackscholes.h"

PricingInfo pricing_info
{
    0.0,        	// Interest rate
    0.0,       		// Strike price
    0.0,       		// Underlying price 
    0.0,         	// Time to maturity 
    0.0,        	// Volatility
    OptionType::Call,	// Option type
    0.0          	// Paid price
};

AxisInfo x_axis
{
    Axes::UnderlyingPrice,
    0.0,        		// min x_axis value
    0.0          		// step size
};

AxisInfo y_axis
{
    Axes::Volatility,
    0.0,        		// min y_axis value
    0.0         		// step size
};

int main(int argc, char** argv)
{
    TApplication app("BlackScholes", &argc, argv);
    TCanvas canvas("BlackScholes Heatmap", "BlackScholes Heatmap", 800, 600);

    BlackScholes black_scholes(pricing_info);

    black_scholes.drawHeatmap(x_axis, y_axis, 0, 0);

    canvas.Draw();
    app.Run();

    return EXIT_SUCCESS;
}

