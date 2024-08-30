#pragma once

#include <iostream>
#include <stdexcept>
#include <utility>
#include <cmath>
#include <numeric>
#include <variant>

#include "TH2D.h"
#include "TStyle.h"
#include "TApplication.h"
#include "TCanvas.h"


// Enum to hold the Put/Call
enum class OptionType
{
    Put, 
    Call
};

// Enum to hold the axes
enum class Axes
{
    InterestRate,      // 0
    UnderlyingPrice,   // 1
    StrikePrice,       // 2
    Time,              // 3
    Volatility,        // 4
};

// Type aliases for clarity

using Rate = double;
using Price = double;
using Time = double;
using Index = std::size_t;
using Volatility = double;
using AxisName = const char*;

// Axis names for showing them later in the rendering
const std::vector<AxisName> axisNames
{
    "Interest Rate",     // Axes::InterestRate
    "Underlying Price",  // Axes::UnderlyingPrice
    "Strike Price",      // Axes::StrikePrice
    "Time",              // Axes::Time
    "Volatility"         // Axes::Volatility
};


// Pricing variables required for the pricing
struct PricingInfo
{
    Rate interest_rate{};       //  r          : Represents the interest rate (is the theoretical rate of return on an investment with zero risk)
    Price underlying_price{};   //  S          : Represents the current underlying price (current market price of the asset)
    Price strike_price{};       //  K          : Represents the current strike price (fixed price at which the owner of a call option can buy, or the owner of a put option can sell, the underlying asset).
    Time time{};                //  T          : Represents the time remaning until the option's expiration (Years).
    Volatility volatility{};    //  σ          : Represents the volatility (variation in the price of the underlying asset over time).
    OptionType type{};          //  Call / Put : Represents the type (can either be a Call or a Put).
    Price paid_price{};         //  $          : Represents the paid price
};

// Struct to hold the information about the axis such as the PricingInfo element it is related to, the initial coordinates and the difference between each element

struct AxisInfo
{
    Axes axis{};
    double initial_coordinate{};
    double axis_difference{};
};

// Amount of shares per contract, can be modified but 100 is commonly used
constexpr std::size_t SHARES_PER_CONTRACT{ 100 };

// Class to handle errors related to pricing variables
class BlackScholesException : public std::runtime_error 
{
public:
    explicit BlackScholesException(const std::string& message) : std::runtime_error(message) {}
};

// Black-Scholes class
class BlackScholes
{
private:
    PricingInfo pricing_info_{};               // Holds the variables required for the pricing
    std::unique_ptr<TH2D> heatmap_{};          // Holds the heatmap
    std::vector<double> x_axis_vector_{};      // Vector used for the heatmap's X-Axis
    std::vector<double> y_axis_vector_{};      // Vector used for the heatmap's Y-Axis
public:
    explicit BlackScholes(const PricingInfo& pricing_info);
    [[nodiscard]] Price getPricing() const;
    [[nodiscard]] double calculatePnl(double current_price, int number_of_contracts) const;
    void drawHeatmap(AxisInfo& x_axis, AxisInfo& y_axis, std::size_t amount, std::size_t number_of_contracts);
private:
    void validateInfo(const PricingInfo& pricing_info) const;
    void setupHeatmap(Axes& x_axis, Axes& y_axis, double& initial_x, double& x_axis_difference, double& initial_y, double& y_axis_difference, std::size_t& amount);
    std::vector<double> getAxisVector(double& axis_difference, double& initial_value, std::size_t& amount);
    inline [[nodiscard]] const AxisName getAxisName(Axes& axes) const noexcept;
    void updatePricingInfo(const Axes& axes, const double& value);
};

inline [[nodiscard]] double normalCdf(double x) noexcept;