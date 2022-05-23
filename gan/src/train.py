import os
import pickle
from tqdm import tqdm

import torch
import torch.nn as nn
import matplotlib.pyplot as plt

from model import Generator, Discriminator
from vis import save_some_samples

class Conv3DGAN:
    def __init__(
        self,
        n_epochs,
        latent_dim,
        lr_G,
        lr_D,
        beta,
        D_threshold,
        fixed_z,
        fixed_data,
        record_dir,
        feature_map,
        use_gpu_core,
        optimizer,
        dropout_rate,
    ):
        self._device = torch.device(f"cuda:{use_gpu_core}" if torch.cuda.is_available() else "cpu")
        self._n_epochs = n_epochs
        self._latent_dim = latent_dim
        self._lr_G = lr_G
        self._lr_D = lr_D
        self._beta = beta
        self._D_threshold = D_threshold
        self._fixed_z = fixed_z.to(self._device)
        self._fixed_data = fixed_data.float().to(self._device)
        self._record_dir = record_dir
        self._feature_map = feature_map
        self._optimizer = optimizer
        self._dropout_rate = dropout_rate
        print('##########################')
        print(f"Device: {self._device}")
        print('##########################')
        # record dir
        if self._record_dir[-1] != "/":
            self._record_dir = self._record_dir + "/"
        os.makedirs(self._record_dir, exist_ok=True)
        # model
        self._G = Generator(self._feature_map, self._latent_dim).to(self._device)
        self._D = Discriminator(self._feature_map, dropout_rate=self._dropout_rate).to(self._device)
        # criterion, optimizer
        self._criterion = nn.BCELoss()
        if self._optimizer == "Adam":
            self._G_optim = torch.optim.Adam(self._G.parameters(), lr=self._lr_G, betas=(self._beta, self._beta))
            self._D_optim = torch.optim.Adam(self._D.parameters(), lr=self._lr_D, betas=(self._beta, self._beta))
        elif self._optimizer == "RMSprop":
            self._G_optim = torch.optim.RMSprop(self._G.parameters(), lr=self._lr_G)
            self._D_optim = torch.optim.RMSprop(self._D.parameters(), lr=self._lr_D)
        # for record
        self._loss_G = []
        self._loss_D = []
        self._genereted_data = []
    
    
    def train(self, data_loader):
        self._i_iter = 0
        for epoch in tqdm(range(self._n_epochs), desc="Epoch", leave=False):
            for (i, data) in enumerate(tqdm(data_loader, desc='Iteration', leave=False)):
                # toggle train mode
                self._G.train()
                self._D.train()
                # prepare labels
                batch_size = data.size(0)
                label_real = torch.full((batch_size, 1,), 1., device=self._device)
                label_fake = torch.full((batch_size, 1,), 0., device=self._device)
                ##############################
                # train D
                ##############################
                z_noise = torch.randn(batch_size, self._latent_dim, device=self._device)
                data_real = data.float().to(self._device).view(batch_size, 1, 25, 25, 25)
                data_fake = self._G(z_noise).view(batch_size, 1, 25, 25, 25)
                d_out_real = self._D(data_real)
                d_out_fake = self._D(data_fake)
                d_real_loss = self._criterion(d_out_real, label_real)
                d_fake_loss = self._criterion(d_out_fake, label_fake)
                d_loss = d_real_loss + d_fake_loss
                d_real_accuracy = torch.ge(d_out_real.squeeze(), 0.5).float()
                d_fake_accuracy = torch.le(d_out_fake.squeeze(), 0.5).float()
                d_total_accuracy = torch.mean(torch.cat((d_real_accuracy, d_fake_accuracy), 0))
                if d_total_accuracy.item() < self._D_threshold:
                    self._D.zero_grad()
                    d_loss.backward()
                    self._D_optim.step()
                ##############################
                # train G
                ##############################
                z_noise = torch.randn(batch_size, self._latent_dim, device=self._device)
                data_fake = self._G(z_noise).view(batch_size, 1, 25, 25, 25)
                d_out_fake = self._D(data_fake)
                g_loss = self._criterion(d_out_fake, label_real)
                self._D.zero_grad()
                self._G.zero_grad()
                g_loss.backward()
                self._G_optim.step()
                ## record
                self._loss_G.append(g_loss.item())
                self._loss_D.append(d_loss.item())
                self._record_loss()
                self._i_iter += 1
            self._record_data(epoch)

    def _record_loss(self):
        axis_array = list(range(len(self._loss_G)))
        plt.plot(axis_array, self._loss_G, label="Generator")
        plt.plot(axis_array, self._loss_D, label="Discriminator")
        plt.legend()
        plt.savefig(f'{self._record_dir}loss.png')
        plt.cla()
        plt.clf()
        plt.close()
    
    def _record_data(self, epoch):
        save_dir = self._record_dir + "generated/"
        if not os.path.exists(save_dir):
            os.makedirs(save_dir, exist_ok=True)
        self._G.eval()
        generated_data = self._G(self._fixed_z).squeeze()
        save_some_samples(generated_data, self._fixed_data.squeeze(), f'{save_dir}{epoch}epoch.png')
        plt.cla()
        plt.clf()
        plt.close()