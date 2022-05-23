import torch
import numpy as np
import pickle
import glob
import os

class DataSetECAL(torch.utils.data.Dataset):
    def __init__(self, dataset_dir):
        super().__init__()
        # check if the file exists
        if not os.path.exists(dataset_dir):
            raise FileNotFoundError(f'{dataset_dir} not found.')
        if dataset_dir[-1] != '/':
            dataset_dir += '/'
        self._pickle_list = glob.glob(dataset_dir + '*.pickle')
        self._pickle_list.sort()
        self._len = len(self._pickle_list)
        
    def __getitem__(self, index):
        ret = pickle.load(open(self._pickle_list[index], 'rb'))
        return ret
        
    def __len__(self):
        return self._len
    
if __name__ == '__main__':
    dataset_dir = 'data/dataloader/test_01_0_pickle'
    dataset = DataSetECAL(dataset_dir)
    train_loader = torch.utils.data.DataLoader(
        dataset,
        batch_size=8,
        shuffle=True,
        num_workers=4,
    )
    print(train_loader.__iter__().__next__())