stockData = readtable('data_1900.csv');
Date = datetime(arrayfun(@string, stockData.Date),'InputFormat','yyyy/MM/dd');
SP500 = stockData.SP500;
DividenDYield = stockData.DividendYield;
k = 10;
Date = Date(k+1:end);
SP500kYearsIncrease = (SP500(k+1:end)-SP500(1:end-k))./SP500(1:end-k);

%EDA

%figure;
%plot(Date, SP500kYearsIncrease);
maxLag = 15;
%figure;
%autocorr(SP500kYearsIncrease, 'NumLags',maxLag);
[acf,acfLags,acfBounds] = autocorr(SP500kYearsIncrease, 'NumLags',maxLag);
%figure;
%parcorr(SP500kYearsIncrease, 'NumLags',maxLag);
[pacf,pacfLags,pacfBounds] = parcorr(SP500kYearsIncrease, 'NumLags',maxLag);


%split train and test data
testYearNum = 1;
testSize = testYearNum*12;
trainDate = Date(1:end-testSize);
trainY = SP500kYearsIncrease(1:end-testSize);
trainY = trainY(:);
testDate = Date(end-testSize+1:end);
testY = SP500kYearsIncrease(end-testSize+1:end);

minErr = inf;
%estimate ARMA model
MALags = (1:9);
ARLags = pacfLags(abs(pacf) > abs(pacfBounds(1)));
ARLags = ARLags(2:end);%the first element is 0 lag
for mri = 1:length(MALags)
    MALagsChosen = MALags(1:mri);
    MALagsChosen = MALagsChosen(:)';
    for ari = 1:length(ARLags)
        ARLagsChosen = ARLags(1:ari);
        ARLagsChosen = ARLagsChosen(:)';
        model = arima('ARLags',ARLagsChosen,'MALags',MALagsChosen,'Constant',0);
        estModel = estimate(model,trainY);
        [forecastY,YMSE] = forecast(estModel,testSize,trainY);
        msqerr = sum((testY-forecastY).^2)/testSize;
        if msqerr < minErr
            minErr = msqerr;
            bestModel = estModel;
            bestForecast = forecastY;
            bestARLags = ARLagsChosen;
            bestMALags = MALagsChosen;
        end   
    end
end

figure;
hold on;
trueDataHdl = plot(Date, SP500kYearsIncrease);
forecastDataHdl = plot(testDate, bestForecast);
legend([trueDataHdl forecastDataHdl],'truth','forecast', 'Location','NorthWest');
hold off;
save('arma.mat','minErr','bestModel','bestForecast','bestARLags','bestMALags');

        





