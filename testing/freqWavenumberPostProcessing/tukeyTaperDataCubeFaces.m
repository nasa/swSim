function dataCube = tukeyTaperDataCubeFaces(dataCube, taperPercents)


windowFreq = tukeywin(size(dataCube,1),taperPercents(1)/100 * 2);
windowXdir = tukeywin(size(dataCube,2),taperPercents(2)/100 * 2);
windowYdir = tukeywin(size(dataCube,3),taperPercents(3)/100 * 2);

parfor i = 1:length(windowFreq)
    dataSlice = squeeze(dataCube(i,:,:));
    for j = 1:length(windowXdir)
        for k = 1:length(windowYdir)
            
            dataSlice(j,k) = dataSlice(j,k) * windowFreq(i) * windowXdir(j) * windowYdir(k); 
            
        end
    end
    dataCube(i,:,:) = dataSlice;
end

end