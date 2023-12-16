% STM_Serial_Testing
%
% Send via serial port to the STM32 microcontroller a set of data for
% testing purpose.
%
% Data are loaded from an external .txt file.
% Each line is sent as a series of singular character (1 Byte).
%
% Author: Windex team
% Date: 15/12/2023

clc
clear variables
close all

%% Settings / Data loading

settings = struct( ...
    "file_name", "2023-12-15_datalogs.txt", ...
    "COM_port", "COM6", ...
    "baundrate", 4800, ...
    "pause_time", 0.1);

file_ID = fopen(settings.file_name, "r");
assert(file_ID ~= -1, "Error opening the file.");

serial = serialport( ...
    settings.COM_port, ...
    settings.baundrate);


%% Data sending

out = [];
try
    while ~feof(file_ID)
        line = fgetl(file_ID);
        for character = num2cell(line)
            write(serial, character{1}, "char");
            out(end+1) = read(serial, 1, "char");
        end
    end
catch
    fclose(file_ID);
    error("Error reading and sending data.");
end

fclose(file_ID);
char(out)
