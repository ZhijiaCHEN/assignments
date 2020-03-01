import torch, math
from torch.autograd import Variable
from torch import optim
import torchvision
import torchvision.transforms as transforms
import torchvision.datasets as datasets

device = torch.device("cuda")
train_loader = torch.utils.data.DataLoader(torchvision.datasets.MNIST('./minist/', train=True, download=True, transform=torchvision.transforms.Compose([torchvision.transforms.ToTensor(), transforms.Normalize([0.5], [0.5])])), batch_size=1000, shuffle=False)

def my_normalize(x):
    return (x-x.mean())/x.std()

def KL_loss(X, U, device = None):
    if device is None:
        loss = torch.tensor(0.0, requires_grad=True)
    else:
        loss = torch.tensor(0.0, requires_grad=True).to(device)
    Q = []
    for xi in X:
        Di = (xi.reshape(2, 1).mm(torch.ones(1, U.size()[0]).to(device)).t()-U).pow(2).sum(1)
        Qi = (1+Di).pow(-1)/(1+Di).pow(-1).sum()
        Q.append(Qi)
    F = sum(Q)
    P = [(Qi.pow(2)/F)/(Qi.pow(2)/F).sum() for Qi in Q]
    loss = sum([(Pi*(Pi/Qi).log()).sum() for Pi, Qi in zip(P, Q)])
    return loss

def task0_method1():
    X = Variable(torch.cat((torch.randn(2000, 2), torch.randn(1500, 2) + 13, torch.mm(torch.randn(1000, 2) + 6,3 * (torch.rand(2, 2) - 0.2))), 0).cuda(), requires_grad=False)
    K = 3
    Xmax = Variable(torch.max(X), requires_grad=False)
    Xmin = Variable(torch.min(X), requires_grad=False)
    X = (X-X.mean())/X.std()
    UBackup = Variable((torch.randn(K, 2)).cuda(), requires_grad=False)
    alpha = [1, 10]
    T = [100, 100]
    for alpha, t in zip(alpha, T):
        U = UBackup.clone()
        U.requires_grad_(True)
        learningRate = 1e-1
        device = torch.device("cuda")
        #optimizer = None
        optimizer = optim.SGD([U], lr=learningRate, weight_decay=1e-5)
        with open('task0_method1-alpha-{}-input.txt'.format(alpha), 'w') as f:
            f.write('# x y\n')
            for x in X:
                f.write('{} {} \n'.format(x[0], x[1]))
        with open('task0_method1-alpha-{}-output.txt'.format(alpha), 'w') as f:
            f.write('# u1x u1y u2x u2y u3x u3y loss\n')
            for i in range(t):
                loss = torch.tensor(0.0, requires_grad=True).to(device)
                for x in X:
                    D = (x.reshape(2, 1).mm(torch.ones(1, K).to(device)).t()-U).pow(2).sum(1)
                    expD = (-1*D*alpha).exp()+1e-32
                    W = expD/expD.sum()
                    loss = loss + (D*W).sum()
                loss = loss/X.numel()
                #loss = KL_loss(X, U, device)

                if U.grad is not None: U.grad.data.zero_()
                loss.backward()
                if optimizer is None:
                    U.data -= learningRate*U.grad.data
                else:
                    optimizer.step()
                print('Round {}: loss = {}; U = '.format(i+1, loss.data))
                print(U)
                data = U.data
                f.write('{} {} {} {} {} {} {}\n'.format(data[0][0], data[0][1], data[1][0], data[1][1], data[2][0], data[2][1], loss.data))

def task1_method1():
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
    alpha = [1, 3]
    T = [50, 100]
    U = Variable((torch.rand(K, DIn)).cuda(), requires_grad=True)
    learningRate = 0.1

    optimizer = optim.SGD([U], lr=learningRate)
    with open('task1_method1.txt', 'w') as f:
        f.write('# loss accuracy\n')
        accuracy = []
        for alpha, t in zip(alpha, T):
            print('alpha: {}'.format(alpha))
            for i in range(t):
                loss = torch.tensor(0.0, requires_grad=True).to(device)
                predict = []
                for x in X:
                    D = (x.reshape(DIn, 1).mm(torch.ones(1, K).to(device)).t()-U).pow(2).sum(1)/DIn
                    expD = (-1*D*alpha).exp()+1e-32
                    W = expD/expD.sum()
                    predict.append(max(range(len(W)), key=W.__getitem__))
                    loss = loss + (D*W).sum()
                #loss = loss/N
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
                #loss.backward()
                #U.data -= learningRate*U.grad.data

                loss.backward()
                optimizer.step()
                print('Round {}: loss = {}; accuracy= {}; U = '.format(i+1, loss.data, accuracy[-1]))
                print(U)
                data = U.grad.data
                f.write('{} {}\n'.format(loss.data, accuracy[-1]))

def task1_method2():
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
    alpha = [50, 100]
    T = [100, 100]
    U = Variable((torch.rand(K, DIn)).cuda(), requires_grad=True)
    W = Variable((torch.rand(N, K)).cuda(), requires_grad=True)
    learningRate = 0.1

    #optimizer = optim.SGD([U], lr=learningRate)
    with open('task1_method2.txt', 'w') as f:
        f.write('# loss accuracy\n')
        accuracy = []
        for alpha, t in zip(alpha, T):
            print('alpha: {}'.format(alpha))
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
    examples = enumerate(train_loader)
    batch_idx, (example_data, example_targets) = next(examples)
    X = example_data.view(-1,784).to(device)
    X.requires_grad_(False)
    Y = example_targets.to(device)
    Y.requires_grad_(False)

    N = X.size()[0]
    DIn = X.size()[1]
    DH1 = 128
    DH2 = 64
    DH3 = 12
    DH4 = 3
    K = 10
    alpha = 5
    T = 1000
    W1 = Variable((torch.randn(DIn, DH1)).cuda(), requires_grad=True)
    b1 = Variable((torch.randn(1, DH1)).cuda(), requires_grad=True)
    W2 = Variable((torch.randn(DH1, DH2)).cuda(), requires_grad=True)
    b2 = Variable((torch.randn(1, DH2)).cuda(), requires_grad=True)
    W3 = Variable((torch.randn(DH2, DH3)).cuda(), requires_grad=True)
    b3 = Variable((torch.randn(1, DH3)).cuda(), requires_grad=True)
    W4 = Variable((torch.randn(DH3, DH4)).cuda(), requires_grad=True)
    b4 = Variable((torch.randn(1, DH4)).cuda(), requires_grad=True)
    U = Variable(torch.randn(K, DIn).cuda().mm(W1).mm(W2).mm(W3), requires_grad=True)
    lossLambda = 0.5
    learningRate = 1e-2
    optimizer = None
    optimizer = optim.Adam([W1, W2, W3, W4, b1, b2, b3, b4], lr=learningRate, weight_decay=1e-5)
    with open('task2_method1.txt', 'w') as f:
        #f.write('# loss accuracy\n')
        accuracy = []
        #for i in range(T):
        embedingLoss = None
        while embedingLoss is None or embedingLoss > 1e-3:
            clusterLoss = torch.tensor(0.0, requires_grad=True).to(device)
            predict = []
            fX = (((X.mm(W1) + b1.repeat(N, 1)).relu().mm(W2) + b2.repeat(N, 1)).relu().mm(W3) + b3.repeat(N, 1)).relu().mm(W4) + b4.repeat(N, 1)
            #if U is None: # initialize U after first embedding computation
            #    fXmax = Variable(torch.max(fX), requires_grad=False)
            #    fXmin = Variable(torch.min(fX), requires_grad=False)
            #    U = Variable((torch.rand(K, DH3)*(fXmax-fXmin)+fXmin).cuda(), requires_grad=True)

            """
            for x in fX:
                D = (x.repeat(K, 1)-U).pow(2).sum(1)
                #D = D-D.min()
                expD = (-1*D*alpha).exp()+1e-32
                W = expD/expD.sum()
                predict.append(max(range(len(W)), key=W.__getitem__))
                clusterLoss = clusterLoss + (D*W).sum()
            clusterLoss = clusterLoss/N
            """
            #calculate accuracy

            embedingLoss = ((((fX.mm(W4.t()) + b3.repeat(N, 1)).relu().mm(W3.t()) + b2.repeat(N, 1)).relu().mm(W2.t()) + b1.repeat(N, 1)).relu().mm(W1.t()).tanh()-X).pow(2).sum()/X.numel()
            
            #loss = embedingLoss+lossLambda*clusterLoss
            loss = embedingLoss
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
            if optimizer is None:
                #U.data -= learningRate*U.grad.clamp(-1, 1).data
                W1.data -= learningRate*W1.grad.clamp(-1, 1).data
                W2.data -= learningRate*W2.grad.clamp(-1, 1).data
                W3.data -= learningRate*W3.grad.clamp(-1, 1).data
                b1.data -= learningRate*b1.grad.clamp(-1, 1).data
                b2.data -= learningRate*b2.grad.clamp(-1, 1).data
                b3.data -= learningRate*b3.grad.clamp(-1, 1).data
            else:
                optimizer.step()
            print('cluster loss: {}, embeding loss: {}, accuracy = {}'.format(clusterLoss.data, embedingLoss.data, accuracy[-1]))
            #print('Round {}: loss = {}; accuracy= {}; U = '.format(i+1, loss.data, accuracy[-1]))
            #print(U)
            #data = U.grad.data
            #f.write('{} {}\n'.format(loss.data, accuracy[-1]))
        f.write('# x y k\n')
        for x,y in zip(fX, Y):
            f.write('{} {} {}\n'.format(x[0], x[1], y))
fun = task0_method1
detectAnomaly = False
if detectAnomaly:
    with torch.autograd.detect_anomaly():
        fun()
else:
    fun()
