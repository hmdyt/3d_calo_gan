import uproot
import numpy as np
import pickle
import time
import os
import argparse
from tqdm import tqdm
from concurrent.futures import ProcessPoolExecutor

def make_one_event_pickle(eDep, pickle_path, n_split):
    eDep_index = eDep.reshape(n_split, n_split, n_split)
    with open(pickle_path, 'wb') as f:
        pickle.dump(eDep_index, f)


class DataMaker:
    def __init__(self, rootfile_path: str, output_dir: str, max_workers: int = 1):
        if output_dir[-1] != '/':
            output_dir += '/'
        self._max_workers = max_workers
        self._rootfile_path = rootfile_path
        self._output_dir = output_dir
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)
        self._open_tree()
    
    def _open_tree(self):
        if not os.path.exists(self._rootfile_path):
            raise FileNotFoundError(f'{self._rootfile_path} not found.')
        self._tree = uproot.concatenate(f'{self._rootfile_path}:tree', library='numpy')
        self._n_split = self._tree['n_split'][0]
        self._eDep_s = self._tree['eDep']
    
    def save_parallel(self):
        with ProcessPoolExecutor(max_workers=self._max_workers) as executor:
            n_iter = self._eDep_s.shape[0]
            tqdm(executor.map(
                make_one_event_pickle,
                self._eDep_s,
                [f'{self._output_dir}{time.time()}.pickle' for i in range(n_iter)],
                [self._n_split for i in range(n_iter)]
            ))

if __name__ == '__main__':
    arg_parser = argparse.ArgumentParser()
    argparser.add_argument('rootfile_path', type=str)
    argparser.add_argument('output_dir', type=str)
    argparser.add_argument('-p', '--max_workers', type=int, default=1)
    args = arg_parser.parse_args()
    
    data_maker = DataMaker(args.rootfile_path, args.output_dir, args.max_workers)
    data_maker.save_parallel()