stockData = readtable('data_1950.csv');
Date = datetime(arrayfun(@string, stockData.Date),'InputFormat','yyyy/MM/dd');
SP500 = stockData.SP500;
DividenDYield = stockData.DividendYield;
k = 12;
Date = Date(k+1:end);
SP500kYearsIncrease = (SP500(k+1:end)-SP500(1:end-k))./SP500(1:end-k);

%EDA

figure;
plot(Date, SP500kYearsIncrease);
maxLag = 15;
figure;
autocorr(SP500kYearsIncrease, 'NumLags',maxLag);
[acf,acfLags,acfBounds] = autocorr(SP500kYearsIncrease, 'NumLags',maxLag);
figure;
parcorr(SP500kYearsIncrease, 'NumLags',maxLag);
[pacf,pacfLags,pacfBounds] = parcorr(SP500kYearsIncrease, 'NumLags',maxLag);


%split train and test data
trainYearNum = 50;
trainDate = Date(1:trainYearNum*12);
trainY = SP500kYearsIncrease(1:trainYearNum*12);
trainY = trainY(:);
c = mean(trainY);

minAic = inf;
%estimate ARMA model
MALags = (1:9);
ARLags = pacfLags(abs(pacf) > abs(pacfBounds(1)));
ARLags = ARLags(2:end);%the first element is 0 lag

for mri = 1:length(MALags)
    MALagsChosen = MALags(1:mri);
    MALagsChosen = MALagsChosen(:)';
    for ari = 1:length(ARLags)
        try
            ARLagsChosen = ARLags(1:ari);
            ARLagsChosen = ARLagsChosen(:)';
            model = arima('ARLags',ARLagsChosen,'MALags',MALagsChosen,'Constant',c);
            [estModel, ~, logL] = estimate(model,trainY);
            [aic,bic] = aicbic(logL, length(MALagsChosen)+length(ARLagsChosen), length(trainY));
            %[forecastY,YMSE] = forecast(estModel,testSize,trainY);
            %msqerr = sum((testY-forecastY).^2)/testSize;
            if aic < minAic
                minAic = aic;
                minBic = bic;
                bestModel = estModel;
                %bestForecast = forecastY;
                bestARLags = ARLagsChosen;
                bestMALags = MALagsChosen;
            end
        catch
            continue
        end
    end
end

testYearNum = 10;
testSize = testYearNum*12;
testDate10Year = Date(length(trainY)+1: length(trainY)+testSize);
testY10Year = SP500kYearsIncrease(length(trainY)+1: length(trainY)+testSize);


[forecastY10Year,YMSE] = forecast(bestModel,testSize,trainY);
msqerr = sum((testY10Year-forecastY10Year).^2)/testSize;
figure;
hold on;
trueDataHdl = plot(testDate10Year, testY10Year);
forecastDataHdl = plot(testDate10Year, forecastY10Year);
legend([trueDataHdl forecastDataHdl],'truth','forecast', 'Location','NorthWest');
hold off;

t1 = array2table(testDate10Year);
t2 = array2table(testY10Year);
t3 = array2table(forecastY10Year);
forecastData = [t1, t2, t3];

save('arma.mat','minAic', 'minBic', 'bestModel', 'bestARLags','bestMALags', 'forecastData');




        





