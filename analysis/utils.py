"""
Material properties, volumes, and activity concentrations for background estimation.
"""

import numpy as np

# General Geant4 related values
G4_TPC = {
    "radius": 1500, # mm
    "top": 1365, # mm
    "bottom": -1682, # mm
}
G4_TPC["height"] = G4_TPC["top"] - G4_TPC["bottom"]
G4_TPC["center_point"] = [0, 0, G4_TPC["top"]-(G4_TPC["top"] - G4_TPC["bottom"]) / 2]
G4_TPC["mass"] = G4_TPC["height"]/10 * np.pi * (G4_TPC["radius"]/10)**2 * 2.862 / 1000  # kg, using LXe density

# Detector component volumes and densities from the Geant4 simulation
# Run the simulation for any specific confinement and extract the values from
# the Geant4 log. Units: volume in cm3, density in g/cm3
# Updated: 16.01.2025, Lutz
G4_components = {
    # LXe properties from Geant4
    "LXe": {"density": 2.862,  # g/cm3
            "temperature": 177.05,  # K
            "pressure": 1.5  # bar
            },
    "PMT": {},  # PmtTpcTop* -> Top and Bottom PMTs
    "Cryostat": {"volume": 1.5662e+06, 
                 "density": 7.7, 
                 "confinement": "phys_oCryostat phys_iCryostat"},
    "Teflon": {"volume": 130301, 
               "density": 2.2, 
               "confinement": "phys_GXeTeflonTub phys_LXeTeflonTub"},
    "Copper": {"volume": 56804.8, 
               "density": 8.92, 
               "confinement": "phys_CopperFCTub"},
    "Sapphire": {"volume": 63877.6, 
                 "density": 3.98, 
                 "confinement": "phys_GXeSapphireTub phys_GXeSapphireCap phys_LXeSapphireTub phys_LXeSapphireCap"},
}

# scaling in kg or unit number
for k, val in G4_components.items():
    if (val is not None) and ("volume" in val) and ("density" in val):
        # Calculate masses from volume and density
        G4_components[k]["scaling"] = val["volume"] * val["density"] / 1000  # convert to kg
G4_components["PMT"]["scaling"] = 2 * 1184  # number of PMTs

## Neutron source activities in detector materials

# List of neutron source isotopes
neutron_sources = ["U235", "U238", "Th228", "Th232", "Ra226"]

# https://www.sciencedirect.com/science/article/pii/0168900295010955
# SCT and SC are two suppliers of sapphire crystals for CRESST
activity_CRESST = {
    "Sapphire_SCT": {"Unit": "ppb", "U": 0.04, "Th": 0.033, "K": 900,},
    "Sapphire_CS": {"Unit": "ppb", "U": 0.02, "Th": 0.0023, "K": 1800,},
}

# Average and convert ppb to mBq/kg
# 1 mBq/kg 238U = 81 ppt U
# 1 mBq/kg 232Th = 246 ppt Th
# 1 mBq/kg 40K = 32.3 ppb K 
activity_CRESST["Sapphire"] = {"Unit": "mBq/kg"}
for isotope in ["U", "Th", "K"]:
    activity_CRESST["Sapphire"][isotope] = 0.5 * (
        activity_CRESST["Sapphire_SCT"][isotope] + activity_CRESST["Sapphire_CS"][isotope]
    )
activity_CRESST["Sapphire"]["U238"] = activity_CRESST["Sapphire"].pop("U") / 81e-3
activity_CRESST["Sapphire"]["Th232"] = activity_CRESST["Sapphire"].pop("Th") / 246e-3
activity_CRESST["Sapphire"]["K40"] = activity_CRESST["Sapphire"].pop("K") / 32.3

# Material activity concentrations/levels from DARWIN
# https://arxiv.org/pdf/2003.13407
# Some components have upper limits only, check the paper
activity_DARWIN = {
    "Titanium": {"Unit": "mBq/kg", "U238": 1.6, "Ra226": 0.09, "Th232": 0.28, "Th228": 0.25, "Co60": 0.02, "TI44": 1.16,},
    "PTFE": {"Unit": "mBq/kg", "U238": 1.2, "Ra226": 0.07, "Th232": 0.07, "Th228": 0.06, "Co60": 0.027,},
    "Copper": {"Unit": "mBq/kg", "U238": 1.0, "Ra226": 0.035, "Th232": 0.033, "Th228": 0.026, "Co60": 0.019,},
    "PMT": {"Unit": "mBq/unit", "U238": 8.0, "Ra226": 0.6, "Th232": 0.7, "Th228": 0.6, "Co60": 0.84,},
    "Electronics": {"Unit": "mBq/unit", "U238": 1.10, "Ra226": 0.34, "Th232": 0.16, "Th228": 0.16, "Co60": 0.008,},
}

# Material activity concentrations/levels from XENONnT
# In addition to DARWIN materials to include Cirlex, Quartz, Kovar, Steel, Al2O3
activity_XENONnT = {
    "PMT_Stem": {"Material": "Al2O3", "Unit": "mBq/unit", "U235": 0.28, "U238": 1.37, "Th228": 0.5, "Th232": 0.17, "Ra226": 0.26,},
    "PMT_Base": {"Material": "cirlex", "Unit": "mBq/unit", "U235": 0.18, "U238": 0.47, "Th228": 0.45, "Th232": 0.15, "Ra226": 0.32,},
    "PMT_Window": {"Material": "quartz", "Unit": "mBq/unit", "U235": 0.06, "U238": 0.69, "Th228": 0.06, "Th232": 0.02, "Ra226": 0.07,},
    "PMT_Body": {"Material": "kovar", "Unit": "mBq/unit", "U235": 0.02, "U238": 0.08, "Th228": 0.81, "Th232": 0.04, "Ra226": 0.31,},
    "PMT_Shell": {"Material": "steel", "Unit": "mBq/unit", "U235": 0.03, "U238": 0.15, "Th228": 0.11, "Th232": 0.03, "Ra226": 0.07,},
    "TPC_Walls": {"Material": "PTFE", "Unit": "mBq/kg", "U235": 0.07, "U238": 0.13, "Th228": 0.06, "Th232": 0.05, "Ra226": 0.14,},
    "InnerCryostat_Shell": {"Material": "steel", "Unit": "mBq/kg", "U235": 2.6, "U238": 8.81, "Th228": 1.27, "Th232": 1.15, "Ra226": 3.10,},
    "OuterCryostat_Shell": {"Material": "steel", "Unit": "mBq/kg", "U235": 2.19, "U238": 16.34, "Th228": 0.83, "Th232": 1.77, "Ra226": 2.70,},
}

# Calculate Cryostat as average of Inner and Outer Cryostat
activity_XENONnT["Cryostat"] = {}
for isotope in neutron_sources:
    activity_XENONnT["Cryostat"][isotope] = 0.5 * (
        activity_XENONnT["InnerCryostat_Shell"][isotope] + activity_XENONnT["OuterCryostat_Shell"][isotope]
    )

# Combined activity data for detector components
activity = {
    "PMT": activity_DARWIN["PMT"],
    "Cryostat": activity_XENONnT["Cryostat"],
    "Teflon": activity_XENONnT["TPC_Walls"],
    "Copper": activity_DARWIN["Copper"],
    "Sapphire": activity_XENONnT["PMT_Stem"],
}

# Neutron yield, [neutrons/decay]
# from https://xe1t-wiki.lngs.infn.it/doku.php?id=xenon:xenonnt_sr0:neutron_background_updated_template
# from https://github.com/XENONnT/nton/blob/neutron_sim/nton/background/neutron_background/neutron_prediction.py
# Main source: https://arxiv.org/pdf/1512.07501
nyield = {
    "Al2O3": {"U238": 1.2e-6, "U235": 1.3e-5, "Ra226": 6.0e-6, "Th232": 9.2e-9, "Th228": 1.4e-5,},
    "cirlex": {"U238": 1.3e-6, "U235": 2.2e-6, "Ra226": 3.5e-6, "Th232": 4.1e-8, "Th228": 2.4e-6,},
    "quartz": {"U238": 1.2e-6, "U235": 1.9e-6, "Ra226": 8.8e-7, "Th232": 6.8e-9, "Th228": 1.9e-6,},
    "kovar": {"U238": 1.1e-6, "U235": 1.3e-7, "Ra226": 1.2e-7, "Th232": 3.0e-11, "Th228": 1.0e-6,},
    "steel": {"U238": 1.1e-6, "U235": 4.1e-7, "Ra226": 3.1e-7, "Th232": 1.8e-9, "Th228": 2.0e-6,},
    "PTFE": {"U238": 7.4e-6, "U235": 1.3e-4, "Ra226": 5.5e-5, "Th232": 7.3e-7, "Th228": 1.0e-4,},
    "copper": {"U238": 1.1e-6, "U235": 3.3e-8, "Ra226": 2.5e-8, "Th232": 3.0e-11, "Th228": 3.6e-7,},
}

def map_material_name(sim_material):
    """
    Map material names between simulation and the neutron yield dictionary.
    """
    mapping = {
        "PMT": "quartz",
        "Cryostat": "steel",
        "Teflon": "PTFE",
        "Copper": "copper",
        "Sapphire": "Al2O3",
    }
    return mapping.get(sim_material, None)

def get_neutron_yield(sim_material, isotope):
    """
    Get the neutron yield for a given simulation material and isotope.
    """
    mat_name = map_material_name(sim_material)
    if mat_name and mat_name in nyield and isotope in nyield[mat_name]:
        return nyield[mat_name][isotope]
    else:
        return None
