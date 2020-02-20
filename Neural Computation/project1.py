import torch, math
from torch.autograd import Variable

X = Variable(torch.cat((torch.randn(2000, 2), torch.randn(1500, 2) + 13, torch.mm(torch.randn(1000, 2) + 6,3 * (torch.rand(2, 2) - 0.2))), 0).cuda(), requires_grad=False)
K = 3
Xmax = Variable(torch.max(X), requires_grad=False)
Xmin = Variable(torch.min(X), requires_grad=False)
X = (X-Xmin)/(Xmax-Xmin)
with open('toy-input.txt', 'w') as f:
    f.write('# x y\n')
    for x in X:
        f.write('{} {} \n'.format(x[0], x[1]))
Alpha = [1, 10, 100]
T = [100, 300, 500]
U = Variable((torch.rand(K, 2)).cuda(), requires_grad=True)
learningRate = 1e-5
device = torch.device("cuda")
with open('toy-output.txt', 'w') as f:
    f.write('# u1x u1y u2x u2y u3x u3y\n')
    for alpha, t in zip(Alpha, T):
        print('Alpha: {}'.format(alpha))
        for i in range(t):
            loss = torch.tensor(0.0, requires_grad=True).to(device)
            for x in X:
                D = (x.reshape(2, 1).mm(torch.ones(1, K).to(device)).t()-U).pow(2).sum(1)
                expD = (-1*D*alpha).exp()+1e-32
                loss = loss + (D*expD/expD.sum()).sum()
            if U.grad is not None: U.grad.data.zero_() # why call this?
            loss.backward()
            U.data -= learningRate*U.grad.data
            print('Round {}: U = '.format(i+1))
            print(U)
            data = U.data
            f.write('{} {} {} {} {} {}\n'.format(data[0][0], data[0][1], data[1][0], data[1][1], data[2][0], data[2][1]))
