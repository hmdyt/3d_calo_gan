import argparse
import time
import torch
from DataSetECAL import DataSetECAL
from train import Conv3DGAN

def main():
    argparser = argparse.ArgumentParser()
    argparser.add_argument('-e', '--epochs', type=int, default=5)
    argparser.add_argument('-l', '--latent_dim', type=int, default=200)
    argparser.add_argument('-b', '--batch_size', type=int, default=4)
    argparser.add_argument('-n', '--num_workers', type=int, default=4)
    argparser.add_argument('-lrG', '--learning_rate_generator', type=float, default=0.0025)
    argparser.add_argument('-lrD', '--learning_rate_discriminator', type=float, default=1e-5)
    argparser.add_argument('--beta', type=float, default=0.5)
    argparser.add_argument('-Dth', '--D_threshold', type=float, default=0.8)
    argparser.add_argument('-r', '--record_dir', type=str, default=f'record/{time.time()}')
    argparser.add_argument('-d', '--data_dir', type=str)
    argparser.add_argument('-f', '--feature_map', type=int, default=512)
    argparser.add_argument('-g', '--use_gpu_core', type=int, default=0)
    argparser.add_argument('-o', '--optimizer', type=str, default='Adam', choices=['Adam', 'RMSprop'])
    argparser.add_argument('--subset', type=float, default=1)
    argparser.add_argument('--dropout_rate', type=float, default=0.0)
    argparser.add_argument('--generator_train_rate', type=int, default=1)
    args = argparser.parse_args()
    
    dataset = DataSetECAL(args.data_dir)
    if args.subset != 1:
        dataset = torch.utils.data.Subset(dataset, range(int(len(dataset) * args.subset)))
        
    train_loader = torch.utils.data.DataLoader(
        # dataset_partial,
        dataset,
        batch_size=args.batch_size,
        shuffle=True,
        num_workers=args.num_workers,
    )
    fixed_z = torch.randn(args.batch_size, args.latent_dim)
    fixed_data = next(iter(train_loader))[0]
    gan = Conv3DGAN(
        n_epochs=args.epochs,
        latent_dim=args.latent_dim,
        lr_G=args.learning_rate_generator,
        lr_D = args.learning_rate_discriminator,
        beta=args.beta,
        D_threshold=args.D_threshold,
        record_dir=args.record_dir,
        feature_map=args.feature_map,
        fixed_z=fixed_z,
        fixed_data=fixed_data,
        use_gpu_core=args.use_gpu_core,
        optimizer=args.optimizer,
        dropout_rate=args.dropout_rate,
        generator_train_rate=args.generator_train_rate,
    )
    
    gan.train(train_loader)
    
if __name__ == "__main__":
    main()