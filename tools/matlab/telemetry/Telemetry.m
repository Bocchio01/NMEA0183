% Telemetry
%
% Manages telemetry loop cycles for various sensors connected to a
% microcontroller, transmitting data using the NMEA0183 library.
%
% The outcome of the script is a visual representation of incoming data.
%
% Instructions:
% Make sure you have installed 'Realterm' program on your PC
% Adjust the Common/Settings.m file to match your specific configuration.
% Connect the external microcontroller running the NMEA0183 library.
%
% Author: Windex team
% Date: 20/12/2023

clc
if exist('handlerRT','var') == 1
    closeRealterm(handlerRT)
end
clear variables
close all

%% Settings / Configuartion

settings = loadSettings();
handlerRT = configRealterm(settings.realterm);

try
    set(handlerRT, "PortOpen", 1)
catch
    closeRealterm(handlerRT)
    close all
    error("Check your selected COM port from settings. Must be a char number (i.e. '6')")
end


%% Sensors classes initialization

myMWV = MWV(settings.telemetry.parent_figure);
myXDR = XDR(settings.telemetry.parent_figure);
drawnow;


%% Realterm launching

currentPath = pwd;
currentDate = string(datetime("now", "Format", 'yyyy-MM-dd'));
loggerFileName = fullfile(currentPath, "\Telemetry\Logs\", currentDate +".txt");
% senderFileName = fullfile(currentPath, "\Telemetry\Logs\", "2023-12-15" +".txt");

% set(handlerRT, "CaptureFile", loggerFileName)
set(handlerRT, "SendFile", senderFileName)

invoke(handlerRT, "StartCapture")
% set(handlerRT, "Send", 1)


%% Telemetry loop

prevFileSeek = 0;
incopleteData = {''};

while(1)

    try
        pause(settings.telemetry.pooling_delay);

        if(prevFileSeek == handlerRT.CharCount)
            continue
        end

        fileID = fopen(loggerFileName, 'r');
        fseek(fileID, prevFileSeek, 'bof');
        fileContent = char(fread(fileID, '*uint8')');
        prevFileSeek = ftell(fileID);
        fclose(fileID);

        splitResult = strsplit(fileContent, '-----------------------\n');

        splitResult{1} = [incopleteData{1}, splitResult{1}];

        incopleteData = splitResult(end);

        for line = splitResult

            if startsWith(line{1}, "MWV")
                myMWV.Parser(line{1});
                myMWV.Plotter();
            elseif startsWith(line{1}, "XDR")
                myXDR.Parser(line{1});
                myXDR.Plotter();
            end
        end

    catch
        disp("Error parsing data.")
        continue
    end

end

