#!/usr/bin/env python3
import pandas as pd
import argparse
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
    args = parser.parse_args()

    print("Starting FUSE combination with the following parameters:")
    print(f"  Config file: {args.config_file}")
    print(f"  Output file: {args.output_file}")
    print(f"  Input folder: {args.input_folder}")

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

    # Open the output hdf5 file
    outstore = pd.HDFStore(args.output_file, complevel=9, complib='blosc', mode='w')

    # In groups of 10 runs, combine them into a single hdf5 file
    for run in tqdm.tqdm(runs):
        macro = run.split("_neutron_")[0]
        runid = run.split("_neutron_")[1]
        try:
            df = st.get_df(run, targets=('clustered_interactions'), progress_bar=False)

            df.insert(0, 'runid', runid)
            df.insert(0, 'macro', macro)

            # write new key or append if key already exists
            if macro in outstore:
                outstore.append(macro, df, format="table", data_columns=df.columns.tolist(), index=False)
            else:
                outstore.put(macro, df, format="table", data_columns=df.columns.tolist())
        except Exception as e:
            print(f"Error processing run {run}: {e}")
            continue

    for key in outstore.keys():
        outstore.create_table_index(key, columns=df.columns.tolist(), optlevel=9, kind='full')

    outstore.close()

if __name__ == "__main__":
    main()