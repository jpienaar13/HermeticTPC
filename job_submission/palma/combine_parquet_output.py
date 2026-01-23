#!/usr/bin/env python3
import dask.dataframe as dd
import pandas as pd
import numpy as np
import argparse
import h5py
import tqdm
import os

# This script will load all processed parquet files and combine them into a single hdf file.
# mcms python /cloud/wwu1/p_agweinheimer/AGW_LowRad/home/l_alth03/simulations/HermeticTPC/combine_parquet_output.py -o /scratch/tmp/l_alth03/20251205_HermeticTPC/combined_parquet_output.h5 -i /scratch/tmp/l_alth03/20251205_HermeticTPC/parquet

def main():
    parser = argparse.ArgumentParser(description="parquet processing script")
    parser.add_argument("-o", "--output_file", default="./parquet/combined_output.h5",
                        help="Output HDF5 file (default: ./parquet/combined_output.h5)")
    parser.add_argument("-i", "--input_folder", default="./parquet",
                        help="Input folder containing processed parquet files (default: ./parquet)")
    parser.add_argument("-m", "--macro", default=None,
                        help="If set, only process this macro e.g. 'run_Copper_Th232' (default: None)")
    args = parser.parse_args()

    print("Starting parquet combination with the following parameters:")
    print(f"  Output file: {args.output_file}")
    print(f"  Input folder: {args.input_folder}")
    print(f"  Macro filter: {args.macro}")

    # List all files in arg.input_folder that end with ".parquet"
    runs = [f for f in os.listdir(args.input_folder) 
                if os.path.isfile(os.path.join(args.input_folder, f)) and f.endswith(".parquet")]

    # If macro is set, filter runs to only include that macro
    if args.macro is not None:
        runs = [r for r in runs if r.startswith(args.macro + "_neutron_")]

    # Read all macros in runs
    keys = list(h5py.File(args.output_file, 'r').keys())
    macros = set([run.split("_neutron_")[0] for run in runs])
    existing_runids = {}

    print(f"Found macros in runs: {macros}")
    print(f"Existing keys in output file: {keys}")

    for macro in macros:
        # Read list of runids already in output file
        if macro in keys:
            ddf_existing = dd.read_hdf(args.output_file, key=macro)
            existing_runids[macro] = ddf_existing['runid'].unique().compute().tolist()
            del ddf_existing
            print(f"Found {len(existing_runids)} existing runids in macro {macro}: {existing_runids}")
        else:
            existing_runids[macro] = []
    
    # Open the output hdf5 file check if it exists then append otherwise create new
    if os.path.exists(args.output_file):
        outstore = pd.HDFStore(args.output_file, complevel=9, complib='blosc', mode='a')
        print("Opening existing output file for appending.")
    else:
        outstore = pd.HDFStore(args.output_file, complevel=9, complib='blosc', mode='w')
        print("Creating new output file.")

    for run in tqdm.tqdm(runs):
        macro = run.split("_neutron_")[0]
        runid = run.split("_neutron_")[1].replace(".parquet", "")

        # Test if table is already in output file
        if macro in outstore:
            # Test if runid is already in table
            if int(runid) in existing_runids[macro]:
                print(f"Skipping already processed run {run}")
                continue

        try:
            df = pd.read_parquet(os.path.join(args.input_folder, run))

            df.insert(0, 'runid', runid)
            df.insert(0, 'macro', macro)

            # Convert runid to integer
            df['runid'] = df['runid'].astype(int)

            # rename xp, yp, zp to x, y, z
            df.rename(columns={'xp': 'x', 'yp': 'y', 'zp': 'z'}, inplace=True)
            # rename event to eventid, cluster to cluster_id
            df.rename(columns={'event': 'eventid', 'cluster': 'cluster_id'}, inplace=True)
            # rename edep to ed
            df.rename(columns={'edep': 'ed'}, inplace=True)
            # rename nclusters to n_clusters
            df.rename(columns={'nclusters': 'n_clusters'}, inplace=True)

            # write new key or append if key already exists
            if macro in outstore:
                outstore.append(macro, df, format="table", data_columns=df.columns.tolist(), index=False)
            else:
                outstore.put(macro, df, format="table", data_columns=df.columns.tolist())
        except Exception as e:
            print(f"Error p rocessing file {run}: {e}")
            continue

    print("Creating indexes for faster access...")
    index_columns = ['eventid', 'x', 'y', 'z', 'ed', 'n_clusters']
    for key in outstore.keys():
        outstore.create_table_index(key, columns=index_columns, optlevel=9, kind='full')

    outstore.close()

if __name__ == "__main__":
    main()