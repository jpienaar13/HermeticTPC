#!/usr/bin/env python3

import uproot
import math
import numpy as np
from numpy import ndarray
import pandas as pd
from tqdm import tqdm
import argparse

def calculate_norm_distance(
        tup1 : tuple, 
        tup2 : tuple
        )   -> float:
    
    x1, y1, z1 = tup1
    x2, y2, z2 = tup2

    sqx = (x2 - x1)**2
    sqy = (y2 - y1)**2
    sqz = (z2 - z1)**2
    
    distance = math.sqrt(sqx + sqy + sqz)
    return distance


def calculate_z_distance(
        z1 : float, 
        z2 : float
        )   -> float:
    
    return abs(z2 - z1)


def generate_cluster_matrix(
    x_arr  : ndarray,
    y_arr  : ndarray,
    z_arr  : ndarray,
    e_arr  : ndarray,
    scale  : float
    )     -> ndarray:

    mask = e_arr > 0.0
    idxs = np.nonzero(mask)[0]

    cluster_matrix = []

    cluster_arr = [idxs[0]] 
    prev_coord = (x_arr[idxs[0]], y_arr[idxs[0]], z_arr[idxs[0]])
    for i in idxs[1:]:
        coord = (x_arr[i], y_arr[i], z_arr[i])
        distance = calculate_norm_distance(prev_coord, coord)
        prev_coord = coord
        if distance < scale:
            cluster_arr.append(i)
        else:
            cluster_matrix.append(cluster_arr)
            cluster_arr = [i]

    cluster_matrix.append(cluster_arr)
    
    return cluster_matrix

def read_root_file(
        fn,
        treename='events', 
        istart=0, 
        istop=1000
        ):

    with uproot.open(fn) as f:
        tree = f[f"{treename}"][f"{treename}"]
        branches = ['nsteps', 
                    'xp', 
                    'yp', 
                    'zp', 
                    'etot', 
                    'xp_pri', 
                    'yp_pri', 
                    'zp_pri', 
                    'ed',
                    'PreStepEnergy',
                    'type',
                    'time']
        
        data = tree.arrays(
            branches, 
            filter_name='nsteps', 
            library='np', 
            entry_start=istart, 
            entry_stop=istop
            )

    return data


def get_total_events(fn, treename='events'):
    with uproot.open(fn) as f:
        tree = f[f"{treename}"][f"{treename}"]
        total = len(tree.arrays('etot'))
    return total

def time_cluster(time_arr, scale=1e8):
    sz = len(time_arr)
    order = np.argsort(time_arr)
    times = time_arr[order]

    cluster_num = 0
    cluster_idxs_sorted = np.zeros(sz, dtype=int)

    prev_t = times[0]
    cluster_idxs_sorted[0] = 0

    for i in range(1, sz):
        t = times[i]
        delta = t - prev_t

        if delta > scale:
            cluster_num += 1
        cluster_idxs_sorted[i] = cluster_num
        prev_t = t

    # map cluster labels back to original order
    cluster_idxs = np.zeros(sz, dtype=int)
    cluster_idxs[order] = cluster_idxs_sorted

    return cluster_idxs

def split_by_septime(data, scale=1e8):

    branches = ['nsteps', 
                'xp', 
                'yp', 
                'zp', 
                'etot', 
                'xp_pri', 
                'yp_pri', 
                'zp_pri', 
                'ed',
                'PreStepEnergy',
                'type',
                'time']

    val_mtx = [[] for _ in range(12)]

    time_mtx = np.array(data['time'])

    for i, time in enumerate(time_mtx):
        cluster_idxs = time_cluster(time, scale=scale)
        t_idx_uniques = np.unique(cluster_idxs)

        for t_idx in t_idx_uniques:
            mask = (cluster_idxs == t_idx)
            for k, branch in enumerate(branches):
                if branch=='type':
                    old_val = np.array(data[branch][i])
                    new_val = old_val[mask]
                elif branch=='nsteps':
                    new_val = int(np.sum(mask))
                elif branch=='etot':
                    edep_arr = np.array(data['ed'][i])
                    edep_arr_cluster = edep_arr[mask]
                    new_val=np.sum(edep_arr_cluster)
                else:
                    old_val = data[branch][i]
                    if isinstance(old_val, np.ndarray):
                        new_val = old_val[mask]
                    elif isinstance(old_val, (np.ndarray, list)):
                        new_val = old_val[mask]
                    else:
                        new_val = old_val

                val_mtx[k].append(new_val)

    new_data = {
        branches[0]  : val_mtx[0],
        branches[1]  : val_mtx[1],
        branches[2]  : val_mtx[2],
        branches[3]  : val_mtx[3],
        branches[4]  : val_mtx[4],
        branches[5]  : val_mtx[5],
        branches[6]  : val_mtx[6],
        branches[7]  : val_mtx[7],
        branches[8]  : val_mtx[8],
        branches[9]  : val_mtx[9],
        branches[10] : val_mtx[10],
        branches[11] : val_mtx[11]
    }

    return new_data

def generate_reduced_df(data, scale=10):

    df = pd.DataFrame(data)
    df = df[df['etot'] > 0] # Select only events with energy depositions    
    length = len(df)
    
    Clusters_main = [] #vector
    Clusterval_main = [] # scalar
    EG_main = [] #scalar
    Edep_main = [] #vector
    xp_main = [] #vector
    yp_main = [] #vector
    zp_main = [] #vector
    xp_pr_main = [] #scalar
    yp_pr_main = [] #scalar
    zp_pr_main = [] #scalar
    
    for idx in range(length):
        try:
            event_df = df.iloc[idx]

            mask = event_df['type'] == 'gamma'
            energies = event_df['PreStepEnergy'][mask]
            e_gam = float(energies[0])

            xp_pr = float(event_df['xp_pri'])
            yp_pr = float(event_df['yp_pri'])
            zp_pr = float(event_df['zp_pri'])
            
            z_arr = np.array(event_df['zp'])
            x_arr = np.array(event_df['xp'])
            y_arr = np.array(event_df['yp'])
            ed_arr = np.array(event_df['ed'])

            cluster_matrix = generate_cluster_matrix(
                x_arr, 
                y_arr, 
                z_arr, 
                ed_arr, 
                scale
                )
    
            sz = len(cluster_matrix)
    
            Edep_vals  = []
            X_vals  = []
            Y_vals  = []
            Z_vals  = []
            
            for cluster in cluster_matrix:
                x = x_arr[cluster]
                y = y_arr[cluster]
                z = z_arr[cluster]
                ed = ed_arr[cluster]
    
                # I should add a better averaging algorithm
                # that weighs position by edep
                E = np.nansum(ed)
                X = np.nanmean(x)
                Y = np.nanmean(y)
                Z = np.nanmean(z)
    
                Edep_vals.append(E)
                X_vals.append(X)
                Y_vals.append(Y)
                Z_vals.append(Z)
        
    
            Clusters_main.append(sz)
            Clusterval_main.append(sz)
            EG_main.append(e_gam)
            Edep_main.append(np.array(Edep_vals))
            xp_main.append(np.array(X_vals))
            yp_main.append(np.array(Y_vals))
            zp_main.append(np.array(Z_vals))
            xp_pr_main.append(xp_pr)
            yp_pr_main.append(yp_pr)
            zp_pr_main.append(zp_pr)
        
        except:
            pass
    
    data_reduced = {
        "clusters" : np.array(Clusters_main),
        "nclusters": Clusterval_main,
        "edep"     : Edep_main,
        "xp"       : xp_main,
        "yp"       : yp_main,
        "zp"       : zp_main,
        "e_gam"    : EG_main,
        "xp_pri"   : xp_pr_main,
        "yp_pri"   : yp_pr_main,
        "zp_pri"   : zp_pr_main
    }
    
    df_reduced = pd.DataFrame(data_reduced)

    return df_reduced


row_length = ('clusters')
scalar_fields = ['nclusters', 'e_gam', 'xp_pri', 'yp_pri', 'zp_pri']
vector_fields = ['edep','xp', 'yp', 'zp']

def flatten_mc_tree(df, last_event=0, row_length=row_length, vector_fields=vector_fields, scalar_fields=scalar_fields):
    flattened_data = []
    for i, row in df.iterrows():
        # Check lengths just in case
        assert row[row_length] == len(row[vector_fields[0]])

        for j in range(row[row_length]):
            new_row = {}
            new_row['event'] = i + last_event
            new_row['cluster'] = j

            for key in scalar_fields:
                new_row[key] = row[key]

            for key in vector_fields:
                new_row[key] = row[key][j]

            flattened_data.append(new_row)

    df_flattend = pd.DataFrame(flattened_data)
    df_flattend.set_index(['event', 'cluster', ])  # MultiIndex

    return df_flattend


# Main Processing Function
def process_root_file(fn, treename='events', chunksize=1000, scale=10, fulfill = 1):
    
    total = int(get_total_events(fn, treename=treename) * fulfill)
    df_arr = []
    prev_df = None
    for i_chunk in tqdm(range(0, total, chunksize)):
        istart = i_chunk
        istop = min(i_chunk + chunksize, total)

        data = read_root_file(fn, treename=treename, istart=istart, istop=istop)
        data = split_by_septime(data, scale=1e8) #0.1s
        df_reduced = generate_reduced_df(data, scale=scale)

        if prev_df is not None:
            last_event = prev_df.loc[:, 'event'].max() + 1
        else:
            last_event = 0

        df_flat = flatten_mc_tree(df_reduced, last_event=last_event)
        prev_df = df_flat

        df_arr.append(df_flat)

    df = pd.concat(df_arr, ignore_index=True)

    return df


# CLI entry point
def main():
    parser = argparse.ArgumentParser(description="Process ROOT file with df_process_file.")
    parser.add_argument("infile_name", help="Path to the input ROOT file.")
    parser.add_argument("--chunksize", type=int, default=1000, help="Number of entries per chunk.")
    parser.add_argument("--istop", type=int, default=-1, help="Stop processing after this many entries.")
    parser.add_argument("--outfile", type=str, default="output.parquet", help="Output Parquet file name.")
    parser.add_argument("--scale", type=float, default=10.0, help="Clustering scale in units consistent with z position.")
    parser.add_argument("--fulfill", type=float, default=1.0, help="Fraction of total events to process.")


    args = parser.parse_args()

    result_df = process_root_file(
        args.infile_name,
        chunksize=args.chunksize,
        scale=args.scale,
        fulfill=args.fulfill
    )

    print(f"Saving DataFrame as parquet: {args.outfile}")
    result_df.to_parquet(args.outfile)

if __name__ == "__main__":
    main()
