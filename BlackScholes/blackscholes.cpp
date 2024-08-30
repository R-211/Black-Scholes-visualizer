#include "blackscholes.h"

BlackScholes::BlackScholes(const PricingInfo& pricing_info)
{
    validateInfo(pricing_info);
    pricing_info_= pricing_info;
}

// @validateInfo : Checks whether the parameters given for the Black-Scholes model are valid
void BlackScholes::validateInfo(const PricingInfo& pricing_info) const 
{
    if (pricing_info.time <= 0.0) throw BlackScholesException("[!] Time must be greater than zero.");
    if (pricing_info.volatility < 0.0)  throw BlackScholesException("[!] Volatility (sigma) must be greater than or equal to zero.");
    if (pricing_info.underlying_price <= 0.0) throw BlackScholesException("[!] Underlying price must be greater than zero.");
    if (pricing_info.strike_price <= 0.0)  throw BlackScholesException("[!] Strike price must be greater than zero.");
    if (pricing_info.interest_rate < 0.0)  throw BlackScholesException("[!] Interest rate must be greater than or equal to zero.");
    if (pricing_info.paid_price < 0.0) throw BlackScholesException("[!] Paid price can't be negative");
}

/* @getPricing : Gets the option price based in the Black - Scholes model
*  -. d1 : Measures how far the stock price is from the strike price.
*  -. d2 : Measures the probability the option will be in-the-money at expiration.
*/
[[nodiscard]] Price BlackScholes::getPricing() const
{
    const auto d1 = (log(pricing_info_.underlying_price / pricing_info_.strike_price) + (pricing_info_.interest_rate + (pricing_info_.volatility * pricing_info_.volatility) / 2.0) * pricing_info_.time) / (pricing_info_.volatility * std::sqrt(pricing_info_.time));
    const auto d2 = d1 - pricing_info_.volatility * std::sqrt(pricing_info_.time);

    switch (pricing_info_.type)
    {
    case OptionType::Call:
        return pricing_info_.underlying_price * normalCdf(d1) - pricing_info_.strike_price * std::exp(-pricing_info_.interest_rate * pricing_info_.time) * normalCdf(d2);
    case OptionType::Put:
        return pricing_info_.strike_price * std::exp(-pricing_info_.interest_rate * pricing_info_.time) * normalCdf(-d2) - pricing_info_.underlying_price * normalCdf(-d1);
    default:
        throw BlackScholesException("[!] Invalid option type: must be Call or Put.");
    }
}

// @calculatePnl : Calculates the PnL based in the CallType
[[nodiscard]] double BlackScholes::calculatePnl(double current_price, int number_of_contracts) const
{
    if (number_of_contracts <= 0) throw BlackScholesException("[!] Number of contracts must be positive.");

    return (current_price - pricing_info_.paid_price) * SHARES_PER_CONTRACT * number_of_contracts;
}

// @drawHeatMap : Draws a simple pnl-based heatmap based in the PnL and the Black-Scholes input given

void BlackScholes::drawHeatmap(AxisInfo& x_axis, AxisInfo& y_axis, std::size_t amount, std::size_t number_of_contracts)
{
    if (number_of_contracts <= 0) throw BlackScholesException("[!] Number of contracts must be greater than zero");
    
    setupHeatmap(x_axis.axis, y_axis.axis, x_axis.initial_coordinate, x_axis.axis_difference,y_axis.initial_coordinate, y_axis.axis_difference, amount);

    // Variables to keep track of the smallest and biggest PnL values
    double min_pnl = std::numeric_limits<double>::max();
    double max_pnl = std::numeric_limits<double>::lowest();

    for (Index i{ 0 }; i < x_axis_vector_.size(); ++i)
    {
        for (Index j{ 0 }; j < y_axis_vector_.size(); ++j)
        {
            updatePricingInfo(x_axis.axis, x_axis_vector_[i]);
            updatePricingInfo(y_axis.axis, y_axis_vector_[j]);

            const Price current_price = getPricing();
            const Price pnl = calculatePnl(current_price, number_of_contracts);
           
            heatmap_->SetBinContent(i + 1, j + 1, pnl);
            min_pnl = std::min(min_pnl, pnl);
            max_pnl = std::max(max_pnl, pnl);
        }
        // Reset y-axis parameter to its initial value
        updatePricingInfo(y_axis.axis, y_axis.initial_coordinate);
    }

    gStyle->SetPalette(kBird);

    heatmap_->GetZaxis()->SetRangeUser(min_pnl, max_pnl);
    heatmap_->Draw("COLZ");
}

void BlackScholes::updatePricingInfo(const Axes& axes, const double& value)
{
    switch (axes)
    {
    case Axes::InterestRate:
        pricing_info_.interest_rate = value;
        break;
    case Axes::UnderlyingPrice:
        pricing_info_.underlying_price = value;
        break;
    case Axes::Time:
        pricing_info_.time = value;
        break;
    case Axes::Volatility:
        pricing_info_.volatility = value;
        break;
    case Axes::StrikePrice:
        pricing_info_.strike_price = value;
        break;
    default:
        throw BlackScholesException("[!] Invalid axis parameter");
    }
}

// @setupHeatmap : Gets the axes vectors and sets the heatmap up
void BlackScholes::setupHeatmap(Axes& x_axis, Axes& y_axis, double& initial_x, double& x_axis_difference, double& initial_y, double& y_axis_difference, std::size_t& amount)
{
    x_axis_vector_ = getAxisVector(x_axis_difference,initial_x, amount);
    y_axis_vector_ = getAxisVector(y_axis_difference,initial_y, amount);

    heatmap_ = std::make_unique<TH2D>(std::move(TH2D("BS Heatmap", "", x_axis_vector_.size(), x_axis_vector_.front(), x_axis_vector_.back(), y_axis_vector_.size(), y_axis_vector_.front(), y_axis_vector_.back())));
    heatmap_->SetXTitle(getAxisName(x_axis));
    heatmap_->SetYTitle(getAxisName(y_axis));
    heatmap_->SetStats(false);
}

// @getAxisVector : Fills the vector based in the difference between elements, the initial value and the amount of elements
std::vector<double> BlackScholes::getAxisVector(double& axis_difference, double& initial_value, std::size_t& amount)
{
    if (axis_difference <= 0) throw BlackScholesException("[!] Difference must be greater than 0");
    if(amount <= 0) throw BlackScholesException("[!] Amount must be greater than 0");

    std::vector<double> axis_vector(amount);

    for (Index i{ 0 }; i < amount; ++i)
    {
        axis_vector[i] = initial_value + i * axis_difference;
    }
    return axis_vector;
}

/* @getAxisName : Gets the string associated with the Axes enum passed
*  -. This basically passes the enum Axes to int and gets the string associated to it in axisNames via index
*/ 
inline [[nodiscard]] const AxisName BlackScholes::getAxisName(Axes& axes) const noexcept
{
    return axisNames[static_cast<std::underlying_type_t<Axes>>(axes)];
}

// @normalCdf : Calculates the normal cumulative distribution
inline [[nodiscard]] double normalCdf(double x) noexcept
{
    return 0.5 * std::erfc(-x / std::sqrt(2.0));
}
