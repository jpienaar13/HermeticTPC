from pathlib import Path
import sys
PROJECT_ROOT = Path.cwd().parents[0]
sys.path.insert(0, str(PROJECT_ROOT))

import numpy as np
import argparse
import matplotlib.pyplot as plt

other_sensitivies = [
    ("nEXO", 7.4e27, 9.95, 1.1),
    ("NEXT-HD", 2.2e27, 9.95, 1.75),
    ("XLZD", 5.7e27, 11.95, 1.2)
]

def plot_double_beta_limit(ax):
    
    dirpath = PROJECT_ROOT / "common_plots" / "data"
    npfile  = np.load(dirpath / "t12s.npz")

    exposures = npfile["exposures"]
    median    = npfile["median"]

    ax.plot(exposures, median, color='maroon', linestyle='--')

    for elm in other_sensitivies:
        ax.scatter(elm[2], elm[1], marker='x', color='black')
        ax.text(elm[2]-elm[3], elm[1]/1.1, elm[0], fontsize=6)

    ax.set_yscale("log")
    ax.set_xlabel("Exposure time (yr)")
    ax.set_ylabel(rf"Sensitivity (yr)")
    ax.set_ylim(5e25, 3e29);
    ax.set_xlim(0,14)

    ax.grid("on", which='both', axis='both', color='grey', alpha=0.45)

def main():

    parser = argparse.ArgumentParser(description="Plot energy spectra")
    
    parser.add_argument(
        "fig_height",
        type=float,
        help="fig_height"
    )

    parser.add_argument(
        "fig_width",
        type=float,
        help="fig_width"
    )

    args = parser.parse_args()
    fig_height = args.fig_height
    fig_width  = args.fig_width

    fig, ax = plt.subplots(figsize=(fig_height, fig_width))
    plot_double_beta_limit(ax)
    plt.tight_layout()
    plt.show();

if __name__ == "__main__":
    main() 
