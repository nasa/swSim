
clear;clc;close all

path = '~/UT_SIM_CODES/swsim/testing/180x180ElongatedQausiIsoCFRPTestPlate/';

tStart = 0;
tStep = 50;
tStop = 20000;

tLen = length(tStart:tStep:tStop);

cellSlice = 1;

modelParams.NX = 721-1;
modelParams.NY = 721-1;
modelParams.NZ = 25-1;

dataCube = zeros(tLen, modelParams.NY, modelParams.NX);

tInc=1;
for t = tStart:tStep:tStop

    fileName = [path 'time_' int2str(t) '_Rank_V2_721x721x25.raw'];
    
    fileID= fopen(fileName);
    nodes = fread(fileID,'double');
    fclose(fileID);
    
    nodes = reshape(nodes,modelParams.NZ+1,modelParams.NY+1,modelParams.NX+1);    
    
    cells = zeros(modelParams.NZ, modelParams.NY, modelParams.NX);
    
    for i=1:modelParams.NX
        for j=1:modelParams.NY
            for k=1:modelParams.NZ

                cells(k,j,i) = cells(k,j,i) + nodes(k,j,i) * 0.125;
                cells(k,j,i) = cells(k,j,i) + nodes(k,j,i+1) * 0.125;
                cells(k,j,i) = cells(k,j,i) + nodes(k,j+1,i) * 0.125;
                cells(k,j,i) = cells(k,j,i) + nodes(k+1,j,i) * 0.125;
                cells(k,j,i) = cells(k,j,i) + nodes(k,j+1,i+1) * 0.125;
                cells(k,j,i) = cells(k,j,i) + nodes(k+1,j,i+1) * 0.125;
                cells(k,j,i) = cells(k,j,i) + nodes(k+1,j+1,i) * 0.125;
                cells(k,j,i) = cells(k,j,i) + nodes(k+1,j+1,i+1) * 0.125;
                
            end
        end
    end
    
    cells = squeeze(cells(cellSlice,:,:));
    
    dataCube(tInc,:,:) = cells;
    tInc = tInc+1;
    
end

tPlotSlice = round(tLen*0.5);
imagesc(squeeze(dataCube(tPlotSlice,:,:)));

dataCube = permute(dataCube,[1 3 2]);

transducer.centerX = 360;
transducer.centerY = 360;
transducer.radius = 75;

dataCube = tukeyTaperDataCubeFaces(dataCube, [25 25 25]);
dataCube = tukeyWindowOutTransducer(dataCube, transducer, 25);

fSize = 512;
kxSize = 1024;
kySize = 1024;

modelParams.dx = 0.25e-3;
modelParams.dy = 0.25e-3;

modelParams.dt = 3.2453e-9;
modelParams.dtOutput = modelParams.dt*tStep;

freqSlice = 200e3;

plotFreqWaveNum3D(dataCube, modelParams, freqSlice, [fSize kxSize kySize]);

