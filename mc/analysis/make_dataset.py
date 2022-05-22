import uproot
import numpy as np
import pickle

from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt

def Dec_to_N(num, base):
    if num >= base:
        yield from Dec_to_N(num // base, base)
    yield num % base

def get_index(copy_num, n_split):
    ret = []
    for i in Dec_to_N(copy_num, n_split):
        ret.append(i)
    for i in range(len(ret), 3):
        ret.insert(0, 0)
    x, y, z = ret
    return x, y, z

# for: eDep.shape (copy_num) -> eDep_index.shape (n_split, n_split, n_split)
def get_eDep_index_map(n_copy_num, n_split):
    ret = []
    for copy_num in range(n_copy_num):
        x, y, z = get_index(copy_num, n_split)
        ret.append([x, y, z])
    return np.array(ret, dtype=int)

def draw_one_event(eDep):
    n_split = round(eDep.shape[0] ** (1/3))
    xyz = get_eDep_index_map(eDep.shape[0], n_split).T /25 * 10 -5
    x, y, z = xyz[0], xyz[1], xyz[2]
    marker_size = (eDep / np.linalg.norm(eDep)) ** 2
    cutted_index = np.where(eDep > 1)
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    img = ax.scatter(
        x[cutted_index], 
        y[cutted_index], 
        z[cutted_index], 
        s=1000 * marker_size[cutted_index],
        c=eDep[cutted_index], 
        cmap='jet')
    fig.colorbar(img)
    plt.show()
    

if __name__ == '__main__':
    tree = uproot.concatenate('/Users/yuto/VS/3d_calo_gan/mc/bench/test_01_0.root:tree', library='numpy')
    n_split = tree['n_split'][0]
    eDep_s = tree['eDep']
    draw_one_event(eDep_s[10])
    