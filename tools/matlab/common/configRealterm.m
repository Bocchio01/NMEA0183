function handlerRT = configRealterm(realterm_settings)
%% RealTerm configuration
% set(hRealterm); List all properties setted by using set(hRealterm, "PROPERTY", VALUE);
% invoke(hRealterm); List all methods called by using invoke(hRealterm, "METHOD", PARAMETHER, ...);

% Launching a fresh istance of the RealTerm server
handlerRT = actxserver('realterm.realtermintf');

% RealTerm window settings
set(handlerRT, "Caption", realterm_settings.Caption)

% Display group settings
set(handlerRT, "LinefeedIsNewline", realterm_settings.LinefeedIsNewline)
set(handlerRT, "HalfDuplex", realterm_settings.HalfDuplex)
set(handlerRT, "DisplayAs", realterm_settings.DisplayAs)

% Port group settings
set(handlerRT, "Port", realterm_settings.Port)
set(handlerRT, "Baud", realterm_settings.Baud)

% Capture group settings
% CaptureFile must be set from launcher script
% set(hRealterm, "CaptureFile", "C:\Users\Bocchio\Documents\GitHub\Polimi_Sailing_Team\Windex\telemetry\out.txt")

% Send group settings
for SendString = realterm_settings.AddCannedSendString
    invoke(handlerRT, 'AddCannedSendString', SendString{1}, 1);
end

% SendFile must be set from launcher script
% set(handlerRT, "SendFile", "C:\Users\Bocchio\Documents\GitHub\Polimi_Sailing_Team\Windex\telemetry\Logger\2023-12-15.txt")
set(handlerRT, "CharDelay", realterm_settings.CharDelay)
set(handlerRT, "LineDelay", realterm_settings.LineDelay)
% How to add auto EOL to each Canned String from command?


%% Event configuration

% % Send some commands
% invoke(hRealterm, 'PutString', 'S 90 01 60 W 00 P'); % Init the TMP100.
% invoke(hRealterm, 'NewlineTerminal'); % Make screen neater
%
% CharsBeforeRequest = hRealterm.charcount;
% invoke(hRealterm, 'PutString', 'S 91 02 P'); % Read the 2-byte temperature from TMP100
% pause(0.1); % Wait for chars to return from adaptor
% CharsReceived = hRealterm.charcount - CharsBeforeRequest;

% procedure OnTimer; dispid 1;
% procedure OnCaptureCount; dispid 2;
% procedure OnCaptureStop; dispid 3;
% procedure OnDataTrigger(Index: Integer; Timeout: WordBool; Data: OleVariant; Size: Integer; Reenable: WordBool); dispid 201;
%
% There are several properties and methods to control the events:
%
% property TimerPeriod: Integer
% property EnableTimerCallbacks: WordBool
%
% procedure EnableDataTrigger(Index: Integer); dispid 202;
% procedure DisableDataTrigger(Index: Integer);dispid 203;
%
% const EndString: WideString;
% PacketSIze: Integer;
% Timeout: Integer;
% AutoEnable: WordBool;
% IgnoreCase: WordBool;
% IncludeStrings: WordBool);

% invoke(hRealterm, "EnableDataTrigger", 1)


