#!/usr/bin/env python3
import pandas as pd
import argparse
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
    args = parser.parse_args()

    print("Starting parquet combination with the following parameters:")
    print(f"  Output file: {args.output_file}")
    print(f"  Input folder: {args.input_folder}")

    # List all files in arg.input_folder that end with ".parquet"
    runs = [f for f in os.listdir(args.input_folder) 
                if os.path.isfile(os.path.join(args.input_folder, f)) and f.endswith(".parquet")]

    # Open the output hdf5 file
    outstore = pd.HDFStore(args.output_file, complevel=9, complib='blosc', mode='w')

    # In groups of 10 runs, combine them into a single hdf5 file
    for run in tqdm.tqdm(runs):
        macro = run.split("_neutron_")[0]
        runid = run.split("_neutron_")[1].replace(".parquet", "")
        try:
            df = pd.read_parquet(os.path.join(args.input_folder, run))

            df.insert(0, 'runid', runid)
            df.insert(0, 'macro', macro)

            # write new key or append if key already exists
            if macro in outstore:
                outstore.append(macro, df, format="table", data_columns=df.columns.tolist(), index=False)
            else:
                outstore.put(macro, df, format="table", data_columns=df.columns.tolist())
        except Exception as e:
            print(f"Error processing file {run}: {e}")
            continue

    for key in outstore.keys():
        outstore.create_table_index(key, columns=df.columns.tolist(), optlevel=9, kind='full')

    outstore.close()

if __name__ == "__main__":
    main()