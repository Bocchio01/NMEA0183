clc
clear variable
close all

%% Generator

rng('shuffle');

currentPath = pwd;
currentDate = string(datetime("now", "Format", 'yyyy-MM-dd'));
filenameDataset = fullfile(currentPath, "\Testing\Datasets\", currentDate +".txt");

fileID = fopen(filenameDataset, 'w');
if fileID == -1
    error('Error opening file');
end

for i = 1:1000

    windAngle = rand * 359 - 0;
    windSpeed = rand * 100 - 0;
    sentence = sprintf('$IIMWV,%.1f,R,%.1f,N,A', windAngle, windSpeed);
    fprintf(fileID, '%s*%02X\n', sentence, computeChecksum(sentence));

    temperature = rand * 40 - 10;
    sentence = sprintf('$WIXDR,C,%.1f,C,,', temperature);
    fprintf(fileID, '%s*%02X\n', sentence, computeChecksum(sentence));

    fprintf(fileID, '$PLCJ,6AFE,6E00,AA,8373,5353\n');
    fprintf(fileID, '$PLCJE8BC8,69A8,2D00,2165,6F,\n');
end

fclose(fileID);


%% Functions

function checksum = computeChecksum(buffer)

checksum = 0;

for i = 2:length(buffer)
    checksum = uint16(bitxor(checksum, double(buffer(i))));
end

% checksum = double(checksum);
% checksum = dec2hex(checksum);

end
