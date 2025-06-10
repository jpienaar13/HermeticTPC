#!/usr/bin/env python3

import uproot
import numpy as np
import pandas as pd
from tqdm import tqdm
import argparse
import os

def read_root_dir_minimal(fn, treename='events', output_type='df', istart=0, istop=1000):
    '''
    Output types can be 'df' for pandas dataframe or 'array' for numpy array.
    '''
    with uproot.open(fn) as f:
        tree = f[f"{treename}"][f"{treename}"]
        branches = ['nsteps', 'xp', 'yp', 'zp', 'etot', 'xp_pri', 'yp_pri', 'zp_pri', 'ed', 'time']
        data = tree.arrays(branches, filter_name='nsteps', library='np', entry_start=istart, entry_stop=istop)

        # Apply selection: NAct > 0
        mask = data['nsteps'] >= 0
        filtered_data = {key: value[mask] for key, value in data.items()}

        if output_type == 'df':
            return pd.DataFrame(filtered_data)
        elif output_type == 'array':
            return filtered_data
        else:
            print(f'Warning: output type you specified is {output_type}, not understood, returning numpy array...')
            return filtered_data


row_length = ('nsteps')
scalar_fields = ['etot', 'xp_pri', 'yp_pri', 'zp_pri']
vector_fields = ['xp', 'yp', 'zp', 'ed', ]

def flatten_mc_tree(df, row_length=row_length, vector_fields=vector_fields, scalar_fields=scalar_fields):
    flattened_data = []
    for i, row in df.iterrows():
        # Check lengths just in case
        assert row[row_length] == len(row[vector_fields[0]])

        for j in range(row[row_length]):
            new_row = {}
            new_row['event'] = i
            new_row['particle'] = j

            for key in scalar_fields:
                new_row[key] = row[key]

            for key in vector_fields:
                new_row[key] = row[key][j]

            flattened_data.append(new_row)

    df_flattend = pd.DataFrame(flattened_data)
    df_flattend.set_index(['event', 'particle', ])  # MultiIndex

    return df_flattend


def process_chunk(df):
    xpos = []
    ypos = []
    zpos = []
    multis = []
    escatter = []

    events = np.unique(df['event'])
    for event in events[:]:
        mask = ((df['event'] == event) & (df['ed'] > 0))
        df_event = df[mask]
        xpos.append(np.average(df_event['xp'], weights=df_event['ed']))
        ypos.append(np.average(df_event['yp'], weights=df_event['ed']))
        zpos.append(np.average(df_event['zp'], weights=df_event['ed']))
        escatter.append(np.sum(df_event['ed']))
        multis.append(np.max(df_event['zp']) - np.min(df_event['zp']) > 10)

    df_scatters = pd.DataFrame({'xpos': xpos,
                                'ypos': ypos,
                                'zpos': zpos,
                                'escatter': escatter,
                                'multis': multis})
    return df_scatters


# Main processing function
def df_process_file(infile_name, treename='events', chunksize=1000, bstop=False, istop=-1):
    infile = uproot.open(infile_name)
    tree = infile[f"{treename}"][f"{treename}"]
    primaries = tree.arrays('etot')
    primaries = len(primaries['etot'])
    df_scatters = []
    print("Number of chunks:", np.ceil(primaries / chunksize))

    for i_chunk in tqdm(range(int(np.ceil(primaries / chunksize)))):
        if bstop and (chunksize * i_chunk) > istop:
            break
        df_raw = read_root_dir_minimal(infile_name, treename, istart=chunksize * i_chunk,
                                       istop=chunksize * (i_chunk + 1))
        df_flat = flatten_mc_tree(df_raw)
        df_flat = df_flat[df_flat['etot'] > 0]
        df_scatters.append(process_chunk(df_flat))

    df_scatters = pd.concat(df_scatters, ignore_index=True)
    return df_scatters


# CLI entry point
def main():
    parser = argparse.ArgumentParser(description="Process ROOT file with df_process_file.")
    parser.add_argument("infile_name", help="Path to the input ROOT file.")
    parser.add_argument("--chunksize", type=int, default=1000, help="Number of entries per chunk.")
    parser.add_argument("--bstop", action="store_true", help="Enable stop condition.")
    parser.add_argument("--istop", type=int, default=-1, help="Stop processing after this many entries.")
    parser.add_argument("--outfile", type=str, default="output.h5", help="Output HDF5 file name.")
    parser.add_argument("--key", type=str, default="df", help="Key name for the HDF5 store.")

    args = parser.parse_args()

    result_df = df_process_file(
        args.infile_name,
        chunksize=args.chunksize,
        bstop=args.bstop,
        istop=args.istop
    )

    print(f"Saving DataFrame to HDF5: {args.outfile} [key={args.key}]")
    #result_df.to_hdf(args.outfile, key=args.key, mode='w')

    result_npy = result_df.to_numpy()
    np.save(args.outfile, result_npy, allow_pickle=True)

if __name__ == "__main__":
    main()
