import argparse
import subprocess
import time
import json
from tqdm import tqdm
from concurrent.futures import ProcessPoolExecutor

def exec_geant4(particle_name, energy, n_events, output_file):
    macro_file_str = open('run.mac.template', 'r').read().format(name=particle_name, energy=energy, num=n_events)
    macro_file_name = f'{output_file}_{time.time()}.mac'
    with open(macro_file_name, 'w') as macro_file:
        macro_file.write(macro_file_str)
    cmd = f'../build/Application_Main {macro_file_name} {output_file} &> {output_file}.log'
    subprocess.run(cmd, shell=True)

if __name__ == '__main__':
    arg_parser = argparse.ArgumentParser(description='Run the program')
    arg_parser.add_argument('json_file', type=str, help='JSON file with particle names and energies')
    arg_parser.add_argument('-p', '--max_processes', type=int, default=2, help='Maximum number of processes to run')
    arg_parser.add_argument('--executive', type=str, help='geant4 executable', default='../build/Application_Main')

    args = arg_parser.parse_args()

    parsed_json = json.load(open(args.json_file))
    particle_names = []
    energies = []
    nums = []
    outfiles = []
    for j in parsed_json['run']:
        particle_names.append(j['particle'])
        energies.append(j['energy'])
        nums.append(j['num'])
        outfiles.append(j['outfile'])

    with ProcessPoolExecutor(max_workers=args.max_processes) as executor:
        tqdm(executor.map(
            exec_geant4,
            particle_names,
            energies,
            nums,
            outfiles
        ))