import csv, os, torch, re, numpy as np, torch.nn.functional as F, pickle
from numpy import array
from os.path import join
from torch_geometric.data import Data, InMemoryDataset, DataLoader
from torch_geometric.nn import SAGEConv, GraphConv, TopKPooling, global_add_pool, global_max_pool, global_mean_pool, global_sort_pool
from brain import FFT, OneHot, AllOne

device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
class SAGE(torch.nn.Module):
    def __init__(self, inChannels, hiddenChannels, outChannels):
        super().__init__()
        self.conv1 = SAGEConv(inChannels, hiddenChannels, concat=True)
        self.conv2 = SAGEConv(hiddenChannels, hiddenChannels, concat=True)

        self.lin1 = torch.nn.Linear(hiddenChannels, 32)
        self.lin2 = torch.nn.Linear(32, 8)
        self.lin3 = torch.nn.Linear(8, outChannels)

        self.set_aggr('add')

    def set_aggr(self, aggr):
        self.conv1.aggr = aggr
        self.conv2.aggr = aggr

    def forward(self, data):
        x0, edgeIndex, batch = data.x, data.edge_index, data.batch

        x1 = F.relu(self.conv1(x0, edgeIndex))
        x2 = F.relu(self.conv2(x1, edgeIndex))

        xz = global_add_pool(x2, batch)
        x = F.relu(self.lin3(F.relu(self.lin2(F.relu(self.lin1(xz)))))).log_softmax(dim=-1)
        return x

class GRAPHOld(torch.nn.Module):
    def __init__(self, inChannels, hiddenChannels, outChannels):
        super().__init__()
        self.conv1 = GraphConv(inChannels, hiddenChannels)
        self.pool1 = TopKPooling(hiddenChannels, ratio=0.8)
        self.conv2 = GraphConv(hiddenChannels, hiddenChannels)
        self.pool2 = TopKPooling(hiddenChannels, ratio=0.8)


        self.lin1 = torch.nn.Linear(2*hiddenChannels, 32)
        self.lin2 = torch.nn.Linear(32, 8)
        self.lin3 = torch.nn.Linear(8, outChannels)

    def forward(self, data):
        x, edgeIndex, batch = data.x, data.edge_index, data.batch

        x = F.relu(self.conv1(x, edgeIndex))
        x, edgeIndex, _, batch, _, _ = self.pool1(x, edgeIndex, None, batch)
        x1 = torch.cat([global_max_pool(x, batch), global_mean_pool(x, batch)], dim=1)

        x = F.relu(self.conv2(x, edgeIndex))
        x, edgeIndex, _, batch, _, _ = self.pool2(x, edgeIndex, None, batch)
        x2 = torch.cat([global_max_pool(x, batch), global_mean_pool(x, batch)], dim=1)

        xz = x1 + x2
        x = F.relu(self.lin3(F.relu(self.lin2(F.relu(self.lin1(xz)))))).log_softmax(dim=-1)
        return x
    
class GRAPH(torch.nn.Module):
    def __init__(self, inChannels, hiddenChannels, outChannels):
        super().__init__()

        self.conv1 = GraphConv(inChannels, hiddenChannels)
        self.pool1 = TopKPooling(hiddenChannels, ratio=0.8)
        self.conv2 = GraphConv(hiddenChannels, hiddenChannels)
        self.pool2 = TopKPooling(hiddenChannels, ratio=0.8)
        self.conv3 = GraphConv(hiddenChannels, hiddenChannels)
        self.pool3 = TopKPooling(hiddenChannels, ratio=0.8)

        self.lin1 = torch.nn.Linear(hiddenChannels*2, hiddenChannels)
        self.lin2 = torch.nn.Linear(hiddenChannels, 64)
        self.lin3 = torch.nn.Linear(64, outChannels)

    def forward(self, data):
        x, edge_index, batch = data.x, data.edge_index, data.batch

        x = F.relu(self.conv1(x, edge_index))
        x, edge_index, _, batch, _, _ = self.pool1(x, edge_index, None, batch)
        x1 = torch.cat([global_max_pool(x, batch), global_mean_pool(x, batch)], dim=1)

        x = F.relu(self.conv2(x, edge_index))
        x, edge_index, _, batch, _, _ = self.pool2(x, edge_index, None, batch)
        x2 = torch.cat([global_max_pool(x, batch), global_mean_pool(x, batch)], dim=1)

        x = F.relu(self.conv3(x, edge_index))
        x, edge_index, _, batch, _, _ = self.pool3(x, edge_index, None, batch)
        x3 = torch.cat([global_max_pool(x, batch), global_mean_pool(x, batch)], dim=1)

        x = x1 + x2 + x3

        x = F.relu(self.lin1(x))
        x = F.dropout(x, p=0.5, training=self.training)
        x = F.relu(self.lin2(x))
        x = F.log_softmax(self.lin3(x), dim=-1)

        return x

class Result(object):
    def __init__(self, baseline=None, accuracy=None, loss=None):
        self.baseline = baseline
        self.accuracy = accuracy
        self.loss = loss
        super().__init__()

def train(model, optimizer, trainLoader):
    global device
    model.train()

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
        #print('predicted: {}'.format(predicted))
    #out = model(data.x.to(device), data.edge_index.to(device))
    #pred = out.argmax(dim=-1)
    #correct = pred.eq(data.y.to(device))
    accs = correctExamples/totalExamples

    return accs

if __name__ == "__main__":
    sage = 'sage'
    graph = 'graph'
    fft = 'fft'
    onehot = 'onehot'
    allone = 'allone'
    modelDict = {sage: SAGE, graph:GRAPH}
    datasetDict = {fft:FFT, onehot:OneHot, allone:AllOne}
    try:
        with open('result.pickle', 'rb') as f:
            result = pickle.load(f)
    except Exception:
        result = {}
    modelName = [sage, graph]
    datasetName = [fft, onehot, allone]
    learnRate = {sage:1e-5, graph:5e-4}
    totalEpoch = 500
    crossFoldNum = 10
    batchSize = 30
    for mn in modelName:
        for dn in datasetName:

            baseline = []
            accuracy = []
            loss = []

            if (mn, dn) in result:
                continue
            dataset = datasetDict[dn](root=join('data', 'brain', dn)).shuffle()
            foldSize = len(dataset) / crossFoldNum
            for foldNum in range(crossFoldNum):
                if int((foldNum + 1) * foldSize) > len(dataset):
                    testData = dataset[int(foldNum * foldSize): ]
                    trainData = dataset[ :int(foldNum * foldSize)]
                else:
                    testDataset = dataset[int(foldNum * foldSize):int((foldNum + 1) * foldSize)]
                    trainDataset = dataset[ :int(foldNum * foldSize)] + dataset[int((foldNum + 1) * foldSize): ]

                # compute baseline accuracy
                pTrainDataCnt = len([data for data in trainDataset if data.y > 0])
                nTrainDataCnt = len(trainDataset) - pTrainDataCnt
                pTestDataCnt = len([data for data in testDataset if data.y > 0])
                nTestDataCnt = len(testDataset) - pTestDataCnt
                if pTrainDataCnt > nTrainDataCnt:
                    baseline.append(pTestDataCnt/len(testDataset))
                else:
                    baseline.append(nTestDataCnt/len(testDataset))

                accuracyi = []
                lossi = []

                model = modelDict[mn](dataset.num_features, 128, dataset.num_classes).to(device)
                optimizer = torch.optim.Adam(model.parameters(), lr=learnRate[mn])
                for epoch in range(totalEpoch):
                    trainLoader = DataLoader(trainDataset, batch_size=batchSize, shuffle=True)
                    testLoader = DataLoader(testDataset, batch_size=batchSize, shuffle=True)

                    lossi.append(train(model, optimizer, trainLoader))
                    accuracyi.append(test(model, testLoader))

                    print('Model {} on dataset {} in fold {}, epoch: {:02d}, Loss: {}, Test: {:.4f}'.format(mn, dn, foldNum + 1, epoch + 1, lossi[-1], accuracyi[-1]))

                accuracy.append(accuracyi)
                loss.append(lossi)

            result[(mn, dn)] = Result(baseline=baseline, accuracy=accuracy, loss=loss)
            with open('result.pickle', 'wb') as f:
                pickle.dump(result, f)

    # #dataset = FFT(root='data/brain/fft').shuffle()
    # dataset = OneHot(root='data/brain/onehot').shuffle()
    # #dataset = AllOne(root='data/brain/allone').shuffle()
    # model = SAGE(dataset.num_features, 128, dataset.num_classes).to(device)
    # optimizer = torch.optim.Adam(model.parameters(), lr=5e-4)
    # n = (len(dataset)+7) // 8
    
    # testDataset = dataset[:n]
    # trainDataset = dataset[n:]
    # testLoader = DataLoader(testDataset, batch_size=batchSize)

    # loss100 = []
    # epoch = 0
    # while len(loss100) < 100 or len(set(loss100)) > 1:
    #     trainLoader = DataLoader(trainDataset, batch_size=batchSize, shuffle=True)
    #     testLoader = DataLoader(testDataset, batch_size=batchSize, shuffle=True)
    #     loss = train(model, optimizer, trainLoader)
    #     loss100.append(loss)
    #     if len(loss100) > 100:
    #         loss100 = loss100[1:]
    #     accs = test(model, testLoader)
    #     epoch += 1
    #     #print('Epoch: {:02d}, Loss: {:.4f}, Test: {:.4f}'.format(epoch, loss, accs))
    #     print('Epoch: {:02d}, Loss: {}, Test: {:.4f}'.format(epoch, loss, accs))



