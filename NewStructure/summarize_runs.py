import os
import re
import sys
import glob
import pandas as pd

# Use like this:
#   python3 summarize_runs.py root://dtn-eic.jlab.org/volatile/eic/EPIC/MC_input/LFHCal_BT/raw/CAEN_Sept24_TB_PS/
# This will output a file run_summary_full.csv in the configs directory.
# It will scrape all of the information from the Run#Info.txt files in the above directory.  In addition it will find the size of 
# every Run*list.txt file in said directory and the count of the triggers recorded in that file.  
# All of this gets summarized in one (huge) but handy table.

try:
    from XRootD import client
    from XRootD.client.flags import OpenFlags
except ImportError:
    client = None

def clean_key(key):
    if ' ' not in key:
        key = key.replace('[', '_').replace(']', '').replace('#', '')
        key = re.sub(r'[^\w_]', '', key)
    return key.strip()

def read_xrdfs_file_lines(xrd_url):
    if client is None:
        raise ImportError("XRootD client not installed. Run: pip install XRootD")

    fixed_url = re.sub(r'^(root://[^/]+)(/[^/])', r'\1/\2', xrd_url)
    if not re.match(r'root://[^/]+//', fixed_url):
        fixed_url = fixed_url.replace("root://", "root:///", 1)

    f = client.File()
    status, _ = f.open(fixed_url, OpenFlags.READ)
    if not status.ok:
        raise IOError(f"Failed to open {fixed_url}: {status.message}")

    lines = []
    offset = 0
    chunk_size = 65536
    while True:
        status, data = f.read(offset, chunk_size)
        if not status.ok or not data:
            break
        lines.append(data.decode('utf-8'))
        offset += len(data)

    f.close()
    return ''.join(lines).splitlines()

def parse_info_file(filepath):
    print(f"Parsing info file: {os.path.basename(filepath)}")
    lines = read_xrdfs_file_lines(filepath) if filepath.startswith("root://") else open(filepath).readlines()

    data = {}
    for line in lines:
        line = line.split('#', 1)[0].strip()
        if not line or line.startswith('*'):
            continue
        if ':' in line:
            key, val = line.split(':', 1)
            data[key.strip()] = val.strip()
        elif '=' in line:
            key, val = line.split('=', 1)
            data[clean_key(key)] = val.strip()
        elif re.match(r'\w+\[?\d*\]?\s+', line):
            parts = re.split(r'\s+', line, maxsplit=1)
            if len(parts) == 2:
                data[clean_key(parts[0])] = parts[1].strip()

    content = "\n".join(lines)
    if "Run" not in data:
        run_match = re.search(r'Run n\.\s*(\d+)', content)
        if run_match:
            data["Run"] = int(run_match.group(1))

    if "Run" in data:
        data["Run"] = int(re.search(r'\d+', str(data["Run"])).group())

    if "Start Time" not in data:
        match = re.search(r'Start Time:\s*(.+)', content)
        if match:
            data["Start Time"] = match.group(1).strip()
    if "Stop Time" not in data:
        match = re.search(r'Stop Time:\s*(.+)', content)
        if match:
            data["Stop Time"] = match.group(1).strip()
    if "Elapsed (s)" not in data:
        match = re.search(r'Elapsed time\s*=\s*([\d.]+)', content)
        if match:
            data["Elapsed (s)"] = float(match.group(1))

    return data

def count_triggers_in_listfile(filepath):
    print(f"  Fast counting triggers in file: {os.path.basename(filepath)}")
    last_trgid = None
    chunk_size = 65536

    if filepath.startswith("root://"):
        if client is None:
            raise ImportError("XRootD client not installed.")
        fixed_url = re.sub(r'^(root://[^/]+)(/[^/])', r'\1/\2', filepath)
        if not re.match(r'root://[^/]+//', fixed_url):
            fixed_url = fixed_url.replace("root://", "root:///", 1)
        f = client.File()
        status, _ = f.open(fixed_url, OpenFlags.READ)
        if not status.ok:
            raise IOError(f"Failed to open {fixed_url}: {status.message}")
        status, statinfo = f.stat()
        filesize = statinfo.size
        offset = max(0, filesize - chunk_size)
        status, data = f.read(offset, min(chunk_size, filesize))
        f.close()
        lines = data.decode("utf-8").splitlines()
    else:
        with open(filepath, 'rb') as f:
            f.seek(0, os.SEEK_END)
            filesize = f.tell()
            offset = max(0, filesize - chunk_size)
            f.seek(offset)
            lines = f.read().decode("utf-8").splitlines()

    lines.reverse()
    for line in lines:
        line = line.strip()
        if not line or line.startswith('//'):
            continue
        parts = re.split(r'\s+', line)
        if len(parts) >= 6:
            try:
                last_trgid = int(parts[5])
                break
            except ValueError:
                continue

    return (last_trgid + 1) if last_trgid is not None else 0

def list_remote_files(xrd_base):
    if client is None:
        raise ImportError("XRootD client not installed.")
    server = xrd_base.split('//')[1].split('/')[0]
    path = '/' + '/'.join(xrd_base.split('//')[1].split('/')[1:])
    fs = client.FileSystem(server)
    status, listing = fs.dirlist(path)
    if not status.ok:
        raise IOError(f"Failed to list {xrd_base}: {status.message}")
    return [f"{xrd_base.rstrip('/')}/{entry.name}" for entry in listing]

def summarize_runs(directory):
    summary = []
    all_files = list_remote_files(directory) if directory.startswith("root://") else glob.glob(os.path.join(directory, "*"))

    info_files = sorted(f for f in all_files if re.search(r'Run\d+_Info\.txt$', f))

    if not info_files:
        print("No Run*_Info.txt files found.")
        return pd.DataFrame()

    for info_path in info_files:
        try:
            info_data = parse_info_file(info_path)
            run_number = info_data['Run']
        except Exception as e:
            print(f"  Skipping {info_path}: {e}")
            continue

        base = f"Run{run_number}"
        part_files = []

        for f in all_files:
            #print(f"    Checking file: {os.path.basename(f)}")
            if re.match(f".*{base}\\.\\d+_list\\.(txt|csv)$", f) or re.match(f".*{base}_list\\.(txt|csv)$", f):
                print(f"    --> Matched: {os.path.basename(f)}")
                part_files.append(f)

        if not part_files:
            print(f"  Warning: no list files found for {base}")
            continue

        n_parts = len(part_files)
        n_triggers = sum(count_triggers_in_listfile(f) for f in part_files)

        total_size = 0

        for f in part_files:
            if f.startswith("root://"):
                server = f.split('//')[1].split('/')[0]
                path = '/' + '/'.join(f.split('//')[1].split('/')[1:])
                fs = client.FileSystem(server)
                status, statinfo = fs.stat(path)
                if status.ok:
                    total_size += statinfo.size
                else:
                    print(f"    Warning: Could not get size for {f}")
            else:
                try:
                    total_size += os.path.getsize(f)
                except:
                    print(f"    Warning: Could not get size for {f}")

        info_data["NumParts"] = n_parts
        info_data["NumTriggers"] = n_triggers
        info_data["TotalSize_MB"] = round(total_size / (1024 * 1024), 2)
        summary.append(info_data)

    return pd.DataFrame(summary)

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description="Summarize CAEN DT5202 run files")
    parser.add_argument("path", help="Local directory or XRootD path (e.g. ./data or root://server/path/)")
    args = parser.parse_args()

    summary_df = summarize_runs(args.path)
    outfile = "../configs/run_summary_full.csv"
    if "Run" in summary_df.columns:
        summary_df = summary_df.sort_values(by="Run")
    print("Writing file: ", outfile)
    summary_df.to_csv(outfile, index=False)
    print(summary_df.head())

