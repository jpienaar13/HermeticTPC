#!/usr/bin/env python3
import argparse
import fuse

def main():
    parser = argparse.ArgumentParser(description="FUSE processing script")
    parser.add_argument("--config_file", default="./fuse_files/XENONnT_public_config.json",
                        help="FUSE simulation config file (default: ./fuse_files/XENONnT_public_config.json)")
    parser.add_argument("-o", "--output_folder", default="./fuse_data",
                        help="Output folder (default: ./fuse_data)")
    parser.add_argument("-f", "--input_file_name", default="20251210_Sapphire_U238_100k_gamma.root",
                        help="Input ROOT file name (default: 20251210_Sapphire_U238_100k_gamma.root)")
    parser.add_argument("--entry_stop", type=int, default=None,
                        help="Optional entry_stop (default: None)")
    parser.add_argument("-r", "--run_number", default="00000",
                        help="Run number passed to st.make (default: 00000)")
    args = parser.parse_args()

    print("Starting FUSE processing with the following parameters:")
    print(f"  Config file: {args.config_file}")
    print(f"  Output folder: {args.output_folder}")
    print(f"  Input file name: {args.input_file_name}")
    print(f"  Entry stop: {args.entry_stop}")
    print(f"  Run number: {args.run_number}")

    st = fuse.context.public_config_context(
        output_folder=args.output_folder,
        simulation_config_file=args.config_file,
        clustering_method="lineage"
    )

    st.set_config(
        {
            "path": "../",
            "file_name": args.input_file_name,
            "entry_stop": args.entry_stop,
        }
    )

    st.make(args.run_number, "clustered_interactions")

if __name__ == "__main__":
    main()