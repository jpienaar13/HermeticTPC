import pickle
import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import PchipInterpolator
from matplotlib.ticker import FixedLocator, FixedFormatter
from scipy.interpolate import UnivariateSpline
from matplotlib.ticker import LogLocator

enrichment = 0.9

def half_life_from_events_mv(
    Nsig,
    mass_kg,
    livetime_yr,
    enrichment=0.90,
    efficiency=1.0,
):
    NA = 6.02214076e23
    m136_g_per_mol = 135.907214

    mass_g = mass_kg * 1000
    N_xe136 = mass_g * enrichment / m136_g_per_mol * NA

    return np.log(2) * N_xe136 * efficiency * livetime_yr / Nsig

def load_limit_band(
    path,
    total_mass_tonnes,
    enrichment=0.9,
    spline_s=1e68,
):
    with open(path, "rb") as f:
        limits = pickle.load(f)

    years = np.array(sorted(limits.keys()))

    out = {"years": years}

    for key in ["median", "p16", "p84", "p025", "p975"]:
        mu90 = np.array([limits[x][key] for x in years])

        # mu90 is events / tonne / year
        Nsig90 = mu90 * total_mass_tonnes * years

        hl = half_life_from_events_mv(
            Nsig90,
            mass_kg=total_mass_tonnes * 1000,
            livetime_yr=years,
            enrichment=enrichment,
        )

        out[key] = hl

        spl = UnivariateSpline(
            years,
            hl,
            w=1 / years,
            s=spline_s,
        )
        out[f"{key}_smooth"] = spl(years_smooth)

    return out


# -----------------------------
# Load noptimal data only
# -----------------------------
mass_vol1 = 1.627
mass_vol2 = 1.16
total_mass_tonnes = mass_vol1 + mass_vol2
years_smooth = np.linspace(0, 12, 400)

band_2vol_2p6 = load_limit_band(
    "data/limits_by_livetime_twovolume_2p6.pkl",
    total_mass_tonnes=total_mass_tonnes,
    enrichment=enrichment,
)



# -----------------------------
# Interpolate median rate at 10 years
# -----------------------------
target_year = 10.0

sens_ind= np.argwhere(band_2vol_2p6['years']==target_year)
median_rate_10yr = band_2vol_2p6['median'][sens_ind]  


# -----------------------------
# Vary Xe-136 enrichment
# -----------------------------
# Enrichment fractions: 1% to 100%
enrichment = np.linspace(0.01, 1.0, 300)

# Linear scaling from 90%
T12 = median_rate_10yr * enrichment / 0.90


# -----------------------------
# Plot
# -----------------------------
plt.figure(figsize=(6, 5.5))

plt.plot(
    enrichment * 100,
    T12[0],
    color="black",
    linewidth=2,
)

ax0 = plt.gca()

# Reference sensitivities
nexo_sensitivity = 1.35e28
xlzd_sensitivity = 6.09e27

refs = [
    (nexo_sensitivity, "nEXO"),
    (xlzd_sensitivity, "XLZD"),
]

for yref, label in refs:
    # horizontal dashed line
    ax0.axhline(
        yref,
        color="lightgrey",
        linestyle="--",
        linewidth=1.5,
        zorder=0,
    )

    # enrichment where your curve intersects the reference sensitivity
    x_intersect = np.interp(yref, T12[0], enrichment * 100)

    # vertical dotted line down to lower plot edge
    ax0.vlines(
        x_intersect,
        ymin=5e27,
        ymax=yref,
        color="dimgray",
        linestyle=":",
        linewidth=1.5,
        zorder=0,
    )

    # label horizontal line
    ax0.annotate(
        label,
        xy=(11, yref),
        xytext=(11, yref * 1.04),
        fontsize=12,
        color="black",
        va="bottom",
    )
plt.yscale("log")
plt.xlim(10, 90)
plt.ylim(5e27, 3e28)

ax0.yaxis.set_major_locator(FixedLocator([1e28]))
ax0.yaxis.set_major_formatter(FixedFormatter([r"$10^{28}$"]))
ax0.yaxis.set_minor_locator(
    LogLocator(base=10, subs=np.arange(1, 10) * 0.1)
)
ax0.yaxis.set_minor_formatter(plt.NullFormatter())

plt.xlabel(r"$^{136}$Xe enrichment [%]")
plt.ylabel(r"$T^{0\nu\beta\beta}_{1/2}$ exclusion limit (90% CL) [yr]")

plt.tight_layout()
plt.savefig("enrichment_plot.pdf", bbox_inches="tight", dpi=300)
plt.show()