import numpy as np
import matplotlib.pyplot as plt
import pickle
from scipy.interpolate import UnivariateSpline
from matplotlib.ticker import LogLocator
import h5py
from scipy.signal import savgol_filter
from scipy.interpolate import PchipInterpolator
import pandas as pd

from numpy import pi, sqrt, exp, zeros, size, shape, array, append, flipud, gradient
from numpy import trapz, interp, loadtxt, log10, log, savetxt, vstack, transpose
from numpy import ravel,tile,mean,inf,nan,amin,amax
from scipy.ndimage.filters import gaussian_filter1d
from matplotlib.colors import LinearSegmentedColormap
from mpl_toolkits.axes_grid1.inset_locator import inset_axes
import cmasher as cmr
from scipy.interpolate import UnivariateSpline

def Floor_2D(data,filt=True,filt_width=3,Ex_crit=1e10):
    sig = data[1:,0]
    m = data[0,1:]
    n = size(m)
    ns = size(sig)
    Ex = flipud(transpose(data[1:,1:].T))
    Ex[Ex>Ex_crit] = nan
    Exmin = amin(Ex[Ex>0])
    Ex[Ex==0] = Exmin
    DY = zeros(shape=shape(Ex))
    for j in range(0,n):
        y = log10(Ex[:,j])
        if filt:
            y = gaussian_filter1d(gaussian_filter1d(y,sigma=3),filt_width)
            dy = gradient(y,log10(sig[2])-log10(sig[1]))
            dy = gaussian_filter1d(dy,filt_width)
        else:
            dy = gradient(y,log10(sig[2])-log10(sig[1]))

        DY[:,j] = dy
    NUFLOOR = zeros(shape=n)
    #for j in range(0,n):
    #    DY[:,j] = gaussian_filter1d(DY[:,j],filt_width)
    for j in range(0,n):
        for i in range(0,ns):
            if DY[ns-1-i,j]<=-2.0:
                i0 = ns-1-i
                i1 = i0+10
                NUFLOOR[j] = 10.0**interp(-2,DY[i0:i1+1,j],log10(sig[i0:i1+1]))
                DY[ns-1-i:-1,j] = nan
                break
    DY = -DY
    DY[DY<2] = 2
    return m,sig,NUFLOOR,DY

import matplotlib.patheffects as pe
def line_background(lw,col):
    return [pe.Stroke(linewidth=lw, foreground=col), pe.Normal()]

data = loadtxt('data/DLNuFloorXe_detailed_SI.txt')
Ex = flipud(data[1:,1:])
m,sig,NUFLOOR,DY = Floor_2D(data)

fig, (ax0, ax1) = plt.subplots(
    1, 2,
    figsize=(12, 5),
    constrained_layout=True
)

# ============================================================
# Panel 1: 0νββ sensitivity
# ============================================================

enrichment = 0.9

def half_life_from_events(
    Nsig,
    mass_kg=1000,
    livetime_yr=1,
    enrichment=0.90,
    efficiency=1.0,
):
    NA = 6.02214076e23  # mol^-1
    m136_g_per_mol = 135.907214  # Xe-136 molar mass

    mass_g = mass_kg * 1000

    N_xe136 = mass_g * enrichment / m136_g_per_mol * NA

    T12 = np.log(2) * N_xe136 * efficiency * livetime_yr / Nsig

    return T12

# -----------------------------
# HTPC Scenarios
# -----------------------------
with open("data/limits_by_livetime_newoptimal.pkl", "rb") as f:
    limits_1p5_noptimal = pickle.load(f)

exposure_1p5_noptimal = np.array(sorted(limits_1p5_noptimal.keys()))   # actually tonne-years
years_1p5_noptimal = exposure_1p5_noptimal / 1.64

median_1p5_noptimal = np.array([limits_1p5_noptimal[x]["median"] for x in exposure_1p5_noptimal])
hl_1p5_noptimal = half_life_from_events(median_1p5_noptimal, enrichment=enrichment)

with open("data/limits_by_livetime_newoptimistic.pkl", "rb") as f:
    limits_1p5_noptimistic = pickle.load(f)

exposure_1p5_noptimistic = np.array(sorted(limits_1p5_noptimistic.keys()))   # actually tonne-years
years_1p5_noptimistic = exposure_1p5_noptimistic / 1.64

median_1p5_noptimistic = np.array([limits_1p5_noptimistic[x]["median"] for x in exposure_1p5_noptimistic])
hl_1p5_noptimistic = half_life_from_events(median_1p5_noptimistic, enrichment=enrichment)

# -----------------------------
# Smooth on common calendar-year grid
# -----------------------------
xmin = max(years_1p5_noptimal.min(), years_1p5_noptimal.min())
xmax = min(years_1p5_noptimistic.max(), years_1p5_noptimistic.max())

years_smooth = np.linspace(xmin, xmax, 400)

weights_noptimal = 1 / years_1p5_noptimal
weights_noptimistic = 1 / years_1p5_noptimistic

spl_1p5_noptimal = UnivariateSpline(
    years_1p5_noptimal,
    hl_1p5_noptimal,
    w=weights_noptimal,
    s=1e68
)

spl_1p5_noptimistic = UnivariateSpline(
    years_1p5_noptimistic,
    hl_1p5_noptimistic,
    w=weights_noptimistic,
    s=1e68
)

smooth_1p5_noptimal = spl_1p5_noptimal(years_smooth)
smooth_1p5_noptimistic = spl_1p5_noptimistic(years_smooth)


ax0.plot(
    years_smooth,
    smooth_1p5_noptimal,
    color="black",
    linewidth=2,
    label="Nominal",
)

ax0.plot(
    years_smooth,
    smooth_1p5_noptimistic,
    color="black",
    linestyle="--",
    linewidth=2,
    label="Optimistic",
)

ax0.fill_between(
    years_smooth,
    smooth_1p5_noptimal,
    smooth_1p5_noptimistic,
    alpha=0.25,
)

# -----------------------------
# Other Projections
# -----------------------------

nexo_years = 10
nexo_sensitivity = 1.35e28

ax0.scatter(
    nexo_years,
    nexo_sensitivity,
    marker="D",
    s=120,
    facecolors="white",
    edgecolors="blue",
    linewidths=2,
    zorder=10,
    label="nEXO projection"
)

ax0.annotate(
    r"nEXO",
    (nexo_years, nexo_sensitivity),
    xytext=(15, -5),
    textcoords="offset points",
    fontsize=15,
    fontweight="bold",
)

# KamLAND2-Zen target: ~2e27 yr in 10 years
kamland2_years = 10
kamland2_sensitivity = 2.0e27

ax0.scatter(
    kamland2_years,
    kamland2_sensitivity,
    marker="s",
    s=110,
    facecolors="white",
    edgecolors="purple",
    linewidths=2,
    zorder=10,
    label="KamLAND2-Zen projection",
)

ax0.annotate(
    "KamLAND2-Zen",
    (kamland2_years, kamland2_sensitivity),
    xytext=(-80, 12),
    textcoords="offset points",
    fontsize=15,
    fontweight="bold",
    color="purple",
)

# NEXT-HD: ~1.2e27 yr in <5 years
nexthd_years = 5
nexthd_sensitivity = 1.2e27

ax0.scatter(
    nexthd_years,
    nexthd_sensitivity,
    marker="^",
    s=120,
    facecolors="white",
    edgecolors="orange",
    linewidths=2,
    zorder=10,
    label="NEXT-HD projection",
)

ax0.annotate(
    "NEXT-HD",
    (nexthd_years, nexthd_sensitivity),
    xytext=(12, -5),
    textcoords="offset points",
    fontsize=15,
    fontweight="bold",
    color="orange",
)

# -----------------------------
# XLZD band from two digitized curves
# -----------------------------
xlzd_low = np.loadtxt(
    "data/XLZD60t_nominal.csv",
    delimiter=",",
    skiprows=1,
)

xlzd_high = np.loadtxt(
    "data/XLZD60t_optimal.csv",
    delimiter=",",
    skiprows=1,
)

years_xlzd_low = xlzd_low[:, 0]
hl_xlzd_low = xlzd_low[:, 1]

years_xlzd_high = xlzd_high[:, 0]
hl_xlzd_high = xlzd_high[:, 1]

years_xlzd_smooth = np.linspace(years_xlzd_high[0], years_xlzd_high[-1], 400)

smooth_xlzd_low = UnivariateSpline(
    years_xlzd_low,
    hl_xlzd_low,
    s=1e68
)

smooth_xlzd_high = UnivariateSpline(
    years_xlzd_high,
    hl_xlzd_high,
    s=1e68
)

smooth_xlzd_low = smooth_xlzd_low(years_xlzd_smooth)
smooth_xlzd_high = smooth_xlzd_high(years_xlzd_smooth)

ax0.plot(
    years_xlzd_low,
    hl_xlzd_low,
    color="green",
    linewidth=2,
    label="XLZD nominal",
)

ax0.plot(
    years_xlzd_high,
    hl_xlzd_high,
    color="green",
    linewidth=2,
    linestyle="--",
    label="XLZD optimistic",
)

ax0.fill_between(
    years_xlzd_smooth,
    smooth_xlzd_low,
    smooth_xlzd_high,
    color="green",
    alpha=0.2,
    label="XLZD band",
)

ax0.text(
    9, 1.65e28,
    "HERETIX (5 t)",
    ha="center",
    va="center",
    fontsize=15,
    fontweight="bold",
    rotation=9.5,
    alpha=0.8,
)

ax0.text(
    9, 7e27,
    "XLZD (60 t)",
    ha="center",
    va="center",
    fontsize=20,
    fontweight="bold",
    color="green",
    rotation=8.5,
    alpha=0.9,
)

ax0.set_yscale("log")
ax0.set_yticks([1e27, 1e28])
ax0.yaxis.set_minor_locator(
    LogLocator(base=10, subs=np.arange(2, 10) * 0.1)
)

ax0.tick_params(which="major", length=5)
ax0.tick_params(which="minor", length=2.5)

ax0.set_xlabel("Exposure [years]")
ax0.set_ylabel(r"$T^{0\nu\beta\beta}_{1/2}$ exclusion limit (90% CL) [years]")
ax0.set_ylim(0.5e27, 3e28)
ax0.set_xlim(0.25, 12)

#ax0.legend(loc="lower right")


# ============================================================
# Panel 2: WIMP sensitivity
# ============================================================

df_xlzd = pd.read_csv("./data/XLZD_design_book.csv", 
                      names=["mass_200", "sigma_200", "mass_1000", "sigma_1000"])


label_map = {
    "nominal_max": "XLZD (20.3 years)",
    "sapphire_enriched_cut_max": "HERETIX (23.2 years)",
    "sapphire_enriched_cut_10p_max": "HERETIX 10% abs."
}

colors = {
    "nominal_max": "orange",
    "sapphire_enriched_cut_max": "black",
    "sapphire_enriched_cut_10p_max": "tab:red",
}

ul_labels = {
    "nominal_max": 4.6e-48,
    "sapphire_enriched_cut_max": 5.0e-49,
    "sapphire_enriched_cut_10p_max": 1.75e-47,
}

plt.plot(df_xlzd["mass_1000"].values, df_xlzd["sigma_1000"].values, 
         label="XLZD (1000 ty)", color="green", ls="-", lw=2)

lz = pd.read_csv("./data/Fig5_SpinIndependentLimitAndSensitivity.txt", sep='\t', comment="#", header=None, skiprows=1,
            names=["mass", "limit", "-2sigma", "-1sigma", "median", "1sigma", "2sigma", "median_3sigma_discovery"])

plt.fill_between(lz["mass"].values, 5e-46, lz["-1sigma"].values, color="gray", 
                 alpha=0.3, lw=0)

data = {"nominal_max": {},
        "sapphire_enriched_cut_max": {},
        "sapphire_enriched_cut_10p_max": {},
}
with h5py.File("data/sens_plot_data.hdf5", "r") as hdf:
    for key in data.keys():
        if key in hdf:
            masses, ul_median = hdf[key][:]
            data[key]["masses"] = masses
            data[key]["ul_median"] = ul_median

            masses_dense = np.logspace(
                np.log10(6),
                np.log10(masses.max()),
                1000
            )

            spl = UnivariateSpline(np.log10(masses), np.log10(ul_median), k=3, s=0.01)
            ul_dense = 10**spl(np.log10(masses_dense))

            data[key]["masses_dense"] = masses_dense
            data[key]["ul_dense"] = ul_dense

# fill between sapphire_enriched_cut_max and sapphire_enriched_cut_10p_max
#if "sapphire_enriched_cut_max" in data.keys() and "sapphire_enriched_cut_10p_max" in data.keys():
#    ax1.fill_between(
#        data["sapphire_enriched_cut_max"]["masses_dense"],
#        data["sapphire_enriched_cut_max"]["ul_dense"],
#        data["sapphire_enriched_cut_10p_max"]["ul_dense"],
#        color="red",
#        alpha=1,
#        zorder=0,
#        label="HERETIX band",
#    )

keys = ["sapphire_enriched_cut_max", "nominal_max"]

for key in keys:
    if key in data.keys():

        ax1.plot(
            data[key]["masses_dense"],
            data[key]["ul_dense"],
            color=colors[key],
            lw=3,
            ls="-" if not "nominal" in key else (0, (5, 5)),
        )

        # place label at x ~ 500 GeV
        if key == "sapphire_enriched_cut_max":
            idx = np.argmin(np.abs(data[key]["masses"] - 50))
        else:
            idx = np.argmin(np.abs(data[key]["masses"] - 600))

        ax1.text(
            data[key]["masses"][idx],
            ul_labels[key],
            label_map[key],
            color=colors[key],
            fontsize=13,
            fontweight="bold",
            rotation=38 if key != "sapphire_enriched_cut_max" else 35,
            ha="left",
            va="center",
        )

ax1.text(
    masses[idx],
    1.1e-48,
    "XLZD (60 t)",
    color="green",
    fontsize=13,
    fontweight="bold",
    rotation=37,
    ha="left",
    va="center",
)

ax1.text(
    masses[idx],
    7.3e-47,
    "Excluded",
    color="gray",
    fontsize=15,
    fontweight="bold",
    rotation=38,
    ha="left",
    va="center",
)

vmax = 11
vmin = 2

interval = np.linspace(0.175,0.8)
colors = cmr.ocean_r(interval)
cmap = LinearSegmentedColormap.from_list('name', colors)

scale = 2.5

ax1 = plt.gca()
ax1.text(1e3,5e-49/scale,'Atmospheric',color='k',alpha=0.85,fontsize=18,rotation=0)
#ax1.text(30,2e-50/scale,'DSNB',color='k',alpha=0.85,fontsize=18,rotation=0)
ax1.text(5,4e-48/scale,r'$hep$',color='k',alpha=0.85,fontsize=18,rotation=0)
ax1.text(3.5,4.5e-46/scale,r'$^8$B',color='k',alpha=0.99,fontsize=18,rotation=0)

ax1 = plt.gca()
col_min = cmap(0.0)
cnt = ax1.contourf(m,sig/scale,DY,levels=np.linspace(2,15,100),vmin=2.3,vmax=vmax,cmap=cmap,zorder=-100)
for c in cnt.collections: c.set_edgecolor("face")
#ax1.plot(m,NUFLOOR/scale,'-',color='gray',lw=1,zorder=1)
ax1.fill_between(m,NUFLOOR/scale,y2=1e-99,color=col_min,zorder=-1000)

ax1.set_xscale("log")
ax1.set_yscale("log")

ax1.set_xlim(3.5, 9e3)
ax1.set_ylim(8e-50, 5e-46)

ax1.set_xlabel("DM mass [GeV/c$^2$]")
ax1.set_ylabel(r"SI DM-nucleon cross section [cm$^2$]")

im = ax1.pcolormesh(-m,sig,DY,vmin=vmin,vmax=vmax,cmap=cmap,rasterized=True)
cb_ax = inset_axes(ax1, width="50%", height="5%", loc="upper right", borderpad=0.6)
cbar = fig.colorbar(im, cax=cb_ax, orientation="horizontal", extend="both", extendfrac=0.03)
cbar.ax.tick_params(labelsize=8)
cbar.set_label("Gradient of discovery limit, $n = -({\\rm d}\ln\sigma/{\\rm d}\ln N)^{-1}$", fontsize=9, rotation=0, labelpad=2)
cbar.ax.tick_params(which='major',direction='in',width=2,length=13,right=True,top=True)

# Optional panel labels
#ax0.text(0.03, 0.95, "(a)", transform=ax0.transAxes, fontsize=14, fontweight="bold", va="top")
#ax1.text(0.03, 0.95, "(b)", transform=ax1.transAxes, fontsize=14, fontweight="bold", va="top")

plt.savefig("combined_sensitivities.png", bbox_inches="tight", dpi=300)
plt.show()