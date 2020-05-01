from torch_geometric.data import Data, InMemoryDataset, DataLoader
from numpy import array
from numpy.fft import fft
from os.path import join
import torch, os, csv, re, numpy as np

class Brain(InMemoryDataset):
    def __init__(self, root, transform=None, pre_transform=None, corrThresh = 0.75):
        self.corrThresh = corrThresh
        super().__init__(root, transform, pre_transform)

    @property
    def raw_file_names(self):
        return [f for f in os.listdir(join('data', 'brain')) if f[:5] == 'train']

    @property
    def processed_file_names(self):
        return ['data.pt']

    def download(self):
        # Download to `self.raw`.
        pass

    def csv2data(self, f):
        pass

    def process(self):
        # Read data into huge `Data` list.
        rawFiles = [join('data', 'brain', f) for f in self.raw_file_names]
        data_list = [self.csv2data(f) for f in rawFiles]

        if self.pre_filter is not None:
            data_list = [data for data in data_list if self.pre_filter(data)]

        if self.pre_transform is not None:
            data_list = [self.pre_transform(data) for data in data_list]

        data, slices = self.collate(data_list)
        torch.save((data, slices), self.processed_paths[0])

class AllOne(Brain):
    def __init__(self, root, transform=None, pre_transform=None, corrThresh = 0.75):
        super().__init__(root, transform, pre_transform, corrThresh = 0.75)
        self.data, self.slices = torch.load(self.processed_paths[0])

    def csv2data(self, f):
        with open (f) as csvFile:
            num = int(re.findall(r'\d+', f)[0])
            if num <= 144:
                y = torch.tensor([0], dtype=torch.long)
            else:
                y = torch.tensor([1], dtype=torch.long)
            reader = csv.reader(csvFile, delimiter=',')
            D = array([[float(n) for n in r] for r in reader]).transpose()
            corrMat = np.corrcoef(D)
            x = torch.tensor([[1]]*94, dtype=torch.float)
            edge_index = torch.tensor([[i, j] for i in range(94) for j in range(94) if i != j and corrMat[i, j] > self.corrThresh]).t().contiguous()
            return(Data(edge_index=edge_index, x=x, y=y))
    
    def process(self):
        super().process()

class OneHot(Brain):
    def __init__(self, root, transform=None, pre_transform=None, corrThresh = 0.75):
        super().__init__(root, transform, pre_transform, corrThresh = 0.75)
        self.data, self.slices = torch.load(self.processed_paths[0])

    def csv2data(self, f):
        with open (f) as csvFile:
            num = int(re.findall(r'\d+', f)[0])
            if num <= 144:
                y = torch.tensor([0], dtype=torch.long)
            else:
                y = torch.tensor([1], dtype=torch.long)
            reader = csv.reader(csvFile, delimiter=',')
            D = array([[float(n) for n in r] for r in reader]).transpose()
            corrMat = np.corrcoef(D)
            x = torch.tensor(np.eye(94), dtype=torch.float)
            edge_index = torch.tensor([[i, j] for i in range(94) for j in range(94) if i != j and corrMat[i, j] > self.corrThresh]).t().contiguous()
            return(Data(edge_index=edge_index, x=x, y=y))

    def process(self):
        super().process()

class FFT(Brain):
    def __init__(self, root, transform=None, pre_transform=None, corrThresh = 0.75):
        super().__init__(root, transform, pre_transform, corrThresh = 0.75)
        self.data, self.slices = torch.load(self.processed_paths[0])

    def csv2data(self, f):
        with open (f) as csvFile:
            num = int(re.findall(r'\d+', f)[0])
            if num <= 144:
                y = torch.tensor([0], dtype=torch.long)
            else:
                y = torch.tensor([1], dtype=torch.long)
            reader = csv.reader(csvFile, delimiter=',')
            D = array([[float(n) for n in r] for r in reader]).transpose()
            Z = np.absolute(fft(D, axis=-1))[:, 6:48]
            Z = Z/np.expand_dims(Z.sum(axis=-1), axis=-1).repeat(42, axis=-1)
            corrMat = np.corrcoef(D)
            x = torch.tensor(Z, dtype=torch.float)
            edge_index = torch.tensor([[i, j] for i in range(94) for j in range(94) if i != j and corrMat[i, j] > self.corrThresh]).t().contiguous()
            return(Data(edge_index=edge_index, x=x, y=y))
    
    def process(self):
        super().process()
