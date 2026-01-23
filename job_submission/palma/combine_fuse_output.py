#!/usr/bin/env python3
import dask.dataframe as dd
import pandas as pd
import argparse
import h5py
import tqdm
import fuse
import os

# This script will load all processed fuse files and combine them into a single hdf file.
# mcms python /cloud/wwu1/p_agweinheimer/AGW_LowRad/home/l_alth03/simulations/HermeticTPC/combine_fuse_output.py --config_file /cloud/wwu1/p_agweinheimer/AGW_LowRad/home/l_alth03/simulations/software/20251205_HermeticTPC/analysis/fuse_files/XENONnT_public_config.json -o /scratch/tmp/l_alth03/20251205_HermeticTPC/combined_fuse_output.h5 -i /scratch/tmp/l_alth03/20251205_HermeticTPC/fuse_data

def main():
    parser = argparse.ArgumentParser(description="FUSE processing script")
    parser.add_argument("--config_file", default="./fuse_files/XENONnT_public_config.json",
                        help="FUSE simulation config file (default: ./fuse_files/XENONnT_public_config.json)")
    parser.add_argument("-o", "--output_file", default="./fuse_data/combined_output.h5",
                        help="Output HDF5 file (default: ./fuse_data/combined_output.h5)")
    parser.add_argument("-i", "--input_folder", default="./fuse_data",
                        help="Input folder containing processed FUSE files (default: ./fuse_data)")
    parser.add_argument("-m", "--macro", default=None,
                        help="If set, only process this macro e.g. 'run_Copper_Th232' (default: None)")
    args = parser.parse_args()

    print("Starting FUSE combination with the following parameters:")
    print(f"  Config file: {args.config_file}")
    print(f"  Output file: {args.output_file}")
    print(f"  Input folder: {args.input_folder}")
    print(f"  Macro filter: {args.macro}")

    st = fuse.context.public_config_context(
        output_folder=args.input_folder,
        simulation_config_file=args.config_file,
        clustering_method="lineage"
    )

    # List all folders in arg.input_folder that don't end with "_temp"
    runs = [d for d in os.listdir(args.input_folder) 
                if os.path.isdir(os.path.join(args.input_folder, d)) and not d.endswith("_temp")]

    # for runs like run_Copper_Th232_neutron_00154-clustered_interactions-rnnslis6ld
    # strip everything afte first "-"
    runs = [r.split("-")[0] for r in runs]

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
        runid = run.split("_neutron_")[1]

        # Test if table is already in output file
        if macro in outstore:
            # Test if runid is already in table
            if int(runid) in existing_runids[macro]:
                print(f"Skipping already processed run {run}")
                continue

        try:
            df = st.get_df(run, targets=('clustered_interactions'), progress_bar=False)

            # Add runid and macro columns
            df.insert(0, 'runid', runid)
            df.insert(0, 'macro', macro)

            # Convert runid to integer
            df['runid'] = df['runid'].astype(int)

            # Add single scatter column
            df['n_clusters'] = df.groupby(['runid', 'eventid'])['cluster_id'].transform('nunique')

            # Hamonize units: from cm to mm
            df[["x", "y", "z", "x_pri", "y_pri", "z_pri"]] *= 10

            #if isinstance(df, dd.DataFrame):
            #    grp = df.groupby(['runid', 'eventid'])['cluster_id'].nunique().rename('n_clusters').reset_index()
            #    df2 = df.merge(grp, on=['runid', 'eventid'], how='left')
            #    ss_events = df2[df2['n_clusters'] == 1]

            # write new key or append if key already exists
            if macro in outstore:
                outstore.append(macro, df, format="table", data_columns=df.columns.tolist(), index=False)
            else:
                outstore.put(macro, df, format="table", data_columns=df.columns.tolist())
        except Exception as e:
            print(f"Error processing run {run}: {e}")
            continue

    print("Creating indexes for faster access...")
    index_columns = ['runid', 'x', 'y', 'z', 'ed', 'n_clusters']
    for key in outstore.keys():
        outstore.create_table_index(key, columns=index_columns, optlevel=9, kind='full')

    outstore.close()

if __name__ == "__main__":
    main()