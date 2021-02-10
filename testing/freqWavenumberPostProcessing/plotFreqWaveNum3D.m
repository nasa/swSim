
function [] = plotFreqWaveNum3D(dataCube, modelParams, freqSlice, padding)

fSize = padding(1);
kxSize = padding(2);
kySize = padding(3);

dt = modelParams.dtOutput;

kxkyf = fftn(dataCube,[fSize kxSize kySize]);
kxkyf = fftshift(kxkyf); % shift half spaces to center 3D FFT

fStep = 1/fSize/dt;
kxStep = 1/kxSize/modelParams.dx;
kyStep = 1/kySize/modelParams.dy;

f = (-fSize/2:fSize/2-1)*fStep;
kx = (-kxSize/2:kxSize/2-1)*kxStep;
ky = (-kySize/2:kySize/2-1)*kyStep; 

% Frequency slice at nearest neighbor
sliceNum = find( abs(f-freqSlice) == min( abs(f-freqSlice)) , 1 , 'first');
disp(f(sliceNum))

slice = squeeze(kxkyf(sliceNum,:,:));
normSlice = slice./max(abs(slice(:)));
invNormSlice = 1-abs(normSlice);

% data along kx=0 and ky=0 lines
xInd = round(size(invNormSlice,1)/2+1);
yInd = round(size(invNormSlice,2)/2+1);

xSlice = squeeze(invNormSlice(xInd,:));
ySlice = squeeze(invNormSlice(:,yInd));

%% Plotting
stepEnd = round(modelParams.dtOutput/modelParams.dt * size(dataCube,1));

dataStr = ['_fSize' int2str(fSize) '_kxSize' int2str(kxSize) '_kySize' int2str(kySize)];

%% plotting, freq slice

h(1) = figure;
g=pcolor(kx,ky,invNormSlice);
g.ZData = g.CData;
set(g,'linestyle','none');colormap('Gray');axis equal

%% plotting, kx at ky=0 line
h(2) = figure;
plot(kx,xSlice);
xlabel('kx Wavenumber')
ylabel('Inverted, normalized Amplitude')

%% plotting, ky at kx=0 line
h(3) = figure;
plot(ky,ySlice);
xlabel('ky Wavenumber')
ylabel('Inverted, normalized Amplitude')

%%
disp('kx and ky discretizations')
display(kxStep)
display(kyStep)

end