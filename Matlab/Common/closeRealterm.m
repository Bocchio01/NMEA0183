function [] = closeRealterm(handlerRT)
%% RealTerm configuration

try
    if handlerRT.PortOpen == 1
        invoke(handlerRT, "Close");
    end
catch
    disp('Unable to close Realterm process');
end

end