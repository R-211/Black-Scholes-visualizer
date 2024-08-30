# How to include the library

This code uses the [CERN ROOT C++ library](https://root.cern/), therefore it will be required to install it in order to run the code.

Visit the ROOT [installation page](https://root.cern/install/), choose the appropriate installation method for your operating system and follow the provided instructions to complete the installation.
Also ensure your project is set up to use C++17.

# Brief explanation of the project

This project generates a heatmap using CERN ROOT, based on Black-Scholes calculations and Profit and Loss (PnL) analysis. The heatmap visually represents PnL values using different colors:
  Higher PnL values are represented by yellow.
  Lower PnL values are represented by blue.

The resulting heatmap provides an intuitive, quick and simple way to visualize the distribution of PnL values across different parameters in the Black-Scholes model, where you 
can modify the axes and all the parameters of the formula.
