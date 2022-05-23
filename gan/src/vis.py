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
    img = ax.scatter(x, y, z, c=e, s=marker_size*500, cmap='jet')
    #ax.colorbar(img)

def save_some_samples(xyz_s, fixed_data, savepath, figsize=(10, 10)):
    N_SHOW = 9
    batch_size = xyz_s.shape[0]
    if batch_size < N_SHOW - 1:
        raise ValueError('batch_size must be larger than n_show - 1')
    fig = plt.figure(figsize=figsize)
    for i in range(N_SHOW):
        ax = fig.add_subplot(3, 3, i+1, projection='3d')
        ax.set_axis_off()
        if i == N_SHOW - 1:
            draw_sample(fixed_data, ax)
        else: 
            draw_sample(xyz_s[i], ax)
    fig.savefig(savepath)