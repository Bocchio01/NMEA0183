% Telemetry
%
% Manages serial communication and telemetry loop cycles for various sensors
% connected to a microcontroller, transmitting data using the NMEA0183 library.
%
% The outcome of the script is a visual representation of incoming data.
%
% Instructions:
% Adjust the Settings.m file to match your specific configuration.
% Connect the external microcontroller running the NMEA0183 library.
%
% Author: Windex team
% Date: 20/12/2023

clc
clear variables
close all

%% Settings / Configuartion

run("Settings.m")

serial = serialport( ...
    settings.serial.port, ...
    settings.serial.baudrate, ...
    "Timeout", settings.serial.timeout);

configureTerminator(serial, settings.serial.terminator)
configureCallback(serial, "terminator", @readSerialCallback)


%% Sensors classes initialization

myMWV = MWV(settings.telemetryFig);
myXDR = XDR(settings.telemetryFig);
drawnow;


%% Telemetry loop

while true
    if ~isempty(serial.UserData)
        try
            if (exist('myMWV', 'var') && serial.UserData.startsWith("MWV"))
                myMWV.Parser(serial.UserData);
                myMWV.Plotter();
            end
            if (exist('myXDR', 'var') && serial.UserData.startsWith("XDR"))
                myXDR.Parser(serial.UserData);
                myXDR.Plotter();
            end
        catch
            disp("Error parsing data.")
            continue
        end
    end
end


%% Functions

function readSerialCallback(serial, ~)

serial.UserData = readline(serial);

end


