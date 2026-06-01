from pathlib import Path
import sys
PROJECT_ROOT = Path.cwd().parents[0]
sys.path.insert(0, str(PROJECT_ROOT))

import numpy as np
import matplotlib.pyplot as plt
import h5py
import argparse

isotope_dict = {
    "U238": {
        "tex": r"$^{238}$U",
        "color": "royalblue",
    },
    "Th232": {
        "tex": r"$^{232}$Th",
        "color": "darkorange",
    },
    "Co60": {
        "tex": r"$^{60}$Co",
        "color": "crimson",
    },
    "Cs137": {
        "tex": r"$^{137}$Cs",
        "color": "purple",
    },
    "K40": {
        "tex": r"$^{40}$K",
        "color": "goldenrod",
    },
    "Kr85": {
        "tex": r"$^{85}$Kr",
        "color": "deepskyblue",
    },
    "Ra222": {
        "tex": r"$^{222}$Ra",
        "color": "saddlebrown",
    },
    "SN": {
        "tex": r"Solar $\nu$",
        "color": "green",
    },
    "Xe136": {
        "tex": r"$^{136}$Xe (3%)",
        "color": "red",
    },
    "Xe124": {
        "tex": r"$^{124}$Xe",
        "color": "fuchsia",
    }
}

# ----------
# Functions
# ----------
def energy_res(energy, a_res=0.317, b_res=1.7):
    resolution = a_res*np.sqrt(energy)+b_res*10**-3*energy
    return resolution

def gaussian(x, mu, sig):
    return (
        1.0 / (np.sqrt(2.0 * np.pi) * sig) * np.exp(-np.power((x - mu) / sig, 2.0) / 2)
    )

def apply_energy_smear(hist, e_edges, bins=1000):

    bw   = e_edges[1] - e_edges[0]
    xs = np.linspace(e_edges[0], e_edges[-1], bins)
    smeared_array = np.zeros(len(xs))

    for idx, val in enumerate(hist):
        E_val = e_edges[idx]
        if E_val != 0:
            sigma = energy_res(E_val)
            fun   = gaussian(xs, E_val, sigma) * val * bw
            smeared_array = np.add(smeared_array,fun)

    return xs, smeared_array

def inline_label(ax, text, x, y, idx, colour, fontsize=6, spacing=0.12):
    ax.text(
        x[idx], 
        y[idx] + spacing, 
        text, 
        fontsize=fontsize, 
        color=colour)


def plot_low_energy(ax):

    dirpath = PROJECT_ROOT / "common_plots" / "data"
    npfile  = np.load(dirpath / "low_e_outer.npz")
    npfile_enriched  = np.load(dirpath / "low_e_outer_enriched.npz")

    e_edges  = npfile["e_edges"]
    hist_arr = npfile["hist_arr"]

    # Get a single scalar for the materials background
    materials_spectra = []
    for idx in range(0,5):
        hist = hist_arr[idx]
        materials_spectra.append(hist)

    # Sum the materials up to 100 keV and then average
    materials_spectrum = np.sum(materials_spectra, axis=0)
    materials_bkg = np.mean(materials_spectrum[:-1])

    # Get spectra for intrinsics
    spectra = []
    for idx in range(5, 10):
        hist = hist_arr[idx]
        spectra.append(hist)

    # Get the enriched Xe136 from enriched npfile
    xe136_enriched = npfile_enriched["hist_arr"][8]

    total = np.sum(spectra, axis=0) + materials_bkg
    total_enriched = total - spectra[3] + xe136_enriched

    labels = ["Kr85", "Ra222", "SN", "Xe136", "Xe124"]
    txt_idxs = [75, 75, 75, 92, 55]
    spacings = [-0.12, 0.07, 0.3, -0.35 , 0.12]
    for idx, spectrum in enumerate(spectra):
        xs, smeared_hist = apply_energy_smear(spectrum, e_edges)
        ax.plot(
            xs, 
            smeared_hist, 
            label = isotope_dict[labels[idx]]["tex"], 
            color = isotope_dict[labels[idx]]["color"])
        label = isotope_dict[labels[idx]]["tex"]
        color = isotope_dict[labels[idx]]["color"]
        inline_label(
            ax, 
            label, 
            xs, 
            smeared_hist, 
            txt_idxs[idx], 
            colour=color,
            spacing=spacings[idx])

    xs, smeared_hist = apply_energy_smear(total, e_edges)
    label = "Total (Depleted)"
    ax.plot(xs, smeared_hist, color='black', label=label)
    inline_label(
        ax, 
        label, 
        xs, 
        smeared_hist, 
        65, 
        colour='black',
        spacing=1.5)

    xs, smeared_hist = apply_energy_smear(total_enriched, e_edges)
    label = "Total (Enriched)"
    ax.plot(xs, smeared_hist, color='black', label=label, linestyle='--')
    inline_label(
        ax, 
        label, 
        xs, 
        smeared_hist, 
        56, 
        colour='black',
        spacing=14)

    xs, smeared_hist = apply_energy_smear(xe136_enriched, e_edges)
    label=r"$^{136}$Xe (90%)"
    ax.plot(xs, smeared_hist, color='red', label=label, linestyle='--')
    inline_label(
        ax, 
        label, 
        xs, 
        smeared_hist, 
        91, 
        colour='red',
        spacing=-10)

    # Plot flat materials background
    ax.axhline(materials_bkg, label = "Materials", color='royalblue')

    ax.set_xlim(0,25)
    xticks = np.arange(0, 25, 1)
    ax.set_ylim(5e-2,1e2)
    ax.set_yscale("log")
    ax.set_xlabel("Energy (keV)")
    ax.set_ylabel(rf"Rate (tonne $\cdot$ year $\cdot$ keV)$^{-1}$")
    ax.set_xticks(xticks ,minor=True)
    ax.axvspan(1, 11, alpha=0.14, color='cyan')
    ax.text(3.4, 40, r'WIMP ROI', fontsize=8)

def plot_high_energy(ax):
    dirpath = PROJECT_ROOT / "common_plots" / "data"
    bkg_file = dirpath / "bkg_template.ii.h5"
    bkg_f    = h5py.File(bkg_file, 'r')
    bkg_template = bkg_f["templates/bkg_template"][:]

    two_neutrino_file = dirpath / "xe136_2vbb_template.ii.h5"
    two_neutrino_f    = h5py.File(two_neutrino_file, 'r')
    two_neutrino_template = two_neutrino_f["templates/xe136_2vbb_template"][:]

    neutrinoless_file = dirpath / "xe136_0vbb_template.ii.h5"
    neutrinoless_f    = h5py.File(neutrinoless_file, 'r')
    neutrinoless_template = neutrinoless_f["templates/xe136_0vbb_template"][:]

    bins = bkg_f["bins/0"][:]
    bin_mids = bins[:-1] + np.diff(bins)/2
    bw = np.mean(np.diff(bins))

    A = 0.02
    combined_template = bkg_template + two_neutrino_template + A*neutrinoless_template

    label = 'Total'
    colour = 'black'
    ax.plot(bin_mids, combined_template, label = label, color=colour)
    inline_label(
    ax, 
    label, 
    bin_mids, 
    combined_template, 
    48, 
    colour=colour,
    spacing=0.0002)

    label = 'Materials'
    colour = 'maroon'
    ax.plot(bin_mids, bkg_template, label = label, color=colour)
    inline_label(
    ax, 
    label, 
    bin_mids, 
    bkg_template, 
    33, 
    colour=colour,
    spacing=-0.000055)

    label = rf"$2\nu2\beta$"
    colour = 'red'
    ax.plot(bin_mids, two_neutrino_template, label = label, color=colour)
    inline_label(
    ax, 
    label, 
    bin_mids, 
    two_neutrino_template, 
    36, 
    colour=colour,
    spacing=0.005)

    ax.axvspan(2458-16, 2458+16, alpha=0.3, color='yellow')
    ax.text(2458-16, 0.04, rf"$0\nu2\beta$", fontsize=8)
    ax.text(2458-12.5, 0.026, rf"ROI", fontsize=8)

    label = rf"$0\nu2\beta$"
    colour = 'orange'
    nltemp = A*neutrinoless_template
    ax.plot(bin_mids, nltemp, label = rf"$0\nu2\beta$", color='orange')
    ax.fill_between(bin_mids, nltemp, np.zeros(len(bin_mids)), alpha=0.9, color=colour)
    inline_label(
    ax, 
    label, 
    bin_mids, 
    nltemp, 
    44, 
    colour='black',
    spacing=-0.0006,
    fontsize=7)

    ax.set_xlabel("Energy (keV)")
    ax.set_ylabel(rf"Rate (tonne $\cdot$ year $\cdot$ keV)$^{-1}$")
    ax.set_yscale("log")
    ax.set_xlim(2300, 2625)
    ax.set_ylim(1e-5, 1e-1)


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

    fig, axs = plt.subplots(nrows=1, ncols=2, figsize=(fig_height, fig_width))
    plot_low_energy(axs[0])
    plot_high_energy(axs[1])
    plt.tight_layout()
    plt.show();

if __name__ == "__main__":
    main() 