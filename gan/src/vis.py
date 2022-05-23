import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import numpy as np

# xyz.shape = (25, 25, 25)
def draw_sample(xyz, ax):
    """
    input shape = (25, 25, 25)
    """
    xyz = xyz.detach().cpu().numpy()
    x = np.zeros(25 * 25 * 25)
    y = np.zeros(25 * 25 * 25)
    z = np.zeros(25 * 25 * 25)
    e = xyz.reshape(25 * 25 * 25)
    n_split = xyz.shape[0]
    counter = 0
    for ix in range(n_split):
        for iy in range(n_split):
            for iz in range(n_split):
                x[counter] = ix
                y[counter] = iy
                z[counter] = iz
                counter += 1
                
    marker_size = (e / np.linalg.norm(e)) ** 2
    print(f'e.sum() = {e.sum()}, marker_size.sum() = {marker_size.sum()}')
    img = ax.scatter(x, y, z, c=e, s=marker_size*500, cmap='jet')
    #ax.colorbar(img)

def draw_some_samples(xyz_s, n_show=4, savepath, figsize=(10, 10)):
    batch_size = xyz_s.shape[0]
    if batch_size < n_show:
        raise ValueError('batch_size must be larger than n_show')
    fig = plt.figure(figsize=figsize)
    for i in range(n_show):
        ax = fig.add_subplot(1, 4, i+1, projection='3d')
        draw_sample(xyz_s[i], ax)
    fig.savefig(savepath)