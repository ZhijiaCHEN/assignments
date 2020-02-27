import torch, math
from torch.autograd import Variable
from torch import optim
import torchvision
import torchvision.transforms as transforms
import torchvision.datasets as datasets

def task0_method1():
    X = Variable(torch.cat((torch.randn(2000, 2), torch.randn(1500, 2) + 13, torch.mm(torch.randn(1000, 2) + 6,3 * (torch.rand(2, 2) - 0.2))), 0).cuda(), requires_grad=False)
    K = 3
    Xmax = Variable(torch.max(X), requires_grad=False)
    Xmin = Variable(torch.min(X), requires_grad=False)
    X = (X-Xmin)/(Xmax-Xmin)
    with open('toy-input.txt', 'w') as f:
        f.write('# x y\n')
        for x in X:
            f.write('{} {} \n'.format(x[0], x[1]))
    Alpha = [1, 50]
    T = [100, 100]
    U = Variable((torch.rand(K, 2)).cuda(), requires_grad=True)
    learningRate = 0.1
    device = torch.device("cuda")
    #optimizer = optim.SGD([U], lr=learningRate)
    with open('toy-output.txt', 'w') as f:
        f.write('# u1x u1y u2x u2y u3x u3y loss\n')
        for alpha, t in zip(Alpha, T):
            print('Alpha: {}'.format(alpha))
            for i in range(t):
                loss = torch.tensor(0.0, requires_grad=True).to(device)
                for x in X:
                    D = (x.reshape(2, 1).mm(torch.ones(1, K).to(device)).t()-U).pow(2).sum(1)/2
                    expD = (-1*D*alpha).exp()+1e-32
                    W = expD/expD.sum()
                    loss = loss + (D*W).sum()
                    #loss = loss + (D*expD/expD.sum()).sum()
                loss = loss/X.size()[0]
                
                if U.grad is not None: U.grad.data.zero_()
                loss.backward()
                U.data -= learningRate*U.grad.data

                #loss.backward()
                #optimizer.step()
                print('Round {}: loss = {}; U = '.format(i+1, loss.data))
                print(U)
                data = U.data
                f.write('{} {} {} {} {} {} {}\n'.format(data[0][0], data[0][1], data[1][0], data[1][1], data[2][0], data[2][1], loss.data))

def task1_method1():
    device = torch.device("cuda")
    train_loader = torch.utils.data.DataLoader(torchvision.datasets.MNIST('./minist/', train=True, download=True, transform=torchvision.transforms.Compose([torchvision.transforms.ToTensor(),])), batch_size=10000, shuffle=False)
    examples = enumerate(train_loader)
    batch_idx, (example_data, example_targets) = next(examples)
    X = example_data.view(-1,784).to(device)
    X.requires_grad_(False)
    Y = example_targets.to(device)
    Y.requires_grad_(False)
    #X = torch.tensor(example_data.view(-1,784), requires_grad=False).to(device)
    #Y = torch.tensor(example_targets, requires_grad=False).to(device)

    N = X.size()[0]
    DIn = X.size()[1] 
    K = 10
    Xmax = Variable(torch.max(X), requires_grad=False)
    Xmin = Variable(torch.min(X), requires_grad=False)
    X = (X-Xmin)/(Xmax-Xmin)
    Alpha = [1, 50]
    T = [100, 100]
    U = Variable((torch.rand(K, DIn)).cuda(), requires_grad=True)
    learningRate = 0.1

    #optimizer = optim.SGD([U], lr=learningRate)
    with open('minist-output.txt', 'w') as f:
        f.write('# loss accuracy\n')
        accuracy = []
        for alpha, t in zip(Alpha, T):
            print('Alpha: {}'.format(alpha))
            for i in range(t):
                loss = torch.tensor(0.0, requires_grad=True).to(device)
                predict = []
                for x in X:
                    D = (x.reshape(DIn, 1).mm(torch.ones(1, K).to(device)).t()-U).pow(2).sum(1)/DIn
                    expD = (-1*D*alpha).exp()+1e-32
                    W = expD/expD.sum()
                    predict.append(max(range(len(W)), key=W.__getitem__))
                    loss = loss + (D*W).sum()
                loss = loss/N
                #calculate accuracy
                accuracy.append(0)
                clusters = [[] for i in range(K)]
                for yp, y in zip(predict, Y):
                    clusters[yp].append(y)
                for cluster in clusters:
                    labelCnt = [0]*K
                    for c in cluster:
                        labelCnt[c] += 1
                    accuracy[-1] += max(labelCnt)
                accuracy[-1]/=len(X)

                if U.grad is not None: U.grad.data.zero_()
                loss.backward()
                U.data -= learningRate*U.grad.data

                #loss.backward()
                #optimizer.step()
                print('Round {}: loss = {}; accuracy= {}; U = '.format(i+1, loss.data, accuracy[-1]))
                print(U)
                data = U.grad.data
                f.write('{} {}\n'.format(loss.data, accuracy[-1]))

def task1_method2():
    device = torch.device("cuda")
    train_loader = torch.utils.data.DataLoader(torchvision.datasets.MNIST('./minist/', train=True, download=True, transform=torchvision.transforms.Compose([torchvision.transforms.ToTensor(),])), batch_size=10000, shuffle=False)
    examples = enumerate(train_loader)
    batch_idx, (example_data, example_targets) = next(examples)
    X = example_data.view(-1,784).to(device)
    X.requires_grad_(False)
    Y = example_targets.to(device)
    Y.requires_grad_(False)
    #X = torch.tensor(example_data.view(-1,784), requires_grad=False).to(device)
    #Y = torch.tensor(example_targets, requires_grad=False).to(device)

    N = X.size()[0]
    DIn = X.size()[1] 
    K = 10
    Xmax = Variable(torch.max(X), requires_grad=False)
    Xmin = Variable(torch.min(X), requires_grad=False)
    X = (X-Xmin)/(Xmax-Xmin)
    Alpha = [50, 100]
    T = [100, 100]
    U = Variable((torch.rand(K, DIn)).cuda(), requires_grad=True)
    W = Variable((torch.rand(N, K)).cuda(), requires_grad=True)
    learningRate = 0.1

    #optimizer = optim.SGD([U], lr=learningRate)
    with open('minist-output.txt', 'w') as f:
        f.write('# loss accuracy\n')
        accuracy = []
        for alpha, t in zip(Alpha, T):
            print('Alpha: {}'.format(alpha))
            for i in range(t):
                loss = torch.tensor(0.0, requires_grad=True).to(device)
                predict = []
                for x, w in zip(X, W.clamp(min=0)):
                    D = (x.reshape(DIn, 1).mm(torch.ones(1, K).to(device)).t()-U).pow(2).sum(1)
                    predict.append(max(range(len(w)), key=w.__getitem__))
                    expW = (w*alpha).exp()/(w*alpha).exp().sum()
                    loss = loss + (D*w).sum() # w may be negative in the process of updating, what should we do?
                loss = loss/N
                #calculate accuracy
                accuracy.append(0)
                clusters = [[] for i in range(K)]
                for yp, y in zip(predict, Y):
                    clusters[yp].append(y)
                for cluster in clusters:
                    labelCnt = [0]*K
                    for c in cluster:
                        labelCnt[c] += 1
                    accuracy[-1] += max(labelCnt)
                accuracy[-1]/=len(X)

                if U.grad is not None: U.grad.data.zero_()
                if W.grad is not None: W.grad.data.zero_()
                loss.backward()
                U.data -= learningRate*U.grad.data
                W.data -= learningRate*W.grad.data

                #loss.backward()
                #optimizer.step()
                print('Round {}: loss = {}; accuracy= {}; U = '.format(i+1, loss.data, accuracy[-1]))
                print(U)
                data = U.grad.data
                f.write('{} {}\n'.format(loss.data, accuracy[-1]))

def task2_method1():
    device = torch.device("cuda")
    train_loader = torch.utils.data.DataLoader(torchvision.datasets.MNIST('./minist/', train=True, download=True, transform=torchvision.transforms.Compose([torchvision.transforms.ToTensor(),])), batch_size=100, shuffle=False)
    examples = enumerate(train_loader)
    batch_idx, (example_data, example_targets) = next(examples)
    X = example_data.view(-1,784).to(device)
    X.requires_grad_(False)
    Y = example_targets.to(device)
    Y.requires_grad_(False)

    N = X.size()[0]
    DIn = X.size()[1]
    DH1 = 500
    DH2 = 250
    DH3 = 2
    K = 10
    Xmax = Variable(torch.max(X), requires_grad=False)
    Xmin = Variable(torch.min(X), requires_grad=False)
    X = (X-Xmin)/(Xmax-Xmin)
    Alpha = [1, 50]
    T = [100, 100]
    U = None
    W1 = Variable((torch.rand(DIn, DH1)).cuda(), requires_grad=True)
    W2 = Variable((torch.rand(DH1, DH2)).cuda(), requires_grad=True)
    W3 = Variable((torch.rand(DH2, DH3)).cuda(), requires_grad=True)
    lossLambda = 0.5
    learningRate = 0.1

    #optimizer = optim.SGD([U], lr=learningRate)
    with open('minist-output.txt', 'w') as f:
        f.write('# loss accuracy\n')
        accuracy = []
        for alpha, t in zip(Alpha, T):
            print('Alpha: {}'.format(alpha))
            for i in range(t):
                clusterLoss = torch.tensor(0.0, requires_grad=True).to(device)
                predict = []
                fX = X.mm(W1).mm(W2).mm(W3)
                if U is None:
                    fXmax = Variable(torch.max(fX), requires_grad=False)
                    fXmin = Variable(torch.min(fX), requires_grad=False)
                    U = Variable((torch.rand(K, DH3)*(fXmax-fXmin)+fXmin).cuda(), requires_grad=True)
                for x in fX:
                    D = (x.reshape(DH3, 1).mm(torch.ones(1, K).to(device)).t()-U).pow(2).sum(1)
                    expD = (-1*D*alpha).exp()+1e-32
                    W = expD/expD.sum()
                    predict.append(max(range(len(W)), key=W.__getitem__))
                    clusterLoss = clusterLoss + (D*W).sum()
                #clusterLoss = clusterLoss/N
                #calculate accuracy
                embedingLoss = (fX.mm(W3.pinverse()).mm(W2.pinverse()).mm(W1.pinverse())-X).pow(2).sum()
                loss = (embedingLoss+lossLambda*clusterLoss)/N
                accuracy.append(0)
                clusters = [[] for i in range(K)]
                for yp, y in zip(predict, Y):
                    clusters[yp].append(y)
                for cluster in clusters:
                    labelCnt = [0]*K
                    for c in cluster:
                        labelCnt[c] += 1
                    accuracy[-1] += max(labelCnt)
                accuracy[-1]/=len(X)

                if U.grad is not None: U.grad.data.zero_()
                if W1.grad is not None: W1.grad.data.zero_()
                if W2.grad is not None: W2.grad.data.zero_()
                if W3.grad is not None: W3.grad.data.zero_()

                loss.backward()
                U.data -= learningRate*U.grad.data
                W1.data -= learningRate*W1.grad.data
                W2.data -= learningRate*W2.grad.data
                W3.data -= learningRate*W3.grad.data

                #loss.backward()
                #optimizer.step()
                print('Round {}: loss = {}; accuracy= {}; U = '.format(i+1, loss.data, accuracy[-1]))
                print(U)
                data = U.grad.data
                f.write('{} {}\n'.format(loss.data, accuracy[-1]))
task0_method1()