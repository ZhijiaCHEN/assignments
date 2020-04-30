import csv, os, torch, re, numpy as np, torch.nn.functional as F
from numpy import array
from os.path import join
from torch_geometric.data import Data, InMemoryDataset, DataLoader
from torch_geometric.nn import SAGEConv, TopKPooling, global_add_pool, global_max_pool, global_mean_pool, global_sort_pool
from brain import Brain

device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
class SAGE(torch.nn.Module):
    def __init__(self, inChannels, hiddenChannels, outChannels):
        super().__init__()
        self.conv1 = SAGEConv(inChannels, hiddenChannels, concat=True)
        self.conv2 = SAGEConv(hiddenChannels, hiddenChannels, concat=True)
        self.conv3 = SAGEConv(hiddenChannels, hiddenChannels, concat=True)

        self.pool = TopKPooling(hiddenChannels, ratio=0.8)

        self.lin1 = torch.nn.Linear(hiddenChannels, 128)
        self.lin2 = torch.nn.Linear(128, 64)
        self.lin3 = torch.nn.Linear(64, outChannels)

       
    def set_aggr(self, aggr):
        self.conv1.aggr = aggr
        self.conv2.aggr = aggr
        self.conv3.aggr = aggr

    def forward(self, data):
        x0, edgeIndex, batch = data.x, data.edge_index, data.batch

        x1 = F.relu(self.conv1(x0, edgeIndex))
        x2 = F.relu(self.conv2(x1, edgeIndex))
        x3 = F.relu(self.conv3(x2, edgeIndex))

        #x, edgeIndex, _, batch, _, _ = self.pool(x3, edgeIndex, None, batch)
        x4 = global_add_pool(x3, batch)
        x = F.relu(self.lin3(F.relu(self.lin2(F.relu(self.lin1(x4)))))).log_softmax(dim=-1)
        return x


def train(model, optimizer, trainLoader):
    global device
    model.train()
    model.set_aggr('add')

    totalLoss = totalExamples = 0
    for data in trainLoader:
        data = data.to(device)
        optimizer.zero_grad()
        out = model(data)
        _, predicted = torch.max(out, 1)
        #loss = F.nll_loss(out, data.y)
        loss = F.cross_entropy(out, data.y).sum()
        loss.backward()
        optimizer.step()
        totalLoss += loss.item()# * data.num_graphs
        totalExamples += data.num_graphs
    return totalLoss / totalExamples

@torch.no_grad()
def test(model, testLoader):
    model.eval()
    totalExamples = correctExamples = 0
    for data in testLoader:
        data = data.to(device)
        out = model(data)
        _, predicted = torch.max(out, 1)
        totalExamples += data.y.size(0)
        correctExamples += (predicted == data.y).sum().item()
        print('predicted: {}'.format(predicted))
    #out = model(data.x.to(device), data.edge_index.to(device))
    #pred = out.argmax(dim=-1)
    #correct = pred.eq(data.y.to(device))
    accs = correctExamples/totalExamples

    return accs

if __name__ == "__main__":
    dataset = Brain(root='Brain').shuffle()
    model = SAGE(dataset.num_features, 128, dataset.num_classes).to(device)
    optimizer = torch.optim.Adam(model.parameters(), lr=5e-5)
    n = (len(dataset)+7) // 8
    batchSize = 20
    testDataset = dataset[:n]
    trainDataset = dataset[n:]
    testLoader = DataLoader(testDataset, batch_size=batchSize)

    loss100 = []
    epoch = 0
    while len(loss100) < 100 or len(set(loss100)) > 1:
        trainLoader = DataLoader(trainDataset, batch_size=batchSize, shuffle=True)
        testLoader = DataLoader(testDataset, batch_size=batchSize, shuffle=True)
        loss = train(model, optimizer, trainLoader)
        loss100.append(loss)
        if len(loss100) > 100:
            loss100 = loss100[1:]
        accs = test(model, testLoader)
        epoch += 1
        #print('Epoch: {:02d}, Loss: {:.4f}, Test: {:.4f}'.format(epoch, loss, accs))
        print('Epoch: {:02d}, Loss: {}, Test: {:.4f}'.format(epoch, loss, accs))



