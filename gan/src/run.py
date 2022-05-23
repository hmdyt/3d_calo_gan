import json
import subprocess
import argparse

argparser = argparse.ArgumentParser()
argparser.add_argument('exec_pythonfile', type=str)
argparser.add_argument('jsonfile', type=str)
arg = argparser.parse_args()

jsonfile = json.load(open(arg.jsonfile))
for a in jsonfile['args']:
    cmd = f'python3 {arg.exec_pythonfile}'
    for k, v in a.items():
        cmd += f' --{k} {v}'
    print(cmd)
    subprocess.run(cmd, shell=True)