from torch_geometric.data import Data, InMemoryDataset, DataLoader
from numpy import array
from numpy.fft import fft
from os.path import join
import torch, os, csv, re, numpy as np
CTH = 0.38
class Brain(InMemoryDataset):
    def __init__(self, root, transform=None, pre_transform=None):
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
    def __init__(self, root, transform=None, pre_transform=None, corrThresh = CTH):
        self.corrThresh = CTH
        super().__init__(root, transform, pre_transform)
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
            x = torch.tensor([[1]*94]*94, dtype=torch.float)
            edge_index = torch.tensor([[i, j] for i in range(94) for j in range(94) if i != j and corrMat[i, j] > self.corrThresh]).t().contiguous()
            return(Data(edge_index=edge_index, x=x, y=y))
    
    def process(self):
        super().process()

class OneHot(Brain):
    def __init__(self, root, transform=None, pre_transform=None, corrThresh = CTH):
        self.corrThresh = CTH
        super().__init__(root, transform, pre_transform)
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
    def __init__(self, root, transform=None, pre_transform=None, corrThresh = CTH):
        self.corrThresh = CTH
        super().__init__(root, transform, pre_transform)
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

if __name__ == "__main__":
    import matplotlib.pyplot as plt
    files = [f for f in os.listdir(join('data', 'brain')) if f[:5] == 'train']
    cth = np.arange(-1, 1.02, 0.02)
    pEdges = []
    nEdges = []
    f2D = {}
    sumZ = 0
    for f in files:
        with open (join('data', 'brain', f)) as csvFile:
            num = int(re.findall(r'\d+', f)[0])
            reader = csv.reader(csvFile, delimiter=',')
            D = array([[float(n) for n in r] for r in reader]).transpose()
            Z = np.absolute(fft(D, axis=-1))
            sumZ += Z.sum(axis=0)
            corrMat = np.corrcoef(D)
            f2D[num] = corrMat
    
    sumZ = sumZ/sumZ.max()
    plt.figure(figsize=(6, 4))
    markerline, stemline, baseline = plt.stem(sumZ[:120])
    plt.setp(markerline, markersize = 3)
    plt.xlabel('frequency')
    plt.ylabel('normalize height')
    plt.margins(0)
    plt.tight_layout()
    plt.savefig(join('figure', 'fft.pdf'))
    plt.close()

    for c in cth:
        nEcnt = 0
        pEcnt = 0
        ncnt = 0
        pcnt = 0
        for num in range(1, 247):
            corrMat = f2D[num]
            edge_index = [[i, j] for i in range(94) for j in range(94) if i != j and corrMat[i, j] > c]
            if num <= 144:
                nEcnt += len(edge_index)
            else:
                pEcnt += len(edge_index)
            print('cth: {}, f: {}'.format(c, num))
        pEdges.append(pEcnt/102)
        nEdges.append(nEcnt/144)
    pEdges = array(pEdges)
    nEdges = array(nEdges)
    plt.figure(figsize=(6, 4))
    plt.plot(cth, pEdges - nEdges)
    #plt.plot(cth, nEdges, label = 'class 1')
    plt.xlabel('threshold')
    plt.ylabel('difference')
    plt.margins(0)
    plt.tight_layout()
    print('max thresh: {}'.format(cth[np.argmax(pEdges - nEdges)]))
    #plt.legend()
    plt.savefig(join('figure', 'cth.pdf'))
    