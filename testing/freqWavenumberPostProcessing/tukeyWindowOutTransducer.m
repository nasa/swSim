function dataCube = tukeyWindowOutTransducer(dataCube, transducer, twoSidedTaperPercent)

windowSize = transducer.radius * (1+twoSidedTaperPercent/100);
windowSize = ceil(windowSize);

% Use half of a Tukey window for transducer radius (rather than diameter)
windowTransducer = tukeywin(windowSize, twoSidedTaperPercent/100*2);
peakInd = find(windowTransducer==max(windowTransducer),1,'first');
windowTransducer(1:peakInd-1) = 1;

for j = 0:transducer.radius
    for k = 0:transducer.radius

        dist = sqrt(j^2+k^2);

        if dist <= transducer.radius

            if dist==0
                windowVal = 0;
            elseif dist ==transducer.radius
                windowVal = 1;
            else
                x1 = floor(dist);
                x2 = ceil(dist);
                y1 = windowTransducer(x1);
                y2 = windowTransducer(x2);
                
                if(x1==x2)
                    windowVal = 1-y1;
                
                else
                    windowVal = y1 + (y2-y1)/(x2-x1) * (dist-x1);
                    windowVal = 1-windowVal;
                end
                
            end
            
            if j==0
                dataCube(:,transducer.centerX, transducer.centerY-k) = dataCube(:,transducer.centerX, transducer.centerY-k) * windowVal;
                dataCube(:,transducer.centerX, transducer.centerY+k) = dataCube(:,transducer.centerX, transducer.centerY+k) * windowVal;
            
            elseif k==0
                dataCube(:,transducer.centerX-j, transducer.centerY) = dataCube(:,transducer.centerX-j, transducer.centerY) * windowVal;
                dataCube(:,transducer.centerX+j, transducer.centerY) = dataCube(:,transducer.centerX+j, transducer.centerY) * windowVal;
            
            else
                dataCube(:,transducer.centerX-j, transducer.centerY-k) = dataCube(:,transducer.centerX-j, transducer.centerY-k) * windowVal;
                dataCube(:,transducer.centerX-j, transducer.centerY+k) = dataCube(:,transducer.centerX-j, transducer.centerY+k) * windowVal;
                dataCube(:,transducer.centerX+j, transducer.centerY-k) = dataCube(:,transducer.centerX+j, transducer.centerY-k) * windowVal;
                dataCube(:,transducer.centerX+j, transducer.centerY+k) = dataCube(:,transducer.centerX+j, transducer.centerY+k) * windowVal;
            end

        end

    end
end

end